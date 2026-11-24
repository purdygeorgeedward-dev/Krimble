/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KOLINESTYLESELECTOR_H_
#define _KOLINESTYLESELECTOR_H_

#include "kritawidgets_export.h"
#include <QComboBox>

class KRITAWIDGETS_EXPORT KoLineStyleSelector : public QComboBox
{
    Q_OBJECT
public:
    explicit KoLineStyleSelector(QWidget *parent = 0);
    ~KoLineStyleSelector() override;

    bool addCustomStyle(const QVector<qreal> &style);

    void setLineStyle(Qt::PenStyle style, const QVector<qreal> &dashes = QVector<qreal>());

    Qt::PenStyle lineStyle() const;
    QVector<qreal> lineDashes() const;

protected:
    void paintEvent(QPaintEvent *pe) override;

private:
    class Private;
    Private * const d;
};

#endif
