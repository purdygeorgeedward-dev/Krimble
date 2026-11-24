/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCANVASOBSERVERBASE_H
#define KOCANVASOBSERVERBASE_H

class KoCanvasBase;
class KoCanvasObserverBasePrivate;

#include "kritaflake_export.h"
#include <QString>
#include <QMainWindow>

class KRITAFLAKE_EXPORT KoCanvasObserverBase
{
public:
    KoCanvasObserverBase();
    virtual ~KoCanvasObserverBase();

    virtual QString observerName() { return ""; }

    void setObservedCanvas(KoCanvasBase *canvas);

    void unsetObservedCanvas();

    KoCanvasBase* observedCanvas() const;

protected:
    virtual void setCanvas(KoCanvasBase *canvas) = 0;

    virtual void unsetCanvas() = 0;

private:
    KoCanvasObserverBasePrivate * const d;
};

#endif
