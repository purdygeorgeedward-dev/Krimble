/*
 * colorrange.h -- Part of Krita
 *
 * SPDX-FileCopyrightText: 2004 Boudewijn Rempt (boud@valdyas.org)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "colorrange.h"

#include <klocalizedstring.h>
#include <kis_debug.h>
#include <kpluginfactory.h>

#include "kis_image.h"
#include "kis_layer.h"
#include "kis_paint_device.h"
#include "kis_global.h"
#include "kis_types.h"
#include "KisViewManager.h"
#include "kis_selection.h"
#include "kis_selection_manager.h"
#include "kis_selection_tool_helper.h"
#include "kis_canvas2.h"
#include "kis_iterator_ng.h"
#include "kis_action.h"

#include "dlg_colorrange.h"
#include <KoColorSpace.h>
#include <KisSignalMapper.h>

// Krimble: Select > Similar
#include <KoColorSpaceRegistry.h>
#include <KSharedConfig>
#include <KConfigGroup>
#include <QVector>
#include "kis_pixel_selection.h"
#include "kis_sequential_iterator.h"
#include "KisImageBarrierLock.h"

K_PLUGIN_FACTORY_WITH_JSON(ColorRangeFactory, "kritacolorrange.json", registerPlugin<ColorRange>();)


ColorRange::ColorRange(QObject *parent, const QVariantList &)
        : KisActionPlugin(parent)
{
    KisAction* action = createAction("colorrange");
    connect(action, SIGNAL(triggered()), this, SLOT(slotActivated()));


    KisSignalMapper *mapper = new KisSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), SLOT(selectOpaque(int)));

    action  = createAction("selectopaque");
    mapper->setMapping(action, int(SELECTION_REPLACE));
    connect(action, SIGNAL(triggered(bool)), mapper, SLOT(map()));

    action  = createAction("selectopaque_add");
    mapper->setMapping(action, int(SELECTION_ADD));
    connect(action, SIGNAL(triggered(bool)), mapper, SLOT(map()));

    action  = createAction("selectopaque_subtract");
    mapper->setMapping(action, int(SELECTION_SUBTRACT));
    connect(action, SIGNAL(triggered(bool)), mapper, SLOT(map()));

    action  = createAction("selectopaque_intersect");
    mapper->setMapping(action, int(SELECTION_INTERSECT));
    connect(action, SIGNAL(triggered(bool)), mapper, SLOT(map()));

    // Krimble: Select > Similar
    action = createAction("similar");
    connect(action, SIGNAL(triggered()), this, SLOT(selectSimilar()));
}

ColorRange::~ColorRange()
{
}

void ColorRange::slotActivated()
{
    DlgColorRange *dlgColorRange = new DlgColorRange(viewManager(), viewManager()->mainWindowAsQWidget());
    Q_CHECK_PTR(dlgColorRange);

    dlgColorRange->exec();
}

void ColorRange::selectOpaque(int id)
{
    KisNodeSP node = viewManager()->activeNode();
    if (!node) return;

    viewManager()->selectionManager()->
        selectOpaqueOnNode(node, SelectionAction(id));
}

/**
 * Krimble: Select > Similar, as in Photoshop.
 * Adds to the selection every pixel of the active layer whose colour is
 * within the Magic Wand's threshold of a colour inside the current selection.
 * Colours are kept in a 64 x 64 x 64 grid (4 levels per cell), grown by the
 * threshold per channel, then every pixel of the picture is looked up.
 */
void ColorRange::selectSimilar()
{
    KisViewManager *view = viewManager();
    if (!view) return;
    KisImageSP image = view->image();
    KisNodeSP node = view->activeNode();
    KisSelectionSP selection = view->selection();
    KisCanvas2 *canvas = view->canvasBase();
    if (!image || !node || !selection || !canvas) return;

    if (!view->blockUntilOperationsFinished(image)) return;

    // Same tolerance as the Magic Wand (contiguous selection tool)
    KConfigGroup grp = KSharedConfig::openConfig()->group("KisToolSelectContiguous");
    int threshold = grp.hasKey("threshold") ? grp.readEntry("threshold", 8)
                                            : grp.readEntry("fuzziness", 8);
    threshold = qBound(1, threshold, 255);

    const int N = 64;               // cells per channel
    const int radius = (threshold + 3) / 4;

    KisPixelSelectionSP result = new KisPixelSelection();

    {
        KisImageBarrierLock lock(image);

        KisPaintDeviceSP device = node->projection();
        if (!device) device = node->paintDevice();
        if (!device) device = node->original();
        if (!device) return;

        const KoColorSpace *rgb8 = KoColorSpaceRegistry::instance()->rgb8();
        KisPaintDeviceSP rgb = device;
        if (!(*device->colorSpace() == *rgb8)) {
            rgb = new KisPaintDevice(*device);
            rgb->convertTo(rgb8);
        }

        KisPixelSelectionSP sel = selection->projection();
        const QRect selRect = sel->selectedExactRect();
        if (selRect.isEmpty()) return;

        // 1. Colours inside the selection
        QVector<quint8> grid(N * N * N, 0);
        bool sawTransparent = false;
        {
            KisSequentialConstIterator it(rgb, selRect);
            KisSequentialConstIterator sit(sel, selRect);
            while (it.nextPixel() && sit.nextPixel()) {
                if (*sit.oldRawData() < 128) continue;
                const quint8 *p = it.oldRawData();   // B, G, R, A
                if (p[3] < 128) { sawTransparent = true; continue; }
                grid[((p[2] >> 2) * N + (p[1] >> 2)) * N + (p[0] >> 2)] = 1;
            }
        }

        // 2. Grow by the threshold along each channel
        QVector<quint8> tmp(N * N * N);
        for (int axis = 0; axis < 3; ++axis) {
            const int stride = axis == 0 ? 1 : (axis == 1 ? N : N * N);
            for (int i = 0; i < N * N * N; ++i) {
                const int c = (i / stride) % N;
                quint8 v = 0;
                for (int d = qMax(0, c - radius); d <= qMin(N - 1, c + radius) && !v; ++d) {
                    v = grid[i + (d - c) * stride];
                }
                tmp[i] = v;
            }
            grid.swap(tmp);
        }

        // 3. Every pixel of the picture
        const QRect rc = image->bounds();
        KisSequentialConstIterator it(rgb, rc);
        KisSequentialIterator oit(result, rc);
        while (it.nextPixel() && oit.nextPixel()) {
            const quint8 *p = it.oldRawData();
            bool on;
            if (p[3] < 128) on = sawTransparent;
            else on = grid[((p[2] >> 2) * N + (p[1] >> 2)) * N + (p[0] >> 2)];
            if (on) *oit.rawData() = MAX_SELECTED;
        }
    }

    // Added to the current selection, so its own edges stay as they were
    KisSelectionToolHelper helper(canvas, kundo2_i18n("Select Similar"));
    result->invalidateOutlineCache();
    helper.selectPixelSelection(result, SELECTION_ADD);
}

#include "colorrange.moc"

