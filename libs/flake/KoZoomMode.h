/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Johannes Schaub <litb_devel@web.de>
   SPDX-FileCopyrightText: 2011 Arjen Hiemstra <ahiemstra@heimr.nl>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KOZOOMMODE_H_
#define _KOZOOMMODE_H_

#include <QFlags>
#include "kritaflake_export.h"

class QDebug;

class KRITAFLAKE_EXPORT KoZoomMode
{
public:
    enum Mode
    {
        ZOOM_CONSTANT = 0,
        ZOOM_PAGE     = 1,
        ZOOM_WIDTH    = 2,
        ZOOM_HEIGHT   = 16,
    };

    Q_DECLARE_FLAGS(Modes, Mode)

    static QString toString(Mode mode);

    static QVector<qreal> generateStandardZoomLevels(qreal minZoom, qreal maxZoom);

    static qreal findNextZoom(qreal currentZoom, const QVector<qreal> &zoomLevels);

    static qreal findPrevZoom(qreal currentZoom, const QVector<qreal> &zoomLevels);

private:
    static const char * const modes[];
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoZoomMode::Modes)

KRITAFLAKE_EXPORT
QDebug operator<<(QDebug dbg, const KoZoomMode::Mode &mode);

#endif
