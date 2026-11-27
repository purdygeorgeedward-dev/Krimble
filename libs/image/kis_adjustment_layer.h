/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */
#ifndef KIS_ADJUSTMENT_LAYER_H_
#define KIS_ADJUSTMENT_LAYER_H_

#include <QObject>
#include <kritaimage_export.h>
#include "kis_selection_based_layer.h"

class KisFilterConfiguration;

class KRITAIMAGE_EXPORT KisAdjustmentLayer : public KisSelectionBasedLayer
{
    Q_OBJECT

public:
    KisAdjustmentLayer(KisImageWSP image, const QString &name, KisFilterConfigurationSP  kfc, KisSelectionSP selection);
    KisAdjustmentLayer(const KisAdjustmentLayer& rhs);
    ~KisAdjustmentLayer() override;

    bool accept(KisNodeVisitor &) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    KisNodeSP clone() const override {
        return KisNodeSP(new KisAdjustmentLayer(*this));
    }

    QIcon icon() const override;

    KisBaseNode::PropertyList sectionModelProperties() const override;

public:

    void setFilter(KisFilterConfigurationSP filterConfig, bool checkCompareConfig = true) override;

    void setChannelFlags(const QBitArray & channelFlags) override;

protected:
    QRect incomingChangeRect(const QRect &rect) const override;
    QRect needRect(const QRect &rect, PositionToFilthy pos = N_FILTHY) const override;

public Q_SLOTS:
    KisLayer* layer() {
        return this;
    }
};

#endif

