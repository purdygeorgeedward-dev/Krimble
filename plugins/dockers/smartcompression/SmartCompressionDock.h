/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  Krimble 2026-11-13 (task 55): George (Chatty's ideas, "The first one is pretty."): the SmartCompression panel follows the first mockup.
 *
 *  The SmartCompression panel (the look is SmartCompressionPanel.h). Whole Picture and Regions are the two boxes of XAT's window; in Regions mode the film (Krita's selection overlay) covers
 *  the protected area, the region tools are enabled, and the starting selection is the Smart Selection. The selection is the area to COMPRESS; what it does not cover is protected. The preview
 *  makes the JPEG in memory with the panel's settings and shows the size and the picture. Save JPEG... hands the settings to the JPEG save dialog and opens it.
 */

#ifndef _SMARTCOMPRESSION_DOCK_H_
#define _SMARTCOMPRESSION_DOCK_H_

#include <QDockWidget>
#include <QPointer>
#include <QTimer>

#include <KoCanvasObserverBase.h>

class KoCanvasBase;
class KisCanvas2;
class SmartCompressionPanel;

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
    void onRegionsToggled(bool regions);
    void onProtectionChanged(int protect);
    void onSensitivityChanged(int value);
    void onSelectionTool(const QString &toolId);
    void onSelectionAction(const QString &actionName);
    void schedulePreview();
    void updatePreview();
    void onSave();

private:
    void triggerAction(const QString &actionName);
    void showFilm(bool on);
    bool hasSelection() const;

    QPointer<KisCanvas2> m_canvas;
    SmartCompressionPanel *m_panel {nullptr};
    QTimer m_previewTimer;
};

#endif
