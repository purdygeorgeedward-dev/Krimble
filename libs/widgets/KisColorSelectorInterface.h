/*
 * SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISCOLORSELECTORINTERFACE_H
#define KISCOLORSELECTORINTERFACE_H

#include "kritawidgets_export.h"

#include <QWidget>
#include <KoColor.h>

class KoColorDisplayRendererInterface;
class KoColorSpace;

class KRITAWIDGETS_EXPORT KisColorSelectorInterface : public QWidget {
    Q_OBJECT
public:
    KisColorSelectorInterface(QWidget *parent = 0)
        : QWidget(parent)
    {
        setContextMenuPolicy(Qt::PreventContextMenu);
    }
    ~KisColorSelectorInterface() override {}
    virtual void setConfig(bool forceCircular, bool forceSelfUpdate)
    {
        Q_UNUSED(forceCircular);
        Q_UNUSED(forceSelfUpdate);
    }
    virtual void setDisplayRenderer (const KoColorDisplayRendererInterface *displayRenderer)
    {
        Q_UNUSED(displayRenderer);
    }

    virtual KoColor getCurrentColor() const = 0;

Q_SIGNALS:
    void sigNewColor(const KoColor &c);

public Q_SLOTS:
    virtual void slotSetColor(const KoColor &c) = 0;
    virtual void slotSetColorSpace(const KoColorSpace *cs);
};

#endif
