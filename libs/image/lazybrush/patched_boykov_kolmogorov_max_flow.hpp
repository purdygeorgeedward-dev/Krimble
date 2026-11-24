//  SPDX-FileCopyrightText: 2006 Stephan Diederich
//  This code may be used under either of the following two licences:
//  SPDX-License-Identifier: MIT OR BSL-1.0

#ifndef BOOST_BOYKOV_KOLMOGOROV_MAX_FLOW_HPP
#define BOOST_BOYKOV_KOLMOGOROV_MAX_FLOW_HPP

#include <boost/config.hpp>
#include <boost/assert.hpp>
#include <vector>
#include <list>
#include <utility>
#include <iosfwd>
#include <algorithm>

#include <boost/pending/queue.hpp>
#include <boost/limits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/none_t.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/lookup_edge.hpp>
#include <boost/concept/assert.hpp>

namespace boost {

namespace detail {

template <class Graph,
          class EdgeCapacityMap,
          class ResidualCapacityEdgeMap,
          class ReverseEdgeMap,
          class PredecessorMap,
          class ColorMap,
          class DistanceMap,
          class IndexMap>
class bk_max_flow {
  typedef typename property_traits<EdgeCapacityMap>::value_type tEdgeVal;
  typedef graph_traits<Graph> tGraphTraits;
  typedef typename tGraphTraits::vertex_iterator vertex_iterator;
  typedef typename tGraphTraits::vertex_descriptor vertex_descriptor;
  typedef typename tGraphTraits::edge_descriptor edge_descriptor;
  typedef typename tGraphTraits::edge_iterator edge_iterator;
  typedef typename tGraphTraits::out_edge_iterator out_edge_iterator;
  typedef boost::queue<vertex_descriptor> tQueue;
  typedef typename property_traits<ColorMap>::value_type tColorValue;
  typedef color_traits<tColorValue> tColorTraits;
  typedef typename property_traits<DistanceMap>::value_type tDistanceVal;

    public:
      bk_max_flow(Graph& g,
                  EdgeCapacityMap cap,
                  ResidualCapacityEdgeMap res,
                  ReverseEdgeMap rev,
                  PredecessorMap pre,
                  ColorMap color,
                  DistanceMap dist,
                  IndexMap idx,
                  vertex_descriptor src,
                  vertex_descriptor sink):
      m_g(g),
      m_index_map(idx),
      m_cap_map(cap),
      m_res_cap_map(res),
      m_rev_edge_map(rev),
      m_pre_map(pre),
      m_tree_map(color),
      m_dist_map(dist),
      m_source(src),
      m_sink(sink),
      m_active_nodes(),
      m_in_active_list_vec(num_vertices(g), false),
      m_in_active_list_map(make_iterator_property_map(m_in_active_list_vec.begin(), m_index_map)),
      m_has_parent_vec(num_vertices(g), false),
      m_has_parent_map(make_iterator_property_map(m_has_parent_vec.begin(), m_index_map)),
      m_time_vec(num_vertices(g), 0),
      m_time_map(make_iterator_property_map(m_time_vec.begin(), m_index_map)),
      m_flow(0),
      m_time(1),
      m_last_grow_vertex(graph_traits<Graph>::null_vertex()){
        vertex_iterator vi, v_end;
        for(boost::tie(vi, v_end) = vertices(m_g); vi != v_end; ++vi){
          set_tree(*vi, tColorTraits::gray());
        }
        edge_iterator ei, e_end;
        for(boost::tie(ei, e_end) = edges(m_g); ei != e_end; ++ei) {
          put(m_res_cap_map, *ei, get(m_cap_map, *ei));
          BOOST_ASSERT(get(m_rev_edge_map, get(m_rev_edge_map, *ei)) == *ei);
        }
        set_tree(m_source, tColorTraits::black());
        set_tree(m_sink, tColorTraits::white());
        put(m_time_map, m_source, 1);
        put(m_time_map, m_sink, 1);
      }

      tEdgeVal max_flow(){
        augment_direct_paths();
        while(true){
          bool path_found;
          edge_descriptor connecting_edge;
          boost::tie(connecting_edge, path_found) = grow();
          if(!path_found){
            break;
          }
          ++m_time;
          augment(connecting_edge);
          adopt();
        }
        return m_flow;
      }

