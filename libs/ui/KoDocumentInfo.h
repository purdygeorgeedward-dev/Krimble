/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999, 2000 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2004 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KO_DOCUMENT_INFO_H
#define KO_DOCUMENT_INFO_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>

#include "kritaui_export.h"
#include <QDomDocument>

class QDomDocument;
class QDomElement;
class KoStore;

class KRITAUI_EXPORT KoDocumentInfo : public QObject
{
    Q_OBJECT

public:
    explicit KoDocumentInfo(QObject *parent = 0);
    explicit KoDocumentInfo(const KoDocumentInfo &rhs, QObject *parent = 0);

    ~KoDocumentInfo() override;
    bool load(const QDomDocument& doc);

    QDomDocument save(QDomDocument &doc);

    void setAuthorInfo(const QString& info, const QString& data);

    QString authorInfo(const QString& info) const;

    QStringList authorContactInfo() const;

    void setAboutInfo(const QString& info, const QString& data);

    QString aboutInfo(const QString& info) const;

    QString originalGenerator() const;

    void setOriginalGenerator(const QString& generator);

    void resetMetaData();

    void updateParameters();

private:
    void updateParametersAndBumpNumCycles();

    void setActiveAuthorInfo(const QString& info, const QString& data);

    bool loadAboutInfo(const QDomElement& e);

    QDomElement saveAboutInfo(QDomDocument& doc);

    bool loadAuthorInfo(const QDomElement& e);

    QDomElement saveAuthorInfo(QDomDocument& doc);

    QStringList m_aboutTags;
    QStringList m_authorTags;
    QStringList m_contactTags;
    QMap <QString, QString> m_contact;
    QMap<QString, QString> m_authorInfo;
    QMap<QString, QString> m_authorInfoOverride;
    QMap<QString, QString> m_aboutInfo;
    QString m_generator;

Q_SIGNALS:
    void infoUpdated(const QString &info, const QString &data);
};

#endif
