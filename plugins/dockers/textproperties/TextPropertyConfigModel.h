/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TEXTPROPERTYCONFIGMODEL_H
#define TEXTPROPERTYCONFIGMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QObject>

class TextPropertyConfigModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(VisibilityState defaultVisibilityState READ defaultVisibilityState WRITE setDefaultVisibilityState NOTIFY defaultVisibilityStateChanged)
    Q_PROPERTY(bool shouldFilter READ shouldFilter NOTIFY shouldFilterChanged)
public:

    enum VisibilityState {
        FollowDefault = 0,
        WhenRelevant = 1,
        WhenSet = 2,
        AlwaysVisible = 3,
        NeverVisible = 4,
    };
    Q_ENUM(VisibilityState)

    enum PropertyType {
        Character,
        Paragraph,
        Mixed
    };
    Q_ENUM(PropertyType)

    enum Roles {
        Visibility = Qt::UserRole + 1,
        SearchTerms,
        Type,
        Name,
    };

    explicit TextPropertyConfigModel(QObject *parent = nullptr);
    ~TextPropertyConfigModel();

    VisibilityState defaultVisibilityState() const;
    void setDefaultVisibilityState(const VisibilityState state);

    bool shouldFilter() const;

    Q_INVOKABLE void loadFromConfiguration();

    Q_INVOKABLE void addProperty(const QString &name,
                     const int propertyType = 0,
                     const QString title = QString(),
                     const QString toolTip = QString(),
                     const QString searchTerms = QString(),
                     const int visibilityState = -1);

    Q_INVOKABLE int visibilityStateForName(const QString &name) const;

    void saveConfiguration();
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void defaultVisibilityStateChanged();
    void shouldFilterChanged();
private:

    struct Private;
    struct TextPropertyData;
    const QScopedPointer<Private> d;
};

class TextPropertyConfigFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool showParagraphProperties READ showParagraphProperties WRITE setShowParagraphProperties NOTIFY showParagraphPropertiesChanged)
    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setProxySourceModel NOTIFY proxySourceModelChanged)
    Q_PROPERTY(QStringList filteredNames READ filteredNames NOTIFY filteredNamesChanged)
public:
    TextPropertyConfigFilterModel(QObject *parent = nullptr);

    bool showParagraphProperties() const;

    void setShowParagraphProperties(const bool show);

    void setProxySourceModel(QAbstractItemModel *model);

    QStringList filteredNames() const;

Q_SIGNALS:
    void showParagraphPropertiesChanged();
    void proxySourceModelChanged();
    void filteredNamesChanged();
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    bool m_showParagraphProperties;
};

#endif
