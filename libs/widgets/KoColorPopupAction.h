/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCOLORPOPUPACTION_H
#define KOCOLORPOPUPACTION_H

#include <QAction>

#include "kritawidgets_export.h"

class KoColor;

class KRITAWIDGETS_EXPORT KoColorPopupAction : public QAction
{
    Q_OBJECT

public:
    explicit KoColorPopupAction(QObject *parent = 0);

    ~KoColorPopupAction() override;

public Q_SLOTS:
    void setCurrentColor( const QColor &color );

    void setCurrentColor( const KoColor &color );

    QColor currentColor() const;

    KoColor currentKoColor() const;

    void updateIcon();

Q_SIGNALS:
    void colorChanged(const KoColor &color);

private Q_SLOTS:
    void emitColorChanged();
    void colorWasSelected(const KoColor &color, bool final);
    void colorWasEdited( const QColor &color );
    void opacityWasChanged( int opacity );

private:
    class KoColorPopupActionPrivate;
    KoColorPopupActionPrivate * const d;
};

#endif

