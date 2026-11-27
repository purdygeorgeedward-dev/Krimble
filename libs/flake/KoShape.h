/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2008 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006, 2008 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2007-2009, 2011 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPE_H
#define KOSHAPE_H

#include "KoFlake.h"
#include "KoFlakeTypes.h"

#include <QSharedPointer>
#include <QSet>
#include <QMap>
#include <QMetaType>
#include <QSharedDataPointer>

#include <QDomDocument>

#include "kritaflake_export.h"

class QPainter;
class QRectF;
class QPainterPath;
class QTransform;

class KoShapeContainer;
class KoShapeStrokeModel;
class KoShapeUserData;
class KoViewConverter;
class KoShapeSavingContext;
class KoShapeLoadingContext;
class KoSnapData;
class KoClipPath;
class KoClipMask;
class KoShapeAnchor;
struct KoInsets;
class KoShapeBackground;
class KisHandlePainterHelper;
class KoShapeManager;

class KRITAFLAKE_EXPORT KoShape
{
public:
    enum ChangeType {
        PositionChanged,
        RotationChanged,
        ScaleChanged,
        ShearChanged,
        SizeChanged,
        GenericMatrixChange,
        KeepAspectRatioChange,
        ParentChanged,
        Deleted,
        StrokeChanged,
        BackgroundChanged,
        BorderChanged,
        ParameterChanged,
        ContentChanged,
        TextContourMarginChanged,
        ChildChanged,
        ConnectionPointChanged,
        ClipPathChanged,
        ClipMaskChanged,
        TransparencyChanged
    };

    enum PaintOrder {
        Fill,
        Stroke,
        Markers
    };

    KoShape();

    virtual ~KoShape();

    virtual KoShape* cloneShape() const;

    KoShape* cloneShapeAndBakeAbsoluteTransform() const;

    virtual void paint(QPainter &painter) const = 0;

    virtual void paintStroke(QPainter &painter) const;

    virtual void paintMarkers(QPainter &painter) const;

    void scale(qreal sx, qreal sy);

    void rotate(qreal angle);

    qreal rotation() const;

    void shear(qreal sx, qreal sy);

    virtual void setSize(const QSizeF &size);

    virtual QSizeF size() const;

    virtual void setPosition(const QPointF &position);

    QPointF position() const;

    virtual bool hitTest(const QPointF &position) const;

    virtual QRectF boundingRect() const;

    static QRectF boundingRect(const QList<KoShape*> &shapes);

    QRectF absoluteOutlineRect() const;

    static QRectF absoluteOutlineRect(const QList<KoShape*> &shapes);

    void setAnchor(KoShapeAnchor *anchor);

    KoShapeAnchor *anchor() const;

    void setMinimumHeight(qreal height);

    qreal minimumHeight() const;

    virtual void setBackground(QSharedPointer<KoShapeBackground> background);

    virtual QSharedPointer<KoShapeBackground> background() const;

    void setInheritBackground(bool value);

    bool inheritBackground() const;

    virtual bool hasTransparency() const;

    void setTransparency(qreal transparency);

    qreal transparency(bool recursive=false) const;

    qint16 zIndex() const;

    void setZIndex(qint16 zIndex);

    static const qint16 maxZIndex;

    static const qint16 minZIndex;

    void setVisible(bool on);
    bool isVisible(bool recursive = true) const;

    void setPrintable(bool on);

    bool isPrintable() const;

    void setSelectable(bool selectable);

    bool isSelectable() const;

    void setGeometryProtected(bool on);

    bool isGeometryProtected() const;

    void setContentProtected(bool protect);

    bool isContentProtected() const;

    KoShapeContainer *parent() const;

    void setParent(KoShapeContainer *parent);

    bool inheritsTransformFromAny(const QList<KoShape*> ancestorsInQuestion) const;

    bool hasCommonParent(const KoShape *shape) const;

    virtual void update() const;

    virtual void updateAbsolute(const QRectF &rect) const;

    enum ChildZOrderPolicy {
        ChildZDefault,
        ChildZParentChild = ChildZDefault,
        ChildZPassThrough
    };

    virtual ChildZOrderPolicy childZOrderPolicy();

    static bool compareShapeZIndex(KoShape *s1, KoShape *s2);

    virtual QPainterPath outline() const;

    virtual QRectF outlineRect() const;

    virtual KoShapeStrokeModelSP stroke() const;

    virtual void setStroke(KoShapeStrokeModelSP stroke);

    void setInheritStroke(bool value);