    protected:
      void augment_direct_paths(){
        out_edge_iterator ei, e_end;
        for(boost::tie(ei, e_end) = out_edges(m_source, m_g); ei != e_end; ++ei){
          edge_descriptor from_source = *ei;
          vertex_descriptor current_node = target(from_source, m_g);
          if(current_node == m_sink){
            tEdgeVal cap = get(m_res_cap_map, from_source);
            put(m_res_cap_map, from_source, 0);
            m_flow += cap;
            continue;
          }
          edge_descriptor to_sink;
          bool is_there;
          boost::tie(to_sink, is_there) = lookup_edge(current_node, m_sink, m_g);
          if(is_there){
            tEdgeVal cap_from_source = get(m_res_cap_map, from_source);
            tEdgeVal cap_to_sink = get(m_res_cap_map, to_sink);
            if(cap_from_source > cap_to_sink){
              set_tree(current_node, tColorTraits::black());
              add_active_node(current_node);
              set_edge_to_parent(current_node, from_source);
              put(m_dist_map, current_node, 1);
              put(m_time_map, current_node, 1);
              put(m_res_cap_map, from_source, get(m_res_cap_map, from_source) - cap_to_sink);
              put(m_res_cap_map, to_sink, 0);
              m_flow += cap_to_sink;
            } else if(cap_to_sink > 0){
              set_tree(current_node, tColorTraits::white());
              add_active_node(current_node);
              set_edge_to_parent(current_node, to_sink);
              put(m_dist_map, current_node, 1);
              put(m_time_map, current_node, 1);
              put(m_res_cap_map, to_sink, get(m_res_cap_map, to_sink) - cap_from_source);
              put(m_res_cap_map, from_source, 0);
              m_flow += cap_from_source;
            }
          } else if(get(m_res_cap_map, from_source)){
            set_tree(current_node, tColorTraits::black());
            set_edge_to_parent(current_node, from_source);
            put(m_dist_map, current_node, 1);
            put(m_time_map, current_node, 1);
            add_active_node(current_node);
          }
        }
        for(boost::tie(ei, e_end) = out_edges(m_sink, m_g); ei != e_end; ++ei){
          edge_descriptor to_sink = get(m_rev_edge_map, *ei);
          vertex_descriptor current_node = source(to_sink, m_g);
          if(get(m_res_cap_map, to_sink)){
            set_tree(current_node, tColorTraits::white());
            set_edge_to_parent(current_node, to_sink);
            put(m_dist_map, current_node, 1);
            put(m_time_map, current_node, 1);
            add_active_node(current_node);
          }
        }
      }

      std::pair<edge_descriptor, bool> grow(){
        BOOST_ASSERT(m_orphans.empty());
        vertex_descriptor current_node;
        while((current_node = get_next_active_node()) != graph_traits<Graph>::null_vertex()){
          BOOST_ASSERT(get_tree(current_node) != tColorTraits::gray() &&
                       (has_parent(current_node) ||
                         current_node == m_source ||
                         current_node == m_sink));

          if(get_tree(current_node) == tColorTraits::black()){
            out_edge_iterator ei, e_end;
            if(current_node != m_last_grow_vertex){
              m_last_grow_vertex = current_node;
              boost::tie(m_last_grow_edge_it, m_last_grow_edge_end) = out_edges(current_node, m_g);
            }
            for(; m_last_grow_edge_it != m_last_grow_edge_end; ++m_last_grow_edge_it) {
              edge_descriptor out_edge = *m_last_grow_edge_it;
              if(get(m_res_cap_map, out_edge) > 0){
                vertex_descriptor other_node = target(out_edge, m_g);
                if(get_tree(other_node) == tColorTraits::gray()){
                  set_tree(other_node, tColorTraits::black());
                  set_edge_to_parent(other_node, out_edge);
                  put(m_dist_map, other_node, get(m_dist_map, current_node) + 1);
                  put(m_time_map, other_node, get(m_time_map, current_node));
                  add_active_node(other_node);
                } else if(get_tree(other_node) == tColorTraits::black()) {
                  if(is_closer_to_terminal(current_node, other_node)){
                    set_edge_to_parent(other_node, out_edge);
                    put(m_dist_map, other_node, get(m_dist_map, current_node) + 1);
                    put(m_time_map, other_node, get(m_time_map, current_node));
                  }
                } else{
                  BOOST_ASSERT(get_tree(other_node)==tColorTraits::white());
                  return std::make_pair(out_edge, true);
                }
              }
            }
          }
          else{
            BOOST_ASSERT(get_tree(current_node) == tColorTraits::white());
            out_edge_iterator ei, e_end;
            if(current_node != m_last_grow_vertex){
              m_last_grow_vertex = current_node;
              boost::tie(m_last_grow_edge_it, m_last_grow_edge_end) = out_edges(current_node, m_g);
            }
            for(; m_last_grow_edge_it != m_last_grow_edge_end; ++m_last_grow_edge_it){
              edge_descriptor in_edge = get(m_rev_edge_map, *m_last_grow_edge_it);
              if(get(m_res_cap_map, in_edge) > 0){
                vertex_descriptor other_node = source(in_edge, m_g);
                if(get_tree(other_node) == tColorTraits::gray()){
                  set_tree(other_node, tColorTraits::white());
                  set_edge_to_parent(other_node, in_edge);
                  add_active_node(other_node);
                  put(m_dist_map, other_node, get(m_dist_map, current_node) + 1);
                  put(m_time_map, other_node, get(m_time_map, current_node));
                } else if(get_tree(other_node) == tColorTraits::white()){
                  if(is_closer_to_terminal(current_node, other_node)){
                    set_edge_to_parent(other_node, in_edge);
                    put(m_dist_map, other_node, get(m_dist_map, current_node) + 1);
                    put(m_time_map, other_node, get(m_time_map, current_node));
                  }
                } else{
                  BOOST_ASSERT(get_tree(other_node)==tColorTraits::black());
                  return std::make_pair(in_edge, true);
                }
              }
            }
          }

          finish_node(current_node);
        }

        return std::make_pair(edge_descriptor(), false);
      }

      void augment(edge_descriptor e) {
        BOOST_ASSERT(get_tree(target(e, m_g)) == tColorTraits::white());
        BOOST_ASSERT(get_tree(source(e, m_g)) == tColorTraits::black());
        BOOST_ASSERT(m_orphans.empty());

        const tEdgeVal bottleneck = find_bottleneck(e);
        put(m_res_cap_map, e, get(m_res_cap_map, e) - bottleneck);
        BOOST_ASSERT(get(m_res_cap_map, e) >= 0);
        put(m_res_cap_map, get(m_rev_edge_map, e), get(m_res_cap_map, get(m_rev_edge_map, e)) + bottleneck);

        vertex_descriptor current_node = source(e, m_g);
        while(current_node != m_source){
          edge_descriptor pred = get_edge_to_parent(current_node);
          const tEdgeVal new_pred_cap = get(m_res_cap_map, pred) - bottleneck;
          put(m_res_cap_map, pred, new_pred_cap);
          BOOST_ASSERT(get(m_res_cap_map, pred) >= 0);

          const edge_descriptor pred_rev = get(m_rev_edge_map, pred);
          put(m_res_cap_map, pred_rev, get(m_res_cap_map, pred_rev) + bottleneck);

          if(new_pred_cap == 0){
            set_no_parent(current_node);
            m_orphans.push_front(current_node);
          }
          current_node = source(pred, m_g);
        }
        current_node = target(e, m_g);
        while(current_node != m_sink){
          edge_descriptor pred = get_edge_to_parent(current_node);
          const tEdgeVal new_pred_cap = get(m_res_cap_map, pred) - bottleneck;
          put(m_res_cap_map, pred, new_pred_cap);
          BOOST_ASSERT(get(m_res_cap_map, pred) >= 0);

          const edge_descriptor pred_rev = get(m_rev_edge_map, pred);
          put(m_res_cap_map, pred_rev, get(m_res_cap_map, pred_rev) + bottleneck);

          if(new_pred_cap == 0){
            set_no_parent(current_node);
            m_orphans.push_front(current_node);
          }
          current_node = target(pred, m_g);
        }
        m_flow += bottleneck;
      }

