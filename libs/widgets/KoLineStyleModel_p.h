/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOLINESTYLEMODEL_H
#define KOLINESTYLEMODEL_H

#include <QAbstractListModel>

#include <QVector>

class KoLineStyleModel : public QAbstractListModel
{
public:
    explicit KoLineStyleModel(QObject *parent = 0);
    ~KoLineStyleModel() override {}
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool addCustomStyle(const QVector<qreal> &style);
    int setLineStyle(Qt::PenStyle style, const QVector<qreal> &dashes);
private:
    QList<QVector<qreal> > m_styles;
    QVector<qreal> m_tempStyle;
    bool m_hasTempStyle;
};

#endif