    bool inheritStroke() const;

    KoInsets strokeInsets() const;

    virtual void setPaintOrder(PaintOrder first, PaintOrder second);

    virtual QVector<PaintOrder> paintOrder() const;

    static QVector<PaintOrder> defaultPaintOrder();

    void setInheritPaintOrder(bool value);

    bool inheritPaintOrder() const;

    void setClipPath(KoClipPath *clipPath);

    KoClipPath * clipPath() const;

    void setClipMask(KoClipMask *clipMask);

    KoClipMask* clipMask() const;

    void setKeepAspectRatio(bool keepAspect);

    bool keepAspectRatio() const;

    QPointF absolutePosition(KoFlake::AnchorPosition anchor = KoFlake::Center) const;

    void setAbsolutePosition(const QPointF &newPosition, KoFlake::AnchorPosition anchor = KoFlake::Center);

    void setUserData(KoShapeUserData *userData);
    KoShapeUserData *userData() const;

    QString shapeId() const;

    void setShapeId(const QString &id);

    QTransform absoluteTransformation() const;

    void applyAbsoluteTransformation(const QTransform &matrix);

    void setTransformation(const QTransform &matrix);

    QTransform transformation() const;

    void applyTransformation(const QTransform &matrix);

    void copySettings(const KoShape *shape);

    static KisHandlePainterHelper createHandlePainterHelperView(QPainter *painter, KoShape *shape, const KoViewConverter &converter, qreal handleRadius = 0.0, int decorationThickness = 1);
    static KisHandlePainterHelper createHandlePainterHelperDocument(QPainter *painter, KoShape *shape, qreal handleRadius, int decorationThickness);

    QPointF shapeToDocument(const QPointF &point) const;

    QRectF shapeToDocument(const QRectF &rect) const;

    QPointF documentToShape(const QPointF &point) const;

    QRectF documentToShape(const QRectF &rect) const;

    QString name() const;

    void setName(const QString &name);

    void notifyChanged();

    virtual void waitUntilReady(bool asynchronous = true) const;

    virtual bool isShapeEditable(bool recursive = true) const;

    bool addDependee(KoShape *shape);

    void removeDependee(KoShape *shape);

    bool hasDependee(KoShape *shape) const;

    QList<KoShape*> dependees() const;

    virtual KoSnapData snapData() const;

    void setAdditionalAttribute(const QString &name, const QString &value);

    void removeAdditionalAttribute(const QString &name);

    bool hasAdditionalAttribute(const QString &name) const;

    QString additionalAttribute(const QString &name) const;

    void setAdditionalStyleAttribute(const char *name, const QString &value);

    void removeAdditionalStyleAttribute(const char *name);

    QSet<KoShape*> toolDelegates() const;

    void setToolDelegates(const QSet<KoShape*> &delegates);

    QString hyperLink () const;

    void setHyperLink(const QString &hyperLink);

    virtual void setResolution(qreal xRes, qreal yRes);

public:

    struct KRITAFLAKE_EXPORT ShapeChangeListener {
        virtual ~ShapeChangeListener();
        virtual void notifyShapeChanged(ChangeType type, KoShape *shape) = 0;

    private:
        friend class KoShape;
        void registerShape(KoShape *shape);
        void unregisterShape(KoShape *shape);
        void notifyShapeChangedImpl(ChangeType type, KoShape *shape);

        QList<KoShape*> m_registeredShapes;
    };

    void addShapeChangeListener(ShapeChangeListener *listener);
    void removeShapeChangeListener(ShapeChangeListener *listener);

protected:
    QList<ShapeChangeListener *> listeners() const;
    void setSizeImpl(const QSizeF &size) const;

public:
    static QList<KoShape*> linearizeSubtree(const QList<KoShape*> &shapes);
    static QList<KoShape *> linearizeSubtreeSorted(const QList<KoShape *> &shapes);
protected:
    KoShape(const KoShape &rhs);

    virtual void shapeChanged(ChangeType type, KoShape *shape = 0);

    QTransform transform() const;

private:
    class Private;
    QScopedPointer<Private> d;

    class SharedData;
    QSharedDataPointer<SharedData> s;

protected:
    void shapeChangedPriv(KoShape::ChangeType type);

private:
    void addShapeManager(KoShapeManager *manager);
    void removeShapeManager(KoShapeManager *manager);
    friend class KoShapeManager;
};

Q_DECLARE_METATYPE(KoShape*)
Q_DECLARE_METATYPE(QVector<KoShape::PaintOrder>)

#endif
