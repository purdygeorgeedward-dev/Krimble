/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCOLORSETWIDGET_H_
#define KOCOLORSETWIDGET_H_

#include <QWidgetAction>
#include <QFrame>

#include <KisSwatch.h>
#include <KoColorSet.h>

#include <KoColorDisplayRendererInterface.h>

#include "kritawidgets_export.h"

class KoColor;
class KoColorPatch;

class KRITAWIDGETS_EXPORT KoColorSetWidget : public QFrame
{

    Q_OBJECT

public:

    explicit KoColorSetWidget(QWidget *parent=0);

    ~KoColorSetWidget() override;

    void setColorSet(KoColorSetSP colorSet);

    void setDisplayRenderer(const KoColorDisplayRendererInterface *displayRenderer);

    KoColorSetSP colorSet();

protected:
    void resizeEvent(QResizeEvent *event) override;

Q_SIGNALS:

    void colorChanged(const KoColor &color, bool final);

    void widgetSizeChanged(const QSize &size);

private Q_SLOTS:
    void slotPatchTriggered(KoColorPatch *);
    void slotColorSelectedByPalette(const KoColor &color);
    void slotPaletteChosen(KoColorSetSP );
    void slotNameListSelection(const KoColor &);

private:
    class KoColorSetWidgetPrivate;
    KoColorSetWidgetPrivate * const d;
};

#endif
