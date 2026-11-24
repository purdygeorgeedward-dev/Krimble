/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOSVGTEXTSHAPE_H
#define KOSVGTEXTSHAPE_H

#include "kritaflake_export.h"
#include <KoSvgTextProperties.h>

#include <KoShapeFactoryBase.h>
#include <SvgShape.h>
#include <KoShape.h>
#include <KoShapeBulkActionInterface.h>
#include <KoSvgText.h>
#include "html/HtmlSavingContext.h"
#include <QFlags>

class KoSvgTextShapeMemento;
class KoSvgTextNodeIndex;
struct KoSvgTextCharacterInfo;
class KoPathShape;
typedef QSharedPointer<KoSvgTextShapeMemento> KoSvgTextShapeMementoSP;

#define KoSvgTextShape_SHAPEID "KoSvgTextShapeID"
#define KoSvgTextShape_TEXTCONTOURGROUP "krita:textContourGroup"
class KRITAFLAKE_EXPORT KoSvgTextShape : public KoShape, public SvgShape, public KoShapeBulkActionInterface
{
public:
    KoSvgTextShape();
    KoSvgTextShape(const KoSvgTextShape &rhs);
    ~KoSvgTextShape() override;

    static const QString &defaultPlaceholderText();

    KoShape* cloneShape() const override;

    void paint(QPainter &painter) const override;
    void paintStroke(QPainter &painter) const override;

    QPainterPath outline() const override;
    QRectF outlineRect() const override;
    QRectF boundingRect() const override;

    QSizeF size() const override;
    void setSize(const QSizeF &size) override;

    enum class DebugElement {
        CharBbox = 1 << 0,
        LineBox = 1 << 1,
    };
    Q_DECLARE_FLAGS(DebugElements, DebugElement);
    void paintDebug(QPainter &painter, DebugElements elements) const;

    void relayout() const;

    KoShape * textOutline() const;

    enum TextType {
        PrePositionedText = 0,
        PreformattedText,
        InlineWrap,
        TextInShape
    };
    TextType textType() const;

    void setShapesInside(QList<KoShape*> shapesInside);

    void addShapeContours(QList<KoShape*> shapes, const bool inside = true);
    bool shapeInContours(KoShape *shape);

    void removeShapesFromContours(QList<KoShape*> shapes, bool callUpdate = true, bool cleanup = true);

    void moveShapeInsideToIndex(KoShape* shapeInside, const int index);

    bool setTextPathOnRange(KoShape *textPath, const int startPos = -1, const int endPos = -1);

    QList<KoShape*> textPathsAtRange(const int startPos = -1, const int endPos = -1);

    void addTextPathAtEnd(KoShape *textPath);

    QList<KoShape*> shapesInside() const;

    void setShapesSubtract(QList<KoShape*> shapesSubtract);

    QList<QPainterPath> textWrappingAreas() const;

    QList<KoShape*> shapesSubtract() const;

    KoShapeManager *internalShapeManager() const;

    QMap<QString, QString> shapeTypeSpecificStyles(SvgSavingContext &context) const;

    void setResolution(qreal xRes, qreal yRes) override;

    int posLeft(int pos, bool visual = false);
    int posRight(int pos, bool visual = false);
    int posUp(int pos, bool visual = false);
    int posDown(int pos, bool visual = false);

    int lineStart(int pos);
    int lineEnd(int pos);

    int wordLeft(int pos, bool visual = false);
    int wordRight(int pos, bool visual = false);

    int nextIndex(int pos);

    int previousIndex(int pos);

    int nextLine(int pos);
    int previousLine(int pos);

    int wordEnd(int pos);
    int wordStart(int pos);

    QPainterPath cursorForPos(int pos, QLineF &caret, QColor &color, double bidiFlagSize = 1.0);

    QPainterPath selectionBoxes(int pos, int anchor);

    QPainterPath underlines(int pos, int anchor,
                            KoSvgText::TextDecorations decor,
                            KoSvgText::TextDecorationStyle style,
                            qreal minimum, bool thick);

    int posForPoint(QPointF point, int start = -1, int end = -1, bool *overlaps = nullptr);

    int posForPointLineSensitive(QPointF point);
    int posForIndex(int index, bool firstIndex = false, bool skipSynthetic = false) const;
    int indexForPos(int pos) const;

    QPointF initialTextPosition() const;

    bool insertText(int pos, QString text);

    bool removeText(int &index, int &length);

    KoSvgTextProperties propertiesForPos(const int pos, bool inherited = false) const;

    QList<KoSvgTextProperties> propertiesForRange(const int startPos, const int endPos, bool inherited = false) const;

    void setPropertiesAtPos(int pos, KoSvgTextProperties properties);

    void mergePropertiesIntoRange(const int startPos,
                                  const int endPos,
                                  const KoSvgTextProperties properties,
                                  const QSet<KoSvgTextProperties::PropertyId> removeProperties = QSet<KoSvgTextProperties::PropertyId>());

    std::unique_ptr<KoSvgTextShape> copyRange(int index, int length) const;

    bool insertRichText(int pos, const KoSvgTextShape *richText, bool inheritPropertiesIfPossible = false);

