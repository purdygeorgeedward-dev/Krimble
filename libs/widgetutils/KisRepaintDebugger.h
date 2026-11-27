/*
 * SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_REPAINT_DEBUGGER_H
#define KIS_REPAINT_DEBUGGER_H

#include <QtGlobal>

#include <kritawidgetutils_export.h>

class QPaintDevice;
class QPaintEvent;
class QRect;

class KRITAWIDGETUTILS_EXPORT KisRepaintDebugger
{
public:
    static bool enabled();

    KisRepaintDebugger() = default;
    ~KisRepaintDebugger() = default;

    void paint(QPaintDevice *paintDevice, const QRect &widgetRect);
    void paint(QPaintDevice *paintDevice, const QVector<QRect> &widgetRects);
    void paint(QPaintDevice *paintDevice, const QPaintEvent *event);
    void paintFull(QPaintDevice *paintDevice);

private:
    void paint(QPaintDevice *paintDevice, const QRect *widgetRects, size_t count);

private:
    unsigned int m_colorIndex {0};
};

#endif
