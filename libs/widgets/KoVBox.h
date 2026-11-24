/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KoVBOX_H
#define KoVBOX_H

#include <kritawidgets_export.h>

#include <QFrame>

class QChildEvent;

class KRITAWIDGETS_EXPORT KoVBox : public QFrame
{
    Q_OBJECT

public:
    explicit KoVBox(QWidget *parent = 0);

    ~KoVBox() override;

    void setMargin(int margin);

    void setSpacing(int space);

    void setStretchFactor(QWidget *widget, int stretch);

    QSize sizeHint() const override;

    QSize minimumSizeHint() const override;

protected:

    void childEvent(QChildEvent *ev) override;

private:
    class Private;
    friend class Private;
    Private *const d;

    Q_DISABLE_COPY(KoVBox)
};

#endif
