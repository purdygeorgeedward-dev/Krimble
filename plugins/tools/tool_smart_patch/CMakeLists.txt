set(kritatoolSmartPatch_SOURCES
    tool_smartpatch.cpp
    kis_tool_smart_patch.cpp
    kis_tool_smart_patch_options_widget.cpp
    kis_inpaint.cpp
    krimble_inpaint.cpp
    krimble_fill.cpp
    )
# Krimble 2026-10-19: the healing brush uses Krimble's fill engines too (krimble_inpaint.cpp, krimble_fill.cpp), with the method chosen in its Tool Options

ki18n_wrap_ui(kritatoolSmartPatch_SOURCES kis_tool_smart_patch_options_widget.ui)

kis_add_library(kritatoolSmartPatch MODULE ${kritatoolSmartPatch_SOURCES})

generate_export_header(kritatoolSmartPatch BASE_NAME kritatoolSmartPatch)

target_link_libraries(kritatoolSmartPatch kritaui)

install(TARGETS kritatoolSmartPatch  DESTINATION ${KRITA_PLUGIN_INSTALL_DIR})

# Krimble: Content-Aware Fill is a separate plugin module from
# kritatoolSmartPatch above, registered as "Krita/ViewPlugin" instead of
# "Krita/Tool" -- see content_aware_fill_plugin.h for why these can't be
# the same class/plugin. kis_inpaint.cpp (the shared PatchMatch
# implementation) is compiled into both plugin modules; each is an
# independent shared library, so this is a normal, safe duplication.
set(kritaContentAwareFill_SOURCES
    content_aware_fill_plugin.cpp
    kis_inpaint.cpp
    krimble_inpaint.cpp
    krimble_fill.cpp
    )

kis_add_library(kritacontentawarefill MODULE ${kritaContentAwareFill_SOURCES})

target_link_libraries(kritacontentawarefill kritaui)

install(TARGETS kritacontentawarefill  DESTINATION ${KRITA_PLUGIN_INSTALL_DIR})

