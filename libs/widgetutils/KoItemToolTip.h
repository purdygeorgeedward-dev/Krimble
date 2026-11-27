/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_ITEM_TOOLTIP_H
#define KO_ITEM_TOOLTIP_H

#include <QFrame>
#include "kritawidgetutils_export.h"

class QStyleOptionViewItem;
class QModelIndex;
class QTextDocument;

class KRITAWIDGETUTILS_EXPORT KoItemToolTip : public QFrame
{
    Q_OBJECT
public:
    KoItemToolTip();
    ~KoItemToolTip() override;
    void showTip(QWidget *widget, const QPoint &pos, const QStyleOptionViewItem &option, const QModelIndex &index);

protected:

    virtual QTextDocument *createDocument(const QModelIndex &index) = 0;

private:
    class Private;
    Private* const d;

    void updatePosition(QWidget *widget, const QPoint &pos, const QStyleOptionViewItem &option);

public:
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *e) override;
    void timerEvent(QTimerEvent *e) override;
    bool eventFilter(QObject *object, QEvent *event) override;
};

#endif
