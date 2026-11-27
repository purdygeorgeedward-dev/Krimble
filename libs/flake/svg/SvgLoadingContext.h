/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGLOADINGCONTEXT_H
#define SVGLOADINGCONTEXT_H

#include <functional>
#include <QStringList>
#include <QDomDocument>
#include <QScopedPointer>
#include <KoSvgTextProperties.h>

#include "kritaflake_export.h"

class SvgGraphicsContext;
class SvgStyleParser;
class KoDocumentResourceManager;
class KoShape;
class KoColorProfile;

class KRITAFLAKE_EXPORT SvgLoadingContext
{
public:
    explicit SvgLoadingContext(KoDocumentResourceManager *documentResourceManager);
    ~SvgLoadingContext();

    SvgGraphicsContext *currentGC() const;

    SvgGraphicsContext *pushGraphicsContext(const QDomElement &element = QDomElement(), bool inherit = true);

    void popGraphicsContext();

    void setInitialXmlBaseDir(const QString &baseDir);

    QString xmlBaseDir() const;

    QString absoluteFilePath(const QString &href);

    QString relativeFilePath(const QString &href);

    int nextZIndex();

    void registerShape(const QString &id, KoShape *shape);

    KoShape* shapeById(const QString &id);

    void addDefinition(const QDomElement &element);

    QDomElement definition(const QString &id) const;

    bool hasDefinition(const QString &id) const;

    void addStyleSheet(const QDomElement &styleSheet);

    QStringList matchingCssStyles(const QDomElement &element) const;

    SvgStyleParser &styleParser();

    void parseProfile(const QDomElement &element);

    QHash<QString, const KoColorProfile*> profiles();

    KoSvgTextProperties resolvedProperties(bool onlyFontAndLineHeight = false) const;

    bool isRootContext() const;

    typedef std::function<QByteArray(const QString&)> FileFetcherFunc;
    void setFileFetcher(FileFetcherFunc func);

    QByteArray fetchExternalFile(const QString &url);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif
