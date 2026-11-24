/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef FONTAXESMODEL_H
#define FONTAXESMODEL_H

#include <QAbstractItemModel>
#include <QLocale>
#include <KoSvgText.h>

class KoSvgTextPropertiesModel;

class FontAxesModel: public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap axisValues READ axisValues WRITE setAxisValues NOTIFY axisValuesChanged)
public:
    enum Roles {
        Min = Qt::UserRole + 1,
        Max,
        Hidden
    };

    FontAxesModel(QObject *parent = nullptr);
    ~FontAxesModel();

    void setAxesData(QList<KoSvgText::FontFamilyAxis> axes);

    void setOpticalSizeDisabled(bool disable);

    void setBlockAxesValuesSignal(bool block);
    bool axesValueSignalBlocked() const;

    QVariantMap axisValues() const;

    Q_INVOKABLE void setFromTextPropertiesModel(KoSvgTextPropertiesModel *textPropertiesModel);
public Q_SLOTS:
    void setAxisValues(const QVariantMap &newAxisValues);

public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void axisValuesChanged();

private:
    struct Private;
    const QScopedPointer<Private> d;
};

#endif
