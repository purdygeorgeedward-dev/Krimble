/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTOOLPROXYPRIVATE_P
#define KOTOOLPROXYPRIVATE_P

#include <QTimer>
#include <QElapsedTimer>
#include <QPointF>
#include <QEvent>
#include <KoPointerEvent.h>
#include <boost/optional.hpp>

class KoPointerEvent;
class KoToolBase;
class KoCanvasController;
class KoToolProxy;

class KoToolProxyPrivate
{
public:
    explicit KoToolProxyPrivate(KoToolProxy *p);

    void timeout();

    void checkAutoScroll(const KoPointerEvent &event);

    void selectionChanged(bool newSelection);

    bool isActiveLayerEditable();

    void setCanvasController(KoCanvasController *controller);

    KoToolBase *activeTool {0};
    bool hasSelection {false};
    QTimer scrollTimer;
    QPointF widgetScrollPointDoc;
    KoCanvasController *controller {0};
    KoToolProxy *parent {0};

    QPoint mouseDownPoint;

    bool mouseLeaveWorkaround {false};

    bool isToolPressed {false};

    int multiClickCount {0};
    QPointF multiClickGlobalPoint;
    QElapsedTimer multiClickTimeStamp;
    QEvent::Type multiClickSource;

    QVector<QKeySequence> toolPriorityShortcuts;

    boost::optional<KoPointerEventWrapper> lastPointerEvent;
};

#endif