      inline tEdgeVal find_bottleneck(edge_descriptor e){
        BOOST_USING_STD_MIN();
        tEdgeVal minimum_cap = get(m_res_cap_map, e);
        vertex_descriptor current_node = source(e, m_g);
        while(current_node != m_source){
          edge_descriptor pred = get_edge_to_parent(current_node);
          minimum_cap = min BOOST_PREVENT_MACRO_SUBSTITUTION(minimum_cap, get(m_res_cap_map, pred));
          current_node = source(pred, m_g);
        }
        current_node = target(e, m_g);
        while(current_node != m_sink){
          edge_descriptor pred = get_edge_to_parent(current_node);
          minimum_cap = min BOOST_PREVENT_MACRO_SUBSTITUTION(minimum_cap, get(m_res_cap_map, pred));
          current_node = target(pred, m_g);
        }
        return minimum_cap;
      }

      void adopt(){
        while(!m_orphans.empty() || !m_child_orphans.empty()){
          vertex_descriptor current_node;
          if(m_child_orphans.empty()){
            current_node = m_orphans.front();
            m_orphans.pop_front();
          } else{
            current_node = m_child_orphans.front();
            m_child_orphans.pop();
          }
          if(get_tree(current_node) == tColorTraits::black()){
            tDistanceVal min_distance = (std::numeric_limits<tDistanceVal>::max)();
            edge_descriptor new_parent_edge;
            out_edge_iterator ei, e_end;
            for(boost::tie(ei, e_end) = out_edges(current_node, m_g); ei != e_end; ++ei){
              const edge_descriptor in_edge = get(m_rev_edge_map, *ei);
              BOOST_ASSERT(target(in_edge, m_g) == current_node);
              if(get(m_res_cap_map, in_edge) > 0){
                vertex_descriptor other_node = source(in_edge, m_g);
                if(get_tree(other_node) == tColorTraits::black() && has_source_connect(other_node)){
                  if(get(m_dist_map, other_node) < min_distance){
                    min_distance = get(m_dist_map, other_node);
                    new_parent_edge = in_edge;
                  }
                }
              }
            }
            if(min_distance != (std::numeric_limits<tDistanceVal>::max)()){
              set_edge_to_parent(current_node, new_parent_edge);
              put(m_dist_map, current_node, min_distance + 1);
              put(m_time_map, current_node, m_time);
            } else{
              put(m_time_map, current_node, 0);
              for(boost::tie(ei, e_end) = out_edges(current_node, m_g); ei != e_end; ++ei){
                edge_descriptor in_edge = get(m_rev_edge_map, *ei);
                vertex_descriptor other_node = source(in_edge, m_g);
                if(get_tree(other_node) == tColorTraits::black() && other_node != m_source){
                  if(get(m_res_cap_map, in_edge) > 0){
                    add_active_node(other_node);
                  }
                  if(has_parent(other_node) && source(get_edge_to_parent(other_node), m_g) == current_node){
                    set_no_parent(other_node);
                    m_child_orphans.push(other_node);
                  }
                }
              }
              set_tree(current_node, tColorTraits::gray());
            }
          }
          else{
            BOOST_ASSERT(get_tree(current_node) == tColorTraits::white());
            out_edge_iterator ei, e_end;
            edge_descriptor new_parent_edge;
            tDistanceVal min_distance = (std::numeric_limits<tDistanceVal>::max)();
            for(boost::tie(ei, e_end) = out_edges(current_node, m_g); ei != e_end; ++ei){
              const edge_descriptor out_edge = *ei;
              if(get(m_res_cap_map, out_edge) > 0){
                const vertex_descriptor other_node = target(out_edge, m_g);
                if(get_tree(other_node) == tColorTraits::white() && has_sink_connect(other_node))
                  if(get(m_dist_map, other_node) < min_distance){
                    min_distance = get(m_dist_map, other_node);
                    new_parent_edge = out_edge;
                  }
              }
            }
            if(min_distance != (std::numeric_limits<tDistanceVal>::max)()){
              set_edge_to_parent(current_node, new_parent_edge);
              put(m_dist_map, current_node, min_distance + 1);
              put(m_time_map, current_node, m_time);
            } else{
              put(m_time_map, current_node, 0);
              for(boost::tie(ei, e_end) = out_edges(current_node, m_g); ei != e_end; ++ei){
                const edge_descriptor out_edge = *ei;
                const vertex_descriptor other_node = target(out_edge, m_g);
                if(get_tree(other_node) == tColorTraits::white() && other_node != m_sink){
                  if(get(m_res_cap_map, out_edge) > 0){
                    add_active_node(other_node);
                  }
                  if(has_parent(other_node) && target(get_edge_to_parent(other_node), m_g) == current_node){
                    set_no_parent(other_node);
                    m_child_orphans.push(other_node);
                  }
                }
              }
              set_tree(current_node, tColorTraits::gray());
            }
          }
        }
      }

