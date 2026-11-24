/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef OPENTYPEFEATUREMODEL_H
#define OPENTYPEFEATUREMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

class KoSvgTextProperties;
struct KoOpenTypeFeatureInfo;
class KoSvgTextPropertiesModel;

class OpenTypeFeatureFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY (bool filterAvailable READ filterAvailable WRITE setFilterAvailable NOTIFY filterAvailableChanged)
    Q_PROPERTY (QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
public:
    OpenTypeFeatureFilterModel(QObject *parent = nullptr);

    Q_INVOKABLE QString firstValidTag() const;
    bool filterAvailable() const;
    void setFilterAvailable(bool newFilterAvailable);

    QString searchText() const;
    void setSearchText(const QString &newSearchText);

Q_SIGNALS:
    void filterAvailableChanged();

    void searchTextChanged();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
private:
    bool m_filterAvailable;
};

class OpenTypeFeatureModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap openTypeFeatures READ openTypeFeatures WRITE setOpenTypeFeatures NOTIFY openTypeFeaturesChanged)
public:
    OpenTypeFeatureModel(QObject *parent = nullptr);

    ~OpenTypeFeatureModel();

    enum Roles {
        Tag = Qt::UserRole + 1,
        Sample,
        Parameters,
        Max
    };
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QHash<int, QByteArray> roleNames() const override;

    void setFromTextProperties(const KoSvgTextProperties &props);

    QVariantMap openTypeFeatures() const;
    void setOpenTypeFeatures(const QVariantMap &newOpenTypeFeatures);

    Q_INVOKABLE void addFeature(const QString &tag);

    Q_INVOKABLE void removeFeature(const QString &tag);

    Q_INVOKABLE QAbstractItemModel *allFeatureModel() const;

    Q_INVOKABLE void setFromTextPropertiesModel(KoSvgTextPropertiesModel *textPropertiesModel);

Q_SIGNALS:
    void openTypeFeaturesChanged();

private:

    struct Private;
    const QScopedPointer<Private> d;
};

class AllOpenTypeFeaturesModel: public QAbstractListModel {
public:
    AllOpenTypeFeaturesModel(QObject *parent = nullptr);

    enum Roles {
        Tag = Qt::UserRole + 1,
        Sample,
        Available
    };
    ~AllOpenTypeFeaturesModel();

    void setAvailableFeatures(const QList<KoOpenTypeFeatureInfo> &features);

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
private:

    struct Private;
    const QScopedPointer<Private> d;
};

#endif
