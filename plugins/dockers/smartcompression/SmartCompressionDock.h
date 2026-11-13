/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  Krimble 2026-11-13 (task 52): George: "When the user edits the selection for SmartCompression they should start with the smart selection." and "This is how XAT handled it." (the Work with Regions mode, the AutoCompress wand, Remove All).
 *
 *  The SmartCompression panel, after XAT's Compress Image window: two boxes, Whole picture and Regions ("Work with Whole Image", "Work with Regions"); Smart (the wand: AutoCompress:
 *  it makes the Smart Selection, the starting selection) and Clear (the red cross: Remove All); in Regions mode the region tools (rectangle, freehand, invert, all) and the film that
 *  covers the protected area. The selection is the area to COMPRESS; what it does not cover (what the film covers) is protected. Save JPEG opens the export, whose SmartCompression tab
 *  has the quality and the level.
 */

#ifndef _SMARTCOMPRESSION_DOCK_H_
#define _SMARTCOMPRESSION_DOCK_H_

#include <QDockWidget>
#include <QPointer>

#include <KoCanvasObserverBase.h>

class KoCanvasBase;
class KisCanvas2;
class QSlider;
class QLabel;
class QPushButton;
class QWidget;

class SmartCompressionDock : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    SmartCompressionDock();
    QString observerName() override { return "SmartCompressionDock"; }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private Q_SLOTS:
    void makeSmartSelection();
    void clearSelection();
    void setRegionsMode(bool regions);
    void useTool(const QString &toolId);
    void triggerAction(const QString &actionName);
    void exportJpeg();

private:
    void showFilm(bool on);

    QPointer<KisCanvas2> m_canvas;
    QPushButton *m_wholeButton {nullptr};
    QPushButton *m_regionsButton {nullptr};
    QWidget *m_regionTools {nullptr};
    QSlider *m_sensitivity {nullptr};
    QLabel *m_sensitivityLabel {nullptr};
};

#endif
