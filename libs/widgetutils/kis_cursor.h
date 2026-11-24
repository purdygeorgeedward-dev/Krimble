/*
 *  kis_cursor.h - part of KImageShop
 *
 *  SPDX-FileCopyrightText: 1999 Matthias Elter <elter@kde.org>
 *  SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __kis_cursor_h__
#define __kis_cursor_h__

#include <kritawidgetutils_export.h>

#include <QCursor>

class KRITAWIDGETUTILS_EXPORT KisCursor
{

public:

    KisCursor();

    static QCursor arrowCursor();
    static QCursor upArrowCursor();
    static QCursor crossCursor();
    static QCursor roundCursor();
    static QCursor pixelBlackCursor();
    static QCursor pixelWhiteCursor();
    static QCursor waitCursor();
    static QCursor ibeamCursor();
    static QCursor sizeVerCursor();
    static QCursor sizeHorCursor();
    static QCursor sizeBDiagCursor();
    static QCursor sizeFDiagCursor();
    static QCursor sizeAllCursor();
    static QCursor blankCursor();
    static QCursor splitVCursor();
    static QCursor splitHCursor();
    static QCursor pointingHandCursor();

    static QCursor zoomSmoothCursor();
    static QCursor zoomDiscreteCursor();
    static QCursor rotateCanvasSmoothCursor();
    static QCursor rotateCanvasDiscreteCursor();
    static QCursor samplerImageForegroundCursor();
    static QCursor samplerImageBackgroundCursor();
    static QCursor samplerLayerForegroundCursor();
    static QCursor samplerLayerBackgroundCursor();
    static QCursor changeExposureCursor();
    static QCursor changeGammaCursor();
    static QCursor triangleLeftHandedCursor();
    static QCursor triangleRightHandedCursor();
    static QCursor eraserCursor();

    static QCursor moveCursor();
    static QCursor moveSelectionCursor();
    static QCursor samplerCursor();
    static QCursor pickLayerCursor();
    static QCursor handCursor();
    static QCursor openHandCursor();
    static QCursor closedHandCursor();
    static QCursor rotateCursor();

    static QCursor meshCursorFree();
    static QCursor meshCursorLocked();

    static QCursor load(const QString & cursorName, int hotspotX = -1, int hotspotY = -1);

    static QCursor loadWithSize(const QString & cursorName, int width, int height, int hotspotX = -1, int hotspotY = -1);

private:

};
#endif
