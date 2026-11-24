/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005, 2007-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGPARSER_H
#define SVGPARSER_H

#include <QMap>
#include <QSizeF>
#include <QRectF>
#include <QSharedPointer>
#include <QExplicitlySharedDataPointer>

#include "kritaflake_export.h"
#include "SvgGradientHelper.h"
#include "SvgClipPathHelper.h"
#include "SvgLoadingContext.h"
#include "SvgStyleParser.h"
#include "KoClipMask.h"
#include <resources/KoSvgSymbolCollectionResource.h>
#include <KoID.h>

class KoShape;
class KoShapeGroup;
class KoShapeContainer;
class KoDocumentResourceManager;
class KoVectorPatternBackground;
class KoMarker;
class KoPathShape;
class KoSvgTextShape;
class KoSvgTextLoader;
class QDomText;

class KRITAFLAKE_EXPORT SvgParser
{
    struct DeferredUseStore;

public:
    explicit SvgParser(KoDocumentResourceManager *documentResourceManager);
    virtual ~SvgParser();

    static QDomDocument createDocumentFromSvg(QIODevice *device, QString *errorMsg = 0, int *errorLine = 0, int *errorColumn = 0);
    static QDomDocument createDocumentFromSvg(const QByteArray &data, QString *errorMsg = 0, int *errorLine = 0, int *errorColumn = 0);
    static QDomDocument createDocumentFromSvg(const QString &data, QString *errorMsg = 0, int *errorLine = 0, int *errorColumn = 0);
    static QDomDocument createDocumentFromSvg(QXmlStreamReader reader, QString *errorMsg = 0, int *errorLine = 0, int *errorColumn = 0);

    QList<KoShape*> parseSvg(const QDomElement &e, QSizeF * fragmentSize = 0);

    void setXmlBaseDir(const QString &baseDir);

    void setResolution(const QRectF boundsInPixels, qreal pixelsPerInch);
    void setDefaultKraTextVersion(int version);

    void setFillStrokeInheritByDefault(const bool enable);

    void setResolveTextPropertiesForTopLevel(const bool enable);

    QList<KoShape*> shapes() const;

    QVector<KoSvgSymbol*> takeSymbols();

    QString documentTitle() const;
    QString documentDescription() const;

    typedef std::function<QByteArray(const QString&)> FileFetcherFunc;
    void setFileFetcher(FileFetcherFunc func);

    QList<QExplicitlySharedDataPointer<KoMarker>> knownMarkers() const;

    void parseDefsElement(const QDomElement &e);
    KoShape* parseTextElement(const QDomElement &e, KoSvgTextShape *mergeIntoShape = 0);

    QStringList warnings() const;

protected:

    KoShape* parseGroup(const QDomElement &e, const QDomElement &overrideChildrenFrom = QDomElement(), bool createContext = true);

    KoShape* getTextPath(const QDomElement &e, bool hideShapesFromDefs = true);

    void parseTextChildren(const QDomElement &e, KoSvgTextLoader &textLoader, bool hideShapesFromDefs = true);

    QList<KoShape*> parseContainer(const QDomElement &);

    QList<KoShape*> parseSingleElement(const QDomElement &b, DeferredUseStore* deferredUseStore = 0);

    KoShape* parseUse(const QDomElement &, DeferredUseStore* deferredUseStore);

    KoShape* resolveUse(const QDomElement &e, const QString& key);

    SvgGradientHelper *parseGradient(const QDomElement &);

    SvgGradientHelper* parseMeshGradient(const QDomElement&);

    QList<QPair<QString, QColor>> parseMeshPatch(const QDomNode& meshpatch);

    QSharedPointer<KoVectorPatternBackground> parsePattern(const QDomElement &e, const KoShape *__shape);

    bool parseFilter(const QDomElement &, const QDomElement &referencedBy = QDomElement());

    bool parseClipPath(const QDomElement &);
    bool parseClipMask(const QDomElement &e);

    bool parseMarker(const QDomElement &e);

    bool parseSymbol(const QDomElement &e);

    void parseMetadataApplyToShape(const QDomElement &e, KoShape *shape);

    qreal parseUnit(const QString &, bool horiz = false, bool vert = false, const QRectF &bbox = QRectF());

    qreal parseUnitX(const QString &unit);

    qreal parseUnitY(const QString &unit);

    qreal parseUnitXY(const QString &unit);

    qreal parseAngular(const QString &unit);

    KoShape *createObjectDirect(const QDomElement &b);

    KoShape * createObject(const QDomElement &, const SvgStyles &style = SvgStyles());

    KoShape * createPath(const QDomElement &);

    SvgGradientHelper* findGradient(const QString &id);

    QSharedPointer<KoVectorPatternBackground> findPattern(const QString &id, const KoShape *shape);

    SvgClipPathHelper* findClipPath(const QString &id);

    void addToGroup(QList<KoShape*> shapes, KoShapeContainer *group);

    KoShape * createShape(const QString &shapeID);

    KoShape * createShapeFromElement(const QDomElement &element, SvgLoadingContext &context);

    KoShape * createShapeFromCSS(const QDomElement e, const QString value, SvgLoadingContext &context, bool hideShapesFromDefs = true);

    QList<KoShape*> createListOfShapesFromCSS(const QDomElement e, const QString value, SvgLoadingContext &context, bool hideShapesFromDefs = true);

    void buildDocument(QList<KoShape*> shapes);

    void uploadStyleToContext(const QDomElement &e);
    void applyCurrentStyle(KoShape *shape, const QPointF &shapeToOriginalUserCoordinates);
    void applyCurrentBasicStyle(KoShape *shape);

    void applyStyle(KoShape *, const QDomElement &, const QPointF &shapeToOriginalUserCoordinates);

    void applyStyle(KoShape *, const SvgStyles &, const QPointF &shapeToOriginalUserCoordinates);

    void applyFillStyle(KoShape * shape);

    void applyStrokeStyle(KoShape * shape);

    void applyClipping(KoShape *shape, const QPointF &shapeToOriginalUserCoordinates);
    void applyMaskClipping(KoShape *shape, const QPointF &shapeToOriginalUserCoordinates);
    void applyMarkers(KoPathShape *shape);

    void applyPaintOrder(KoShape *shape);

    void applyId(const QString &id, KoShape *shape);

    void applyViewBoxTransform(const QDomElement &element);

    QDomText getTheOnlyTextChild(const QDomElement &e);

    bool shapeInDefs(const KoShape *shape);

private:
    SvgLoadingContext m_context;
    QMap<QString, SvgGradientHelper> m_gradients;
    QMap<QString, SvgClipPathHelper> m_clipPaths;
    QMap<QString, QSharedPointer<KoClipMask>> m_clipMasks;
    QMap<QString, QExplicitlySharedDataPointer<KoMarker>> m_markers;
    KoDocumentResourceManager *m_documentResourceManager;
    QList<KoShape*> m_shapes;
    QMap<QString, KoSvgSymbol*> m_symbols;
    QList<KoShape*> m_defsShapes;
    bool m_isInsideTextSubtree = false;
    QString m_documentTitle;
    QString m_documentDescription;
    QVector<KoID> m_warnings;
    QMap<KoShape *, QTransform> m_shapeParentTransform;
    bool m_inheritStrokeFillByDefault = false;
    bool m_resolveTextPropertiesForTopLevel = true;
};

#endif