      inline vertex_descriptor get_next_active_node(){
        while(true){
          if(m_active_nodes.empty())
            return graph_traits<Graph>::null_vertex();
          vertex_descriptor v = m_active_nodes.front();

      if(!has_parent(v) && v != m_source && v != m_sink){
            m_active_nodes.pop();
            put(m_in_active_list_map, v, false);
          } else{
            BOOST_ASSERT(get_tree(v) == tColorTraits::black() || get_tree(v) == tColorTraits::white());
            return v;
          }
        }
      }

      inline void add_active_node(vertex_descriptor v){
        BOOST_ASSERT(get_tree(v) != tColorTraits::gray());
        if(get(m_in_active_list_map, v)){
          if (m_last_grow_vertex == v) {
              m_last_grow_vertex = graph_traits<Graph>::null_vertex();
          }
          return;
        } else{
          put(m_in_active_list_map, v, true);
          m_active_nodes.push(v);
        }
      }

      inline void finish_node(vertex_descriptor v){
        BOOST_ASSERT(m_active_nodes.front() == v);
        m_active_nodes.pop();
        put(m_in_active_list_map, v, false);
        m_last_grow_vertex = graph_traits<Graph>::null_vertex();
      }

      inline void remove_active_node(vertex_descriptor v){
        (void)v;
        BOOST_ASSERT(!has_parent(v));
      }

      inline tColorValue get_tree(vertex_descriptor v) const {
        return get(m_tree_map, v);
      }

      inline void set_tree(vertex_descriptor v, tColorValue t){
        put(m_tree_map, v, t);
      }

      inline edge_descriptor get_edge_to_parent(vertex_descriptor v) const{
        return get(m_pre_map, v);
      }

      inline bool has_parent(vertex_descriptor v) const{
        return get(m_has_parent_map, v);
      }

      inline void set_edge_to_parent(vertex_descriptor v, edge_descriptor f_edge_to_parent){
        BOOST_ASSERT(get(m_res_cap_map, f_edge_to_parent) > 0);
        put(m_pre_map, v, f_edge_to_parent);
        put(m_has_parent_map, v, true);
      }

      inline void set_no_parent(vertex_descriptor v){
        put(m_has_parent_map, v, false);
      }

      inline bool has_sink_connect(vertex_descriptor v){
        tDistanceVal current_distance = 0;
        vertex_descriptor current_vertex = v;
        while(true){
          if(get(m_time_map, current_vertex) == m_time){
            current_distance += get(m_dist_map, current_vertex);
            break;
          }
          if(current_vertex == m_sink){
            put(m_time_map, m_sink, m_time);
            break;
          }
          if(has_parent(current_vertex)){
            current_vertex = target(get_edge_to_parent(current_vertex), m_g);
            ++current_distance;
          } else{
            return false;
          }
        }
        current_vertex=v;
        while(get(m_time_map, current_vertex) != m_time){
          put(m_dist_map, current_vertex, current_distance);
          --current_distance;
          put(m_time_map, current_vertex, m_time);
          current_vertex = target(get_edge_to_parent(current_vertex), m_g);
        }
        return true;
      }

      inline bool has_source_connect(vertex_descriptor v){
        tDistanceVal current_distance = 0;
        vertex_descriptor current_vertex = v;
        while(true){
          if(get(m_time_map, current_vertex) == m_time){
            current_distance += get(m_dist_map, current_vertex);
            break;
          }
          if(current_vertex == m_source){
            put(m_time_map, m_source, m_time);
            break;
          }
          if(has_parent(current_vertex)){
            current_vertex = source(get_edge_to_parent(current_vertex), m_g);
            ++current_distance;
          } else{
            return false;
          }
        }
        current_vertex=v;
        while(get(m_time_map, current_vertex) != m_time){
            put(m_dist_map, current_vertex, current_distance);
            --current_distance;
            put(m_time_map, current_vertex, m_time);
            current_vertex = source(get_edge_to_parent(current_vertex), m_g);
        }
        return true;
      }

      inline bool is_closer_to_terminal(vertex_descriptor p, vertex_descriptor q){
        return (get(m_time_map, q) <= get(m_time_map, p) &&
                get(m_dist_map, q) > get(m_dist_map, p)+1);
      }

      Graph& m_g;
      IndexMap m_index_map;
      EdgeCapacityMap m_cap_map;
      ResidualCapacityEdgeMap m_res_cap_map;
      ReverseEdgeMap m_rev_edge_map;
      PredecessorMap m_pre_map;
      ColorMap m_tree_map;
      DistanceMap m_dist_map;
      vertex_descriptor m_source;
      vertex_descriptor m_sink;

      tQueue m_active_nodes;
      std::vector<bool> m_in_active_list_vec;
      iterator_property_map<std::vector<bool>::iterator, IndexMap> m_in_active_list_map;

      std::list<vertex_descriptor> m_orphans;
      tQueue m_child_orphans;

      std::vector<bool> m_has_parent_vec;
      iterator_property_map<std::vector<bool>::iterator, IndexMap> m_has_parent_map;

      std::vector<long> m_time_vec;
      iterator_property_map<std::vector<long>::iterator, IndexMap> m_time_map;
      tEdgeVal m_flow;
      long m_time;
      vertex_descriptor m_last_grow_vertex;
      out_edge_iterator m_last_grow_edge_it;
      out_edge_iterator m_last_grow_edge_end;
};

}

template<class Graph,
         class CapacityEdgeMap,
         class ResidualCapacityEdgeMap,
         class ReverseEdgeMap, class PredecessorMap,
         class ColorMap,
         class DistanceMap,
         class IndexMap>
typename property_traits<CapacityEdgeMap>::value_type
boykov_kolmogorov_max_flow(Graph& g,
                           CapacityEdgeMap cap,
                           ResidualCapacityEdgeMap res_cap,
                           ReverseEdgeMap rev_map,
                           PredecessorMap pre_map,
                           ColorMap color,
                           DistanceMap dist,
                           IndexMap idx,
                           typename graph_traits<Graph>::vertex_descriptor src,
                           typename graph_traits<Graph>::vertex_descriptor sink)
{
  typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
  typedef typename graph_traits<Graph>::edge_descriptor edge_descriptor;

  BOOST_CONCEPT_ASSERT(( VertexListGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT(( EdgeListGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT(( IncidenceGraphConcept<Graph> ));
  BOOST_CONCEPT_ASSERT(( ReadablePropertyMapConcept<CapacityEdgeMap, edge_descriptor> ));
  BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<ResidualCapacityEdgeMap, edge_descriptor> ));
  BOOST_CONCEPT_ASSERT(( ReadablePropertyMapConcept<ReverseEdgeMap, edge_descriptor> ));
  BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<PredecessorMap, vertex_descriptor> ));
  BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<ColorMap, vertex_descriptor> ));
  BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<DistanceMap, vertex_descriptor> ));
  BOOST_CONCEPT_ASSERT(( ReadablePropertyMapConcept<IndexMap, vertex_descriptor> ));
  BOOST_ASSERT(num_vertices(g) >= 2 && src != sink);

  detail::bk_max_flow<
    Graph, CapacityEdgeMap, ResidualCapacityEdgeMap, ReverseEdgeMap,
    PredecessorMap, ColorMap, DistanceMap, IndexMap
  > algo(g, cap, res_cap, rev_map, pre_map, color, dist, idx, src, sink);

  return algo.max_flow();
}

