/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCOLORPOPUPBUTTON_H_
#define KOCOLORPOPUPBUTTON_H_

#include <QToolButton>

#include "kritawidgets_export.h"

class KRITAWIDGETS_EXPORT KoColorPopupButton: public QToolButton
{
    Q_OBJECT

public:
    explicit KoColorPopupButton(QWidget *parent=0);

    ~KoColorPopupButton() override;

    QSize sizeHint() const override;

Q_SIGNALS:
    void iconSizeChanged();

protected:
    void resizeEvent(QResizeEvent *) override;
};

#endif
