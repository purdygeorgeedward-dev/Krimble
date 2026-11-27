/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TAGFILTERPROXYMODELQMLWRAPPER_H
#define TAGFILTERPROXYMODELQMLWRAPPER_H

#include <QObject>
#include <QQmlEngine>
#include <QAbstractItemModel>
#include <KisTagFilterResourceProxyModel.h>

class TagFilterProxyModelQmlWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *model READ model NOTIFY modelChanged)
    Q_PROPERTY(QAbstractItemModel *tagModel READ tagModel NOTIFY tagModelChanged)

    Q_PROPERTY(QString resourceType READ resourceType WRITE setResourceType NOTIFY resourceTypeChanged)
    Q_PROPERTY(QString resourceTypeName READ resourceTypeName NOTIFY resourceTypeChanged)
    Q_PROPERTY(bool importEnabled READ importEnabled NOTIFY resourceTypeChanged)

    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(int currentTag READ currentTag WRITE tagActivated NOTIFY activeTagChanged)
    Q_PROPERTY(bool searchInTag READ searchInTag WRITE setSearchInTag NOTIFY searchInTagChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentResourceChanged)

    Q_PROPERTY(QString resourceName READ resourceName WRITE setResourceToName NOTIFY currentResourceChanged)
    Q_PROPERTY(QString resourceFilename READ resourceFilename WRITE setResourceToFileName NOTIFY currentResourceChanged)
    Q_PROPERTY(KoResourceSP currentResource READ currentResource NOTIFY currentResourceChanged)

    QML_NAMED_ELEMENT(TagFilterProxyModelQmlWrapper)
public:
    TagFilterProxyModelQmlWrapper(QObject *parent = nullptr);
    ~TagFilterProxyModelQmlWrapper();

    QAbstractItemModel *model() const;

    QAbstractItemModel *tagModel() const;

    QString resourceType() const;

    void setResourceType(const QString &type);

    QString resourceTypeName() const;

    bool importEnabled() const;

    void tagActivated(const int &row);
    int currentTag() const;

    QString searchText() const;
    void setSearchText(const QString &text);

    void setSearchInTag(const bool &newSearchInTag);

    bool searchInTag();

    Q_INVOKABLE void addNewTag(const QString &newTagName, const int &resourceIndex = -1);

    Q_INVOKABLE void tagResource(const int &tagIndex, const int &resourceIndex);

    Q_INVOKABLE void untagResource(const int &tagIndex, const int &resourceIndex);

    Q_INVOKABLE QVariantList taggedResourceModel (const int &resourceIndex) const;

    Q_INVOKABLE bool showResourceTagged(const int &tagIndex, const int &resourceIndex) const;

    int currentIndex() const;

    void setCurrentIndex(const int &index);

    void setResourceToFileName(const QString &filename);

    QString resourceFilename();

    QString resourceName() const;
    void setResourceToName(const QString &name);

    KoResourceSP currentResource() const;

    Q_INVOKABLE QVariant testFileName(const QString &name);

    Q_INVOKABLE QVariant testName(const QString &name);

    Q_INVOKABLE void importResource();

    Q_INVOKABLE void removeResource();

Q_SIGNALS:
    void modelChanged();
    void searchTextChanged();
    void activeTagChanged();
    void searchInTagChanged();
    void modelSortUpdated();
    void currentResourceChanged();
    void tagModelChanged();
    void resourceTypeChanged();

private Q_SLOTS:
    void setSearchTextOnModel();
private:

    struct Private;
    const QScopedPointer<Private> d;
};

class FontFamilyTagFilterModel: public KisTagFilterResourceProxyModel {
public:
    FontFamilyTagFilterModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;

    bool additionalResourceNameChecks(const QModelIndex &index, const KisResourceSearchBoxFilter *filter) const override;
};

#endif