    void cleanUp();

    bool setCharacterTransformsOnRange(const int startPos, const int endPos, const QVector<QPointF> positions, const QVector<qreal> rotateDegrees, const bool deltaPosition = true);

    QList<KoSvgTextCharacterInfo> getPositionsAndRotationsForRange(const int startPos, const int endPos) const;

    void removeTransformsFromRange(const int startPos, const int endPos);

    KoSvgTextNodeIndex findNodeIndexForPropertyId(KoSvgTextProperties::PropertyId propertyId);

    QPair<int, int> findRangeForNodeIndex(const KoSvgTextNodeIndex &node) const;

    KoSvgTextNodeIndex topLevelNodeForPos(int pos) const;

    KoSvgTextNodeIndex nodeForTextPath(KoShape *textPath) const;

    KoSvgTextProperties textProperties() const;
    QSharedPointer<KoShapeBackground> background() const override;
    void setBackground(QSharedPointer<KoShapeBackground> background) override;
    KoShapeStrokeModelSP stroke() const override;
    void setStroke(KoShapeStrokeModelSP stroke) override;
    QVector<PaintOrder> paintOrder() const override;
    void setPaintOrder(PaintOrder first, PaintOrder second) override;

    QString plainText();

    KoSvgText::WritingMode writingMode() const;

    KoSvgText::Direction direction() const;

    bool singleNode() const;

    struct KRITAFLAKE_EXPORT TextCursorChangeListener : public ShapeChangeListener {
        void notifyShapeChanged(ChangeType type, KoShape *shape) override;
        virtual void notifyCursorPosChanged(int pos, int anchor) = 0;
        virtual void notifyMarkupChanged() = 0;
    };
    void notifyCursorPosChanged(int pos, int anchor);
    void notifyMarkupChanged();

    void convertCharTransformsToPreformatted(bool makeInlineSize = false);

    void setCharacterTransformsFromLayout();

    bool saveSvg(SvgSavingContext &context) override;
    bool saveHtml(HtmlSavingContext &context);

    void enterNodeSubtree();
    void leaveNodeSubtree();

    KoSvgTextShapeMementoSP getMemento();

    void setMemento(const KoSvgTextShapeMementoSP memento);

    void setMemento(const KoSvgTextShapeMementoSP memento, int pos, int anchor);

    void debugParsing();

    void setRelayoutBlocked(const bool disable);

    bool relayoutIsBlocked() const;

    void setFontMatchingDisabled(const bool disable);

    bool fontMatchingDisabled() const;

    static QList<QPainterPath> generateTextAreas(const QList<KoShape*> shapesInside, const QList<KoShape*> shapesSubtract, const KoSvgTextProperties &props);

    void startBulkAction() override;
    QRectF endBulkAction() override;

protected:

    void shapeChanged(ChangeType type, KoShape *shape) override;
private:
    friend class TestSvgText;
    friend class KoSvgTextLoader;
    int nextPos(int pos, bool visual);

    int previousPos(int pos, bool visual);

    QPainterPath defaultCursorShape();

    void setMementoImpl(const KoSvgTextShapeMementoSP memento);

    class Private;
    QScopedPointer<Private> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoSvgTextShape::DebugElements)

class KRITAFLAKE_EXPORT KoSvgTextShapeMemento {
public:
    KoSvgTextShapeMemento() {}
    virtual ~KoSvgTextShapeMemento() {}
private:
    friend class KoSvgTextShape;
};

class KRITAFLAKE_EXPORT KoSvgTextNodeIndex {
public:
    KoSvgTextNodeIndex(const KoSvgTextNodeIndex &rhs);
    ~KoSvgTextNodeIndex();

    KoSvgTextProperties *properties();

    KoSvgText::TextOnPathInfo *textPathInfo();

    KoShape *textPath();

private:
    KoSvgTextNodeIndex();

private:
    friend class KoSvgTextShape;
    struct Private;
    QScopedPointer<Private> d;

};

struct KoSvgTextCharacterInfo {
    QPointF finalPos;
    QPointF advance;
    qreal rotateDeg;
    int visualIndex = -1;
    int logicalIndex = -1;
    bool rtl = false;
    bool middle = false;
    KoSvgText::FontMetrics metrics;

    static bool visualLessThan(const KoSvgTextCharacterInfo &a, const KoSvgTextCharacterInfo &b) {
        bool compare = false;
        if (a.visualIndex >= 0
                && b.visualIndex >= 0) {
            compare = a.visualIndex < b.visualIndex;
        } else {
            compare = a.logicalIndex < b.logicalIndex;
        }
        return compare;
    }
    static bool logicalLessThan(const KoSvgTextCharacterInfo &a, const KoSvgTextCharacterInfo &b) {
        return a.logicalIndex < b.logicalIndex;
    }
};

class KoSvgTextShapeFactory : public KoShapeFactoryBase
{

public:
    KoSvgTextShapeFactory();
    ~KoSvgTextShapeFactory() {}

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const override;

    KoShape *createShape(const KoProperties *params, KoDocumentResourceManager *documentResources = 0) const override;
    bool supports(const QDomElement &e, KoShapeLoadingContext &context) const override;
};

#endif
