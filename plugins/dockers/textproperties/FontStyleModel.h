/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef FONTSTYLEMODEL_H
#define FONTSTYLEMODEL_H

#include <QAbstractItemModel>
#include <KoSvgText.h>
#include <QLocale>

class KoSvgTextPropertiesModel;
class FontStyleModel: public QAbstractItemModel
{
    Q_OBJECT
    enum Roles {
        Weight = Qt::UserRole + 1,
        Width,
        StyleMode,
        Slant,
        AxisValues
    };

public:
    FontStyleModel(QObject *parent = nullptr);
    ~FontStyleModel();

    void setStylesInfo(QList<KoSvgText::FontFamilyStyleInfo> styles);

    Q_INVOKABLE qreal weightValue(int row);
    Q_INVOKABLE qreal widthValue(int row);
    Q_INVOKABLE int styleModeValue(int row);
    Q_INVOKABLE qreal slantValue(int row);
    Q_INVOKABLE QVariantHash axesValues(int row);

    Q_INVOKABLE int rowForStyle(const qreal &weight, const qreal &width, const int &styleMode, const qreal &styleValue);

    Q_INVOKABLE void setFromTextPropertiesModel(KoSvgTextPropertiesModel *textPropertiesModel);

public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
private:
    struct Private;
    const QScopedPointer<Private> d;
};

#endif