template<class Graph,
         class CapacityEdgeMap,
         class ResidualCapacityEdgeMap,
         class ReverseEdgeMap,
         class IndexMap>
typename property_traits<CapacityEdgeMap>::value_type
boykov_kolmogorov_max_flow(Graph& g,
                           CapacityEdgeMap cap,
                           ResidualCapacityEdgeMap res_cap,
                           ReverseEdgeMap rev,
                           IndexMap idx,
                           typename graph_traits<Graph>::vertex_descriptor src,
                           typename graph_traits<Graph>::vertex_descriptor sink)
{
  typename graph_traits<Graph>::vertices_size_type n_verts = num_vertices(g);
  std::vector<typename graph_traits<Graph>::edge_descriptor> predecessor_vec(n_verts);
  std::vector<default_color_type> color_vec(n_verts);
  std::vector<typename graph_traits<Graph>::vertices_size_type> distance_vec(n_verts);
  return
    boykov_kolmogorov_max_flow(
      g, cap, res_cap, rev,
      make_iterator_property_map(predecessor_vec.begin(), idx),
      make_iterator_property_map(color_vec.begin(), idx),
      make_iterator_property_map(distance_vec.begin(), idx),
      idx, src, sink);
}

template<class Graph,
         class CapacityEdgeMap,
         class ResidualCapacityEdgeMap,
         class ReverseEdgeMap,
         class ColorMap,
         class IndexMap>
typename property_traits<CapacityEdgeMap>::value_type
boykov_kolmogorov_max_flow(Graph& g,
                           CapacityEdgeMap cap,
                           ResidualCapacityEdgeMap res_cap,
                           ReverseEdgeMap rev,
                           ColorMap color,
                           IndexMap idx,
                           typename graph_traits<Graph>::vertex_descriptor src,
                           typename graph_traits<Graph>::vertex_descriptor sink)
{
  typename graph_traits<Graph>::vertices_size_type n_verts = num_vertices(g);
  std::vector<typename graph_traits<Graph>::edge_descriptor> predecessor_vec(n_verts);
  std::vector<typename graph_traits<Graph>::vertices_size_type> distance_vec(n_verts);
  return
    boykov_kolmogorov_max_flow(
      g, cap, res_cap, rev,
      make_iterator_property_map(predecessor_vec.begin(), idx),
      color,
      make_iterator_property_map(distance_vec.begin(), idx),
      idx, src, sink);
}

template<class Graph, class P, class T, class R>
typename property_traits<typename property_map<Graph, edge_capacity_t>::const_type>::value_type
boykov_kolmogorov_max_flow(Graph& g,
                           typename graph_traits<Graph>::vertex_descriptor src,
                           typename graph_traits<Graph>::vertex_descriptor sink,
                           const bgl_named_params<P, T, R>& params)
{
  return
  boykov_kolmogorov_max_flow(
    g,
    choose_const_pmap(get_param(params, edge_capacity), g, edge_capacity),
    choose_pmap(get_param(params, edge_residual_capacity), g, edge_residual_capacity),
    choose_const_pmap(get_param(params, edge_reverse), g, edge_reverse),
    choose_pmap(get_param(params, vertex_predecessor), g, vertex_predecessor),
    choose_pmap(get_param(params, vertex_color), g, vertex_color),
    choose_pmap(get_param(params, vertex_distance), g, vertex_distance),
    choose_const_pmap(get_param(params, vertex_index), g, vertex_index),
    src, sink);
}

template<class Graph>
typename property_traits<typename property_map<Graph, edge_capacity_t>::const_type>::value_type
boykov_kolmogorov_max_flow(Graph& g,
                           typename graph_traits<Graph>::vertex_descriptor src,
                           typename graph_traits<Graph>::vertex_descriptor sink)
{
  bgl_named_params<int, buffer_param_t> params(0);
  return boykov_kolmogorov_max_flow(g, src, sink, params);
}

}

#endif

