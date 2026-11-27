/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KIS_VIEW_H
#define KIS_VIEW_H

#include <QWidget>

#include <KoColorSpace.h>
#include <KoColorProfile.h>

#include <kis_types.h>
#include "kritaui_export.h"
#include <KisNodeAdditionFlags.h>

#include "widgets/kis_floating_message.h"

#include <kis_node.h>
class KisDocument;
class KisMainWindow;
class KisCanvasController;
class KisZoomManager;
class KisCanvas2;
class KisViewManager;
class KisDocument;
class KisCanvasResourceProvider;
class KisCoordinatesConverter;
class KisInputManager;

class KoCanvasResourceProvider;

class QAction;
class KisKActionCollection;
class KConfigGroup;

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QPrintDialog;
class QCloseEvent;
class QStatusBar;
class QMdiSubWindow;

class KRITAUI_EXPORT KisView : public QWidget
{
    Q_OBJECT

public:
    KisView(KisDocument *document, KisViewManager *viewManager, QWidget *parent = 0);
    ~KisView() override;

    void setViewManager(KisViewManager *view);
    KisViewManager *viewManager() const;

public:

    KisDocument *document() const;

    KisView *replaceBy(KisDocument *document);

    KisMainWindow *mainWindow() const;

    void setSubWindow(QMdiSubWindow *subWindow);

    QStatusBar *statusBar() const;

    void addStatusBarItem(QWidget * widget, int stretch = 0, bool permanent = false);

    void removeStatusBarItem(QWidget * widget);

    QList<QAction*> createChangeUnitActions(bool addPixelUnit = false);

    void closeView();

    QScreen *currentScreen() const;

public:

    KisZoomManager *zoomManager() const;

    KisCanvasController *canvasController() const;
    KisCanvasResourceProvider *resourceProvider() const;

    KisInputManager* globalInputManager() const;

    KisCanvas2 *canvasBase() const;

    KisImageWSP image() const;

    KisCoordinatesConverter *viewConverter() const;

    void setCurrentNode(KisNodeSP node);
    KisNodeSP currentNode() const;
    KisLayerSP currentLayer() const;
    KisMaskSP currentMask() const;
    bool softProofing();
    bool gamutCheck();

    KisSelectionSP selection();

    void notifyCurrentStateChanged(bool isCurrent);
    bool isCurrent() const;

    void setShowFloatingMessage(bool show);
    void showFloatingMessage(const QString &message, const QIcon& icon, int timeout = 4500,
                             KisFloatingMessage::Priority priority = KisFloatingMessage::Medium,
                             int alignment = Qt::AlignCenter | Qt::TextWordWrap);

    bool canvasIsMirrored() const;

    void syncLastActiveNodeToDocument();

    void saveViewState(KisPropertiesConfiguration &config) const;
    void restoreViewState(const KisPropertiesConfiguration &config);

public Q_SLOTS:

    void slotSavingStatusMessage(const QString &text, int timeout, bool isAutoSaving = false);

    void slotClearStatusText();
    void slotSoftProofing(bool softProofing);
    void slotGamutCheck(bool gamutCheck);

    bool queryClose();

    void slotMigratedToScreen(QScreen *screen);
    void slotScreenOrResolutionChanged();

    void slotThemeChanged(QPalette pal);

    void slotUpdateDocumentTitle();

private Q_SLOTS:
    void slotContinueAddNode(KisNodeSP newActiveNode, KisNodeAdditionFlags flags);

    void slotImageNodeRemoved(KisNodeSP node);
    void slotContinueRemoveNode(KisNodeSP newActiveNode);

Q_SIGNALS:
    void sigSizeChanged(const QPointF &oldStillPoint, const QPointF &newStillPoint);
    void sigProfileChanged(const KoColorProfile *  profile);
    void sigColorSpaceChanged(const KoColorSpace*  cs);

protected:

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    QString newObjectName();

public Q_SLOTS:
    void slotLoadingFinished();
    void slotImageResolutionChanged();
    void slotImageSizeChanged(const QPointF &oldStillPoint, const QPointF &newStillPoint);

private:
    bool shouldAcceptDrag(const QDropEvent *event) const;

    class Private;
    Private * const d;

    static bool s_firstView;
};

#endif
