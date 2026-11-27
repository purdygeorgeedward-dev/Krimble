/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2022 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KO_SVG_TEXT_SHAPE_P_H
#define KO_SVG_TEXT_SHAPE_P_H

#include "KoSvgTextShape.h"

#include "KoSvgText.h"
#include "KoSvgTextContentElement.h"
#include <KoShapePainter.h>
#include <KoShapeManager.h>
#include <KoInsets.h>

#include "KoCssTextUtils.h"
#include <KoShapeGroup.h>

#include <kis_assert.h>
#include <KisForest.h>

#include <QFont>
#include <QImage>
#include <QLineF>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QVector>
#include <QtMath>

#include <variant>

#include <ft2build.h>
#include FT_FREETYPE_H

constexpr qreal SHAPE_PRECISION = 1e-6;

class KoPathShape;
struct raqm_glyph_t;

enum class BreakType {
    NoBreak,
    SoftBreak,
    HardBreak
};

enum class LineEdgeBehaviour {
    NoChange,
    Collapse,
    ForceHang,
    ConditionallyHang
};

struct CursorInfo {
    QLineF caret;
    QVector<int> graphemeIndices;
    QVector<QPointF> offsets;
    bool rtl = false;
    bool isWordBoundary = false;
    QColor color;
};

struct CursorPos {
    int cluster;
    int index;
    int offset = 0;
    bool synthetic = false;
};

namespace Glyph {

struct Outline {
    QPainterPath path;
};

struct Bitmap {
    QVector<QImage> images;
    QVector<QRectF> drawRects;
};

struct ColorLayers {
    QVector<QPainterPath> paths;
    QVector<QBrush> colors;
    QVector<bool> replaceWithForeGroundColor;
};

using Variant = std::variant<std::monostate, Outline, Bitmap, ColorLayers>;

}

struct CharacterResult {
    QPointF finalPosition;
    qreal rotate = 0.0;
    bool hidden = false;
    bool addressable = true;
    bool middle = false;
    bool anchored_chunk = false;

    Glyph::Variant glyph;

    QRectF inkBoundingBox;
    bool isHorizontal = true;
    int visualIndex = -1;
    int plaintTextIndex = -1;
    QPointF cssPosition = QPointF();
    QPointF textLengthOffset = QPointF();
    QPointF textPathAndAnchoringOffset = QPointF();
    QPointF dominantBaselineOffset = QPointF();
    QPointF baselineOffset = QPointF();
    QPointF advance;
    BreakType breakType = BreakType::NoBreak;
    LineEdgeBehaviour lineEnd = LineEdgeBehaviour::NoChange;
    LineEdgeBehaviour lineStart = LineEdgeBehaviour::NoChange;
    bool justifyBefore = false;
    bool justifyAfter = false;
    bool isHanging = false;
    bool textLengthApplied = false;
    bool overflowWrap = false;

    qreal extraFontScaling = 1.0;
    qreal fontHalfLeading;
    KoSvgText::FontMetrics metrics;
    qreal scaledHalfLeading{};
    qreal scaledAscent{};
    qreal scaledDescent{};

    std::optional<qreal> tabSize;

    void calculateAndApplyTabsize(QPointF currentPos, bool isHorizontal, const KoSvgText::ResolutionHandler &resHandler) {
        if (!tabSize) return;
        if (*tabSize == qInf() || qIsNaN(*tabSize)) return;

        if (*tabSize > 0) {
            qreal remainder = *tabSize - (isHorizontal? fmod(currentPos.x(), *tabSize): fmod(currentPos.y(), *tabSize));
            advance = resHandler.adjust(isHorizontal? QPointF(remainder, advance.y()): QPointF(advance.x(), remainder));
        }
    }

    QRectF layoutBox() const {
        return isHorizontal? QRectF(0, advance.y()+scaledAscent, advance.x(), scaledDescent-scaledAscent)
                         : QRectF(advance.x()+scaledDescent, 0, scaledAscent-scaledDescent, advance.y());
    }
    QRectF lineHeightBox () const {
        QRectF lBox = layoutBox();
        return isHorizontal? lBox.adjusted(0, -scaledHalfLeading, 0, scaledHalfLeading)
                         : lBox.adjusted(-scaledHalfLeading, 0, scaledHalfLeading, 0);
    }

    void translateOrigin(QPointF newOrigin) {
        if (newOrigin == QPointF()) return;
        if (Glyph::Outline *outlineGlyph = std::get_if<Glyph::Outline>(&glyph)) {
            outlineGlyph->path.translate(-newOrigin);
        } else if (Glyph::Bitmap *bitmapGlyph = std::get_if<Glyph::Bitmap>(&glyph)) {
            for (int i = 0; i< bitmapGlyph->drawRects.size(); i++) {
                bitmapGlyph->drawRects[i].translate(-newOrigin);
            }
        } else if  (Glyph::ColorLayers *colorGlyph = std::get_if<Glyph::ColorLayers>(&glyph)) {
            for (int i = 0; i< colorGlyph->paths.size(); i++) {
                colorGlyph->paths[i].translate(-newOrigin);
            }
        }
        cursorInfo.caret.translate(-newOrigin);
        inkBoundingBox.translate(-newOrigin);

        if (isHorizontal) {
            scaledDescent -= newOrigin.y();
            scaledAscent -= newOrigin.y();
        } else {
            scaledDescent -= newOrigin.x();
            scaledAscent -= newOrigin.x();
        }
    }

    void scaleCharacterResult(qreal xScale, qreal yScale) {
        QTransform scale = QTransform::fromScale(xScale, yScale);
        if (scale.isIdentity()) return;
        const bool scaleToZero = !(xScale > 0 && yScale > 0);

        if (Glyph::Outline *outlineGlyph = std::get_if<Glyph::Outline>(&glyph)) {
            if (!outlineGlyph->path.isEmpty()) {
                if (scaleToZero) {
                    outlineGlyph->path = QPainterPath();
                } else {
                    outlineGlyph->path = scale.map(outlineGlyph->path);
                }
            }
        } else if (Glyph::Bitmap *bitmapGlyph = std::get_if<Glyph::Bitmap>(&glyph)) {
            if (scaleToZero) {
                bitmapGlyph->drawRects.clear();
                bitmapGlyph->images.clear();
            } else {
                for (int i = 0; i< bitmapGlyph->drawRects.size(); i++) {
                    bitmapGlyph->drawRects[i] = scale.mapRect(bitmapGlyph->drawRects[i]);
                }
            }
        } else if  (Glyph::ColorLayers *colorGlyph = std::get_if<Glyph::ColorLayers>(&glyph)) {
            for (int i = 0; i< colorGlyph->paths.size(); i++) {
                if (scaleToZero) {
                    colorGlyph->paths[i] = QPainterPath();
                } else {
                    colorGlyph->paths[i] = scale.map(colorGlyph->paths[i]);
                }
            }
        }
        advance = scale.map(advance);
        cursorInfo.caret = scale.map(cursorInfo.caret);
        for (int i = 0; i < cursorInfo.offsets.size(); i++) {
            cursorInfo.offsets[i] = scale.map(cursorInfo.offsets.at(i));
        }
        inkBoundingBox = scale.mapRect(inkBoundingBox);

        if (isHorizontal) {
            scaledDescent *= yScale;
            scaledAscent *= yScale;
            scaledHalfLeading *= yScale;
            metrics.scaleBaselines(yScale);
            if (tabSize) {
                tabSize = scale.map(QPointF(*tabSize, *tabSize)).x();
            }
        } else {
            scaledDescent *= xScale;
            scaledAscent *= xScale;
            scaledHalfLeading *= xScale;
            metrics.scaleBaselines(xScale);
            if (tabSize) {
                tabSize = scale.map(QPointF(*tabSize, *tabSize)).y();
            }
        }
    }

    QPointF totalBaselineOffset() const {
        return baselineOffset+dominantBaselineOffset;
    }

    QFont::Style fontStyle = QFont::StyleNormal;
    int fontWeight = 400;

    CursorInfo cursorInfo;

    KoSvgText::TextAnchor anchor = KoSvgText::AnchorStart;
    KoSvgText::Direction direction = KoSvgText::DirectionLeftToRight;

    QTransform finalTransform() const {
        QTransform tf =
            QTransform::fromTranslate(finalPosition.x(), finalPosition.y());
        tf.rotateRadians(rotate);
        return tf;
    }
};

struct LineChunk {
    QLineF length;
    QVector<int> chunkIndices;
    QRectF boundingBox;
    QPointF conditionalHangEnd = QPointF();
};

struct LineBox {

    LineBox() {
    }

    LineBox(QPointF start, QPointF end, const KoSvgText::ResolutionHandler &resHandler) {
        LineChunk chunk;
        chunk.length =  QLineF(resHandler.adjustCeil(start), resHandler.adjustFloor(end));
        chunks.append(chunk);
        currentChunk = 0;
    }

    LineBox(QVector<QLineF> lineWidths, bool ltr, QPointF indent, const KoSvgText::ResolutionHandler &resHandler) {
        textIndent = indent;
        if (ltr) {
            Q_FOREACH(QLineF line, lineWidths) {
                LineChunk chunk;
                chunk.length = QLineF(resHandler.adjustCeil(line.p1()), resHandler.adjustFloor(line.p2()));
                chunks.append(chunk);
                currentChunk = 0;
            }
        } else {
            Q_FOREACH(QLineF line, lineWidths) {
                LineChunk chunk;
                chunk.length = QLineF(resHandler.adjustFloor(line.p2()), resHandler.adjustCeil(line.p1()));
                chunks.insert(0, chunk);
                currentChunk = 0;
            }
        }
    }

    QVector<LineChunk> chunks;
    int currentChunk = -1;

    qreal expectedLineTop = 0;
    qreal actualLineTop = 0;
    qreal actualLineBottom = 0;

    QPointF baselineTop = QPointF();
    QPointF baselineBottom = QPointF();

    QPointF textIndent = QPointF();
    bool firstLine = false;
    bool lastLine = false;
    bool lineFinalized = false;
    bool justifyLine = false;

    LineChunk chunk() {
        return chunks.value(currentChunk);
    }

    void setCurrentChunk(LineChunk chunk) {
        currentChunk = qMax(currentChunk, 0);
        if (currentChunk < chunks.size()) {
            chunks[currentChunk] = chunk;
        } else {
            chunks.append(chunk);
        }
    }

    void clearAndAdjust(bool isHorizontal, QPointF current, QPointF indent) {
        actualLineBottom = 0;
        actualLineTop = 0;
        LineChunk chunk;
        textIndent = indent;
        QLineF length = chunks.at(currentChunk).length;
        if (isHorizontal) {
            length.setP1(QPointF(length.p1().x(), current.y()));
            length.setP2(QPointF(length.p2().x(), current.y()));
        } else {
            length.setP1(QPointF(current.x(), length.p1().y()));
            length.setP2(QPointF(current.x(), length.p2().y()));
        }
        chunks.clear();
        currentChunk = 0;
        chunk.length = length;
        chunks.append(chunk);
        firstLine = false;
    }

    void setCurrentChunkForPos(QPointF pos, bool isHorizontal) {
        for (int i=0; i<chunks.size(); i++) {
            LineChunk chunk = chunks.at(i);
            if (isHorizontal) {
                qreal min = qMin(chunk.length.p1().x(), chunk.length.p2().x()) - SHAPE_PRECISION;
                qreal max = qMax(chunk.length.p1().x(), chunk.length.p2().x()) + SHAPE_PRECISION;
                if ((pos.x() < max) &&
                        (pos.x() >= min)) {
                        currentChunk = i;
                        break;
                }
            } else {
                qreal min = qMin(chunk.length.p1().y(), chunk.length.p2().y()) - SHAPE_PRECISION;
                qreal max = qMax(chunk.length.p1().y(), chunk.length.p2().y()) + SHAPE_PRECISION;
                if ((pos.y() < max) &&
                        (pos.y() >= min)) {
                    currentChunk = i;
                    break;
                }
            }
        }
    }

    bool isEmpty() {
        if (chunks.isEmpty()) return true;
        for (int i =0; i < chunks.size(); i++) {
            if (!chunks.at(i).chunkIndices.isEmpty()) return false;
        }
        return true;
    }

};

struct SubChunk {

    SubChunk(KisForest<KoSvgTextContentElement>::child_iterator leaf):
        associatedLeaf(leaf){

    }

    QString text;
    QString originalText;
    KisForest<KoSvgTextContentElement>::child_iterator associatedLeaf;
    QVector<QPair<int, int>> newToOldPositions;
    bool textInPath = false;
    bool firstTextInPath = false;
    KoSvgTextProperties inheritedProps;
    QSharedPointer<KoShapeBackground> bg;
};

class KRITAFLAKE_EXPORT KoSvgTextShape::Private
{
public:

    Private()
        : internalShapesPainter(new KoShapePainter)
        , shapeGroup(new KoShapeGroup)
    {
        shapeGroup->setSelectable(false);
    }

    enum InternalShapeState {
        Decorative,
        ShapeInside,
        ShapeSubtract,
        TextPath
    };

    Private(const Private &rhs)
        : internalShapesPainter(new KoShapePainter)
        , textData(rhs.textData)
    {

        KoShapeGroup *g = dynamic_cast<KoShapeGroup*>(rhs.shapeGroup.data()->cloneShape());
        shapeGroup.reset(g);
        shapeGroup->setSelectable(false);
        handleShapes(shapeGroup->shapes(), rhs.shapeGroup->shapes(), rhs.shapesInside, shapesInside);
        handleShapes(shapeGroup->shapes(), rhs.shapeGroup->shapes(), rhs.shapesSubtract, shapesSubtract);
        handleShapes(shapeGroup->shapes(), rhs.shapeGroup->shapes(), rhs.textPaths, textPaths);
        updateInternalShapesList();

        yRes = rhs.yRes;
        xRes = rhs.xRes;
        result = rhs.result;
        lineBoxes = rhs.lineBoxes;

        cursorPos = rhs.cursorPos;
        logicalToVisualCursorPos = rhs.logicalToVisualCursorPos;
        plainText = rhs.plainText;
        isBidi = rhs.isBidi;
        initialTextPosition = rhs.initialTextPosition;

        isLoading = rhs.isLoading;
        disableFontMatching = rhs.disableFontMatching;

        currentTextWrappingAreas = rhs.currentTextWrappingAreas;
    }

    void handleShapes(const QList<KoShape*> &sourceShapeList, const QList<KoShape*> referenceList2, const QList<KoShape*> referenceShapeList, QList<KoShape*> &destinationShapeList) {
        for (int i = 0; i<sourceShapeList.size(); i++) {
            if (referenceShapeList.contains(referenceList2.at(i))) {
                destinationShapeList.append(sourceShapeList.at(i));
            }
        }
    }

    ~Private() {

        internalShapesPainter.reset();
        Q_FOREACH(KoShape *shape, shapeGroup->shapes()) {
            shapeGroup->removeShape(shape);
        }
        shapeGroup.reset();
        qDeleteAll(shapesInside);
        shapesInside.clear();
        qDeleteAll(shapesSubtract);
        shapesSubtract.clear();
        qDeleteAll(textPaths);
        textPaths.clear();
    }

    int xRes = 72;
    int yRes = 72;

    QScopedPointer<KoShapePainter> internalShapesPainter;
    QScopedPointer<KoShapeGroup> shapeGroup;

    QList<KoShape*> internalShapes() const {
        return internalShapesPainter->internalShapeManager()->shapes();
    }

    void updateShapeGroup() {
        Q_FOREACH(KoShape *shape, shapeGroup->shapes()) {
            shapeGroup->removeShape(shape);
        }
        Q_FOREACH(KoShape *shape, shapesInside) {
            shapeGroup->addShape(shape);
        }
        Q_FOREACH(KoShape *shape, shapesSubtract) {
            shapeGroup->addShape(shape);
        }
        Q_FOREACH(KoShape *shape, textPaths) {
            shapeGroup->addShape(shape);
        }
        updateTextWrappingAreas();
        updateInternalShapesList();
    }
    void updateInternalShapesList() {
        if (shapeGroup) {
            internalShapesPainter->setShapes(shapeGroup->shapes());
        }
    }

    struct BulkActionState {
        BulkActionState(QRectF originalBoundingRectArg) : originalBoundingRect(originalBoundingRectArg) {}

        QRectF originalBoundingRect;
        bool contourHasChanged = false;
        bool layoutHasChanged = false;
        bool layoutSetFromMemento = false;

        bool changed() const {
            return contourHasChanged || layoutHasChanged || layoutSetFromMemento;
        }
    };

    std::optional<BulkActionState> bulkActionState;

    QList<KoShape*> shapesInside;
    QList<KoShape*> shapesSubtract;
    QList<KoShape*> textPaths;

    QList<QPainterPath> currentTextWrappingAreas;

    void updateTextWrappingAreas();
    static QList<QPainterPath> generateShapes(const QList<KoShape*> shapesInside, const QList<KoShape*> shapesSubtract, const KoSvgTextProperties &properties);

    static KoShape *textPathByName(QString name, QList<KoShape*> textPaths) {
        auto it = std::find_if(textPaths.begin(), textPaths.end(), [&name](const KoShape *s) -> bool {return s->name() == name;});
        return it != textPaths.end()? *it: nullptr;
    }

    CursorPos getCursorPos(int pos) {
        return cursorPos.at(qBound(0, pos, cursorPos.size()-1));
    }

    KisForest<KoSvgTextContentElement> textData;
    bool isLoading = false;

    bool disableFontMatching = false;

    QVector<CharacterResult> result;
    QVector<LineBox> lineBoxes;

    QVector<CursorPos> cursorPos;
    QMap<int, int> logicalToVisualCursorPos;

    QString plainText;
    bool isBidi = false;
    QPointF initialTextPosition = QPointF();

    void relayout();

    static bool loadGlyph(const KoSvgText::ResolutionHandler &resHandler,
                   const FT_Int32 faceLoadFlags,
                   const bool isHorizontal,
                   const char32_t firstCodepoint,
                   const KoSvgText::TextRendering rendering,
                   raqm_glyph_t &currentGlyph,
                   CharacterResult &charResult,
                   QPointF &totalAdvanceFTFontCoordinates);

    static QVector<QPointF> getLigatureCarets(const KoSvgText::ResolutionHandler &resHandler,
                                  const bool isHorizontal,
                                  raqm_glyph_t &currentGlyph);

    static std::pair<QTransform, qreal> loadGlyphOnly(const QTransform &ftTF,
                                               FT_Int32 faceLoadFlags,
                                               bool isHorizontal,
                                               raqm_glyph_t &currentGlyph,
                                               CharacterResult &charResult, const KoSvgText::TextRendering rendering);

    void clearAssociatedOutlines();
    static void resolveTransforms(KisForest<KoSvgTextContentElement>::child_iterator currentTextElement,
                           QString text, QVector<CharacterResult> &result, int &currentIndex,
                           bool isHorizontal, bool wrapped, bool textInPath, QVector<KoSvgText::CharTransformation> &resolved,
                           QVector<bool> collapsedChars, const KoSvgTextProperties resolvedProps, bool withControls = true);

    void applyTextLength(KisForest<KoSvgTextContentElement>::child_iterator currentTextElement, QVector<CharacterResult> &result, int &currentIndex, int &resolvedDescendentNodes, bool isHorizontal,
                         const KoSvgTextProperties resolvedProps, const KoSvgText::ResolutionHandler &resHandler);
    static void applyAnchoring(QVector<CharacterResult> &result, bool isHorizontal, const KoSvgText::ResolutionHandler resHandler);
    static qreal anchoredChunkShift(const QVector<CharacterResult> &result, const bool isHorizontal, const int start, int &end);
    static qreal
    characterResultOnPath(CharacterResult &cr, qreal length, qreal offset, bool isHorizontal, bool isClosed);
    static QPainterPath stretchGlyphOnPath(const QPainterPath &glyph,
                                           const QPainterPath &path,
                                           bool isHorizontal,
                                           qreal offset,
                                           bool isClosed);
    static void applyTextPath(KisForest<KoSvgTextContentElement>::child_iterator parent, QVector<CharacterResult> &result, bool isHorizontal, QPointF &startPos, const KoSvgTextProperties resolvedProps, QList<KoShape*> textPaths);
    static void computeFontMetrics(KisForest<KoSvgTextContentElement>::child_iterator parent, const KoSvgTextProperties &parentProps,
                            const KoSvgText::FontMetrics &parentBaselineTable, const KoSvgText::Baseline parentBaseline,
                            QVector<CharacterResult> &result,
                            int &currentIndex,
                            const KoSvgText::ResolutionHandler resHandler,
                            const bool isHorizontal,
                            const bool disableFontMatching);
    static void handleLineBoxAlignment(KisForest<KoSvgTextContentElement>::child_iterator parent,
                            QVector<CharacterResult> &result, const QVector<LineBox> lineBoxes,
                            int &currentIndex,
                            const bool isHorizontal, const KoSvgTextProperties resolvedProps);
    void computeTextDecorations(KisForest<KoSvgTextContentElement>::child_iterator currentTextElement,
                                const QVector<CharacterResult>& result,
                                const QMap<int, int>& logicalToVisual,
                                const KoSvgText::ResolutionHandler resHandler,
                                KoPathShape *textPath,
                                qreal textPathoffset,
                                bool side,
                                int &currentIndex,
                                bool isHorizontal,
                                bool ltr,
                                bool wrapping,
                                const KoSvgTextProperties resolvedProps, QList<KoShape*> textPaths);
    QMap<KoSvgText::TextDecoration, QPainterPath> generateDecorationPaths(const int &start, const int &end,
                                                                          const KoSvgText::ResolutionHandler resHandler,
                                                                          const QVector<CharacterResult> &result,
                                                                          const bool isHorizontal,
                                                                          const KoSvgText::TextDecorations &decor,
                                                                          const KoSvgText::TextDecorationStyle style = KoSvgText::TextDecorationStyle::Solid,
                                                                          const bool textDecorationSkipInset = false,
                                                                          const KoPathShape *currentTextPath = nullptr,
                                                                          const qreal currentTextPathOffset = 0.0,
                                                                          const bool textPathSide = false,
                                                                          const KoSvgText::TextDecorationUnderlinePosition underlinePosH = KoSvgText::TextDecorationUnderlinePosition::UnderlineAuto,
                                                                          const KoSvgText::TextDecorationUnderlinePosition underlinePosV = KoSvgText::TextDecorationUnderlinePosition::UnderlineAuto);
    static void finalizeDecoration (
            QPainterPath decorationPath,
            const QPointF offset,
            const QPainterPathStroker &stroker,
            const KoSvgText::TextDecoration type,
            QMap<KoSvgText::TextDecoration, QPainterPath> &decorationPaths,
            const KoPathShape *currentTextPath,
            const bool isHorizontal,
            const qreal currentTextPathOffset,
            const bool textPathSide
            );

    void paintTextDecoration(QPainter &painter,
                             const QPainterPath &outlineRect,
                             const KoShape *rootShape,
                             const KoSvgText::TextDecoration type,
                             const KoSvgText::TextRendering rendering);
    void paintPaths(QPainter &painter,
                    const QPainterPath &outlineRect,
                    const KoShape *rootShape,
                    const QVector<CharacterResult> &result,
                    const KoSvgText::TextRendering rendering,
                    QPainterPath &chunk,
                    int &currentIndex);
    KoShape* collectPaths(const KoSvgTextShape *rootShape, QVector<CharacterResult> &result, int &currentIndex);
    void paintDebug(QPainter &painter,
                    const QVector<CharacterResult> &result,
                    int &currentIndex);

    static int numChars(KisForest<KoSvgTextContentElement>::child_iterator parent, bool withControls = false, KoSvgTextProperties resolvedProps = KoSvgTextProperties::defaultProperties()) {
        KoSvgTextProperties props = parent->properties;
        props.inheritFrom(resolvedProps, true);
        int count = parent->numChars(withControls, props);
        for (auto it = KisForestDetail::childBegin(parent); it != KisForestDetail::childEnd(parent); it++) {
            count += numChars(it, withControls, props);
        }
        return count;
    }

    static int childCount(KisForest<KoSvgTextContentElement>::child_iterator it) {
        return std::distance(KisForestDetail::childBegin(it), KisForestDetail::childEnd(it));
    }
    static QVector<SubChunk> collectSubChunks(KisForest<KoSvgTextContentElement>::child_iterator it, KoSvgTextProperties parent, bool textInPath, bool &firstTextInPath);

    static KisForest<KoSvgTextContentElement>::depth_first_tail_iterator findTextContentElementForIndex(KisForest<KoSvgTextContentElement> &tree,
                                                                                                        int &currentIndex,
                                                                                                        int sought,
                                                                                                        bool skipZeroWidth = false)
    {
        auto it = tree.depthFirstTailBegin();
        for (; it != tree.depthFirstTailEnd(); it++) {
            if (childCount(siblingCurrent(it)) > 0) {
                continue;
            }
            int length = it->numChars(false);
            if (length == 0 && skipZeroWidth) {
                continue;
            }

            if (sought == currentIndex || (sought > currentIndex && sought < currentIndex + length)) {
                break;
            } else {
                currentIndex += length;
            }
        }
        return it;
    }
    static bool splitContentElement(KisForest<KoSvgTextContentElement> &tree, int index, bool allowEmptyText = true) {
        int currentIndex = 0;

        auto contentElement = depth(tree) == 1? tree.depthFirstTailBegin(): findTextContentElementForIndex(tree, currentIndex, index, true);
        if (contentElement == tree.depthFirstTailEnd()) return false;

        bool suitableStartIndex = siblingCurrent(contentElement) == tree.childBegin()? index >= currentIndex: index > currentIndex;
        bool suitableEndIndex = siblingCurrent(contentElement) == tree.childBegin()? true: index < currentIndex + contentElement->numChars(false);

        if (suitableStartIndex && suitableEndIndex) {
            KoSvgTextContentElement duplicate = KoSvgTextContentElement();
            duplicate.text = contentElement->text;
            int start = index - currentIndex;
            int length = contentElement->numChars(false) - start;
            int zero = 0;
            duplicate.removeText(start, length);

            if (!allowEmptyText && (duplicate.text.isEmpty() || length == 0)) {
                return true;
            }

            if (siblingCurrent(contentElement) != tree.childBegin()
                    && contentElement->textPathId.isEmpty()
                    && contentElement->textLength.isAuto
                    && contentElement->localTransformations.isEmpty()) {
                contentElement->removeText(zero, start);
                duplicate.properties = contentElement->properties;
                tree.insert(siblingCurrent(contentElement), duplicate);
            } else {
                KoSvgTextContentElement duplicate2 = KoSvgTextContentElement();
                duplicate2.text = contentElement->text;
                duplicate2.removeText(zero, start);
                contentElement->text.clear();
                tree.insert(childBegin(contentElement), duplicate);
                tree.insert(childEnd(contentElement), duplicate2);
            }
            return true;
        }
        return false;
    }

    static void splitTree(KisForest<KoSvgTextContentElement> &tree, int index, bool textPathAfterSplit) {
        splitContentElement(tree, index, false);
        int currentIndex = 0;
        auto contentElement = depth(tree) == 1? tree.depthFirstTailBegin(): findTextContentElementForIndex(tree, currentIndex, index, true);

        if (contentElement == tree.depthFirstTailEnd()) return;
        if (siblingCurrent(contentElement) == tree.childBegin()) return;

        auto lastNode = siblingCurrent(contentElement);
        for (auto parentIt = KisForestDetail::hierarchyBegin(siblingCurrent(contentElement));
             parentIt != KisForestDetail::hierarchyEnd(siblingCurrent(contentElement)); parentIt++) {
            if (lastNode == siblingCurrent(parentIt)) continue;
            if (siblingCurrent(parentIt) == tree.childBegin()) {
                break;
            }

            if (lastNode != childBegin(siblingCurrent(parentIt))) {
                KoSvgTextContentElement duplicate = KoSvgTextContentElement();
                duplicate.properties = parentIt->properties;
                if (textPathAfterSplit) {
                    duplicate.textPathId = parentIt->textPathId;
                    duplicate.textPathInfo = parentIt->textPathInfo;
                    parentIt->textPathId = QString();
                    parentIt->textPathInfo = KoSvgText::TextOnPathInfo();
                }
                auto insert = siblingCurrent(parentIt);
                insert ++;
                auto it = tree.insert(insert, duplicate);

                QVector<KisForest<KoSvgTextContentElement>::child_iterator> movableChildren;
                for (auto child = lastNode; child != childEnd(siblingCurrent(parentIt)); child++) {
                    movableChildren.append(child);
                }
                while(!movableChildren.isEmpty()) {
                    auto child = movableChildren.takeLast();
                    tree.move(child, childBegin(it));
                }
                lastNode = it;
            } else {
                lastNode = siblingCurrent(parentIt);
            }
        }
    }

    static KisForest<KoSvgTextContentElement>::child_iterator findTopLevelParent(KisForest<KoSvgTextContentElement>::child_iterator root,
                                                                                 KisForest<KoSvgTextContentElement>::child_iterator child) {
        if (!child.node()) return childEnd(root);
        if (KisForestDetail::parent(child) == root) return child;
        for (auto rootChild = childBegin(root); rootChild != childEnd(root); rootChild++) {
            for (auto leaf = KisForestDetail::tailSubtreeBegin(rootChild);
                 leaf != KisForestDetail::tailSubtreeEnd(rootChild); leaf++) {
                if (siblingCurrent(leaf) == child) {
                    return rootChild;
                }
            }
        }
        return childEnd(root);
    }

    static QVector<bool> collapsedWhiteSpacesForText(KisForest<KoSvgTextContentElement> &tree, QString &allText, const bool alsoCollapseLowSurrogate = false, bool includeBidiControls = false) {
        QMap<int, KoSvgText::TextSpaceCollapse> collapseModes;

        QList<KoSvgTextProperties> parentProps = {KoSvgTextProperties::defaultProperties()};
        for (auto it = tree.compositionBegin(); it != tree.compositionEnd(); it++) {
            if (it.state() == KisForestDetail::Enter) {
                KoSvgTextProperties ownProperties = it->properties;
                ownProperties.inheritFrom(parentProps.last());
                parentProps.append(ownProperties);

                const int children = childCount(siblingCurrent(it));
                if (children == 0) {
                    QString text = it->text;
                    if (includeBidiControls) {
                        KoSvgText::UnicodeBidi bidi = KoSvgText::UnicodeBidi(parentProps.last().propertyOrDefault(KoSvgTextProperties::UnicodeBidiId).toInt());
                        KoSvgText::Direction direction = KoSvgText::Direction(parentProps.last().propertyOrDefault(KoSvgTextProperties::DirectionId).toInt());
                        QVector<QPair<int, int>> positions;
                        QString text = KoCssTextUtils::getBidiOpening(direction, bidi);
                        text += it->getTransformedString(positions, parentProps.last());
                        text += KoCssTextUtils::getBidiClosing(bidi);
                    }
                    KoSvgText::TextSpaceCollapse collapse = KoSvgText::TextSpaceCollapse(parentProps.last().propertyOrDefault(KoSvgTextProperties::TextCollapseId).toInt());
                    collapseModes.insert(allText.size(), collapse);
                    allText += text;
                }
            } else {
                parentProps.pop_back();
            }
        }
        QVector<bool> collapsed = KoCssTextUtils::collapseSpaces(&allText, collapseModes);

        if (alsoCollapseLowSurrogate) {
            for (int i = 0; i < allText.size(); i++) {
                if (i > 0 && allText.at(i).isLowSurrogate() && allText.at(i-1).isHighSurrogate()) {
                    collapsed[i] = true;
                }
            }
        }
        return collapsed;
    }

    static void removeTransforms(KisForest<KoSvgTextContentElement> &tree, const int start, const int length) {
        QString all;
        QVector<bool> collapsedCharacters = collapsedWhiteSpacesForText(tree, all, true);

        auto root = tree.childBegin();
        removeTransformsImpl(root, 0, start, length, collapsedCharacters);
    }

    static int removeTransformsImpl(KisForest<KoSvgTextContentElement>::child_iterator currentTextElement, const int globalIndex, const int start, const int length, const QVector<bool> collapsedCharacters) {
        int currentLength = 0;
        auto it = childBegin(currentTextElement);
        if (it != childEnd(currentTextElement)) {
            for (; it != childEnd(currentTextElement); it++) {
                currentLength += removeTransformsImpl(it, globalIndex + currentLength, start, length, collapsedCharacters);
            }
        } else {
            currentLength = currentTextElement->text.size();
        }

        if (!currentTextElement->localTransformations.isEmpty()) {
            int transformOffset = 0;
            int transformOffsetEnd = 0;

            for (int i = globalIndex; i < globalIndex + currentLength; i++) {
                if (i >= collapsedCharacters.size()) break;
                if (i < start) {
                    transformOffset += collapsedCharacters.at(i)? 0: 1;
                }
                if (i < start + length) {
                    transformOffsetEnd += collapsedCharacters.at(i)? 0: 1;
                } else {
                    break;
                }
            }
            if (transformOffset < currentTextElement->localTransformations.size()) {
                int length = qBound(0, transformOffsetEnd-transformOffset, qMax(0, currentTextElement->localTransformations.size()-transformOffset));
                currentTextElement->localTransformations.remove(transformOffset,
                                                                length);
            }

        }
        return currentLength;
    }

    static void insertTransforms(KisForest<KoSvgTextContentElement> &tree, const int start, const int length, const bool allowSkipFirst) {
        QString all;
        QVector<bool> collapsedCharacters = collapsedWhiteSpacesForText(tree, all, true);

        auto root = tree.childBegin();
        insertTransformsImpl(root, 0, start, length, collapsedCharacters, allowSkipFirst);
    }
    static int insertTransformsImpl(KisForest<KoSvgTextContentElement>::child_iterator currentTextElement, const int globalIndex, const int start, const int length, const QVector<bool> collapsedCharacters, const bool allowSkipFirst) {
        int currentLength = 0;
        auto it = childBegin(currentTextElement);
        if (it != childEnd(currentTextElement)) {
            for (; it != childEnd(currentTextElement); it++) {
                currentLength += insertTransformsImpl(it, globalIndex + currentLength, start, length, collapsedCharacters, allowSkipFirst);
            }
        } else {
            currentLength = currentTextElement->text.size();
        }

        if (!currentTextElement->localTransformations.isEmpty()) {
            int transformOffset = 0;
            int transformOffsetEnd = 0;

            for (int i = globalIndex; i < globalIndex + currentLength; i++) {
                if (i < start) {
                    transformOffset += collapsedCharacters.at(i)? 0: 1;
                }
                if (i < start + length) {
                    transformOffsetEnd += collapsedCharacters.at(i)? 0: 1;
                } else {
                    break;
                }
            }

            if (transformOffset == 0 && allowSkipFirst && currentTextElement->localTransformations.at(0).startsNewChunk()) {
                transformOffset += 1;
            }

            if (transformOffset < currentTextElement->localTransformations.size()) {
                for (int i = transformOffset; i < transformOffsetEnd; i++) {
                    currentTextElement->localTransformations.insert(i, KoSvgText::CharTransformation());
                }
            }

        }
        return currentLength;
    }

    void applyWhiteSpace(KisForest<KoSvgTextContentElement> &tree, const bool convertToPreWrapped = false) {
        QString allText;
        QVector<bool> collapsed = collapsedWhiteSpacesForText(tree, allText, false);

        auto end = std::make_reverse_iterator(tree.childBegin());
        auto begin = std::make_reverse_iterator(tree.childEnd());

        for (; begin != end; begin++) {
            applyWhiteSpaceImpl(begin, collapsed, allText, convertToPreWrapped);
        }
        if (convertToPreWrapped) {
            tree.childBegin()->properties.setProperty(KoSvgTextProperties::TextCollapseId, QVariant::fromValue(KoSvgText::Preserve));
            tree.childBegin()->properties.setProperty(KoSvgTextProperties::TextWrapId, QVariant::fromValue(KoSvgText::Wrap));
        }
    }

    void applyWhiteSpaceImpl(std::reverse_iterator<KisForest<KoSvgTextContentElement>::child_iterator> current, QVector<bool> &collapsed, QString &allText, const bool convertToPreWrapped) {
        auto base = current.base();
        base--;
        if(base != siblingEnd(base)) {
            auto end = std::make_reverse_iterator(childBegin(base));
            auto begin = std::make_reverse_iterator(childEnd(base));
            for (; begin != end; begin++) {
                applyWhiteSpaceImpl(begin, collapsed, allText, convertToPreWrapped);
            }
        }

        if (!current->text.isEmpty()) {
            const int total = current->text.size();
            QString currentText = allText.right(total);

            for (int i = 0; i < total; i++) {
                const int j = total - (i+1);
                const bool col = collapsed.takeLast();
                if (col) {
                    currentText.remove(j, 1);
                }

            }
            current->text = currentText;
            allText.chop(total);
        }
        if (convertToPreWrapped) {
            current->properties.removeProperty(KoSvgTextProperties::TextCollapseId);
            current->properties.removeProperty(KoSvgTextProperties::TextWrapId);
        }
    }

    static QVector<KoSvgText::CharTransformation> resolvedTransformsForTree(KisForest<KoSvgTextContentElement> &tree, bool shapesInside = false, bool includeBidiControls = false) {
        QString all;
        QVector<bool> collapsed = collapsedWhiteSpacesForText(tree, all, false, includeBidiControls);
        QVector<CharacterResult> result(all.size());
        int globalIndex = 0;
        KoSvgTextProperties props = tree.childBegin()->properties;
        props.inheritFrom(KoSvgTextProperties::defaultProperties(), true);
        KoSvgText::WritingMode mode = KoSvgText::WritingMode(
                    props.propertyOrDefault(KoSvgTextProperties::WritingModeId).toInt());
        bool isHorizontal = mode == KoSvgText::HorizontalTB;
        bool isWrapped = (props.hasProperty(KoSvgTextProperties::InlineSizeId)
                          || shapesInside);
        QVector<KoSvgText::CharTransformation> resolvedTransforms(all.size());
        resolveTransforms(tree.childBegin(), all, result, globalIndex,
                          isHorizontal, isWrapped, false, resolvedTransforms,
                          collapsed, KoSvgTextProperties::defaultProperties(),
                          false);
        return resolvedTransforms;
    }

    static void insertNewLinesAtAnchors(KisForest<KoSvgTextContentElement> &tree, bool shapesInside = false) {
        QVector<KoSvgText::CharTransformation> resolvedTransforms = resolvedTransformsForTree(tree, shapesInside);

        auto end = std::make_reverse_iterator(tree.childBegin());
        auto begin = std::make_reverse_iterator(tree.childEnd());

        bool inTextPath = false;
        for (; begin != end; begin++) {
            insertNewLinesAtAnchorsImpl(begin, resolvedTransforms, inTextPath);
        }
    }

    static void insertNewLinesAtAnchorsImpl(std::reverse_iterator<KisForest<KoSvgTextContentElement>::child_iterator> current,
                                            QVector<KoSvgText::CharTransformation> &resolvedTransforms,
                                            bool &inTextPath) {

        inTextPath = (!current->textPathId.isEmpty());
        auto base = current.base();
        base--;
        if(base != siblingEnd(base)) {
            auto end = std::make_reverse_iterator(childBegin(base));
            auto begin = std::make_reverse_iterator(childEnd(base));
            for (; begin != end; begin++) {
                insertNewLinesAtAnchorsImpl(begin,
                                            resolvedTransforms,
                                            inTextPath);
            }
        }

        if (!current->text.isEmpty()) {
            const int total = current->text.size();

            for (int i = 0; i < total; i++) {
                const int j = total - (i+1);
                KoSvgText::CharTransformation transform = resolvedTransforms.takeLast();
                bool startsNewChunk = transform.startsNewChunk() && j == 0;

                if (inTextPath) {
                    startsNewChunk = false;
                    inTextPath = false;
                }
                if (startsNewChunk && !resolvedTransforms.isEmpty() && current->text.at(j) != QChar::LineFeed) {
                    current->text.insert(j, "\n");
                }
            }
        }
        current->localTransformations.clear();
    }

    void setTransformsFromLayout(KisForest<KoSvgTextContentElement> &tree, const QVector<CharacterResult> layout) {
        for (int i = 0; i< layout.size(); i++) {
            if (layout.at(i).anchored_chunk) {
                int plainTextIndex = layout.at(i).plaintTextIndex;
                splitContentElement(tree, plainTextIndex);
            }
        }

        int globalIndex = 0;
        for (auto it = tree.childBegin(); it != tree.childEnd(); it++) {
            KoSvgText::WritingMode mode = KoSvgText::WritingMode(
                        it->properties.propertyOrDefault(KoSvgTextProperties::WritingModeId).toInt());
            bool isHorizontal = mode == KoSvgText::HorizontalTB;
            setTransformsFromLayoutImpl(it, KoSvgTextProperties::defaultProperties(), layout, globalIndex, isHorizontal);
        }
    }

    void setTransformsFromLayoutImpl(KisForest<KoSvgTextContentElement>::child_iterator current,
                                     const KoSvgTextProperties parentProps, const QVector<CharacterResult> layout,
                                     int &globalIndex, bool isHorizontal) {
        KoSvgTextProperties props = current->properties;
        props.inheritFrom(parentProps);
        if (!current->textPathId.isEmpty()) return;
        for (auto it = childBegin(current); it!= childEnd(current); it++) {
            setTransformsFromLayoutImpl(it, props, layout, globalIndex, isHorizontal);
        }

        if (current->text.isEmpty()) {
            current->localTransformations.clear();
        } else {
            QVector<KoSvgText::CharTransformation> transforms;
            const int length = current->numChars(true, props);

            for (int i = globalIndex; i< globalIndex+length; i++) {
                CharacterResult result = layout.value(i);

                if (!result.addressable) {
                    continue;
                }
                KoSvgText::CharTransformation transform;

                if (result.anchored_chunk) {
                    int endIndex = 0;
                    qreal shift = anchoredChunkShift(layout, isHorizontal, i, endIndex);
                    QPointF offset = isHorizontal? QPointF(shift, 0): QPointF(0, shift);
                    transform.xPos = result.finalPosition.x() - offset.x();
                    transform.yPos = result.finalPosition.y() - offset.y();
                } else if (i > 0) {
                    CharacterResult resultPrev = layout.value(i-1);
                    QPointF offset = (result.finalPosition - result.cssPosition) - (resultPrev.finalPosition - resultPrev.cssPosition);

                    transform.dxPos = offset.x();
                    transform.dyPos = offset.y();
                }
                transform.rotate = result.rotate;

                transforms.append(transform);
            }
            current->localTransformations = transforms;
            current->text = current->text.split("\n").join(" ");
            globalIndex += length;
        }
    }

    static bool cleanUpImpl(KisForest<KoSvgTextContentElement> &tree, KisForest<KoSvgTextContentElement>::child_iterator current) {
        const int children = childCount(current);
        if (children == 0) {
            const int length = current->numChars(false);
            if (length == 0 && current != tree.childBegin() && current->textPathId.isEmpty()) {
                return true;
            } else {
                auto siblingPrev = current;
                KoSvgText::UnicodeBidi bidi = KoSvgText::UnicodeBidi(current->properties.property(KoSvgTextProperties::UnicodeBidiId,
                                                                                             QVariant(KoSvgText::BidiNormal)).toInt());
                siblingPrev--;
                while (!isEnd(siblingPrev) && siblingPrev->text.isEmpty()) {
                    siblingPrev--;
                }
                if (!isEnd(siblingPrev)
                        && siblingPrev != current
                        && (siblingPrev->localTransformations.isEmpty() && current->localTransformations.isEmpty())
                        && (siblingPrev->textPathId.isEmpty() && current->textPathId.isEmpty())
                        && (siblingPrev->textLength.isAuto && current->textLength.isAuto)
                        && (siblingPrev->properties == current->properties)
                        && (bidi != KoSvgText::BidiIsolate && bidi != KoSvgText::BidiIsolateOverride)
                        && childCount(siblingPrev) == 0) {
                    siblingPrev->text += current->text;
                    current->text = QString();
                    return true;
                }
            }
        } else if (children == 1) {
            auto child = childBegin(current);
            if ((child->localTransformations.isEmpty() && current->localTransformations.isEmpty())
                    && (child->textPathId.isEmpty() && current->textPathId.isEmpty())
                    && (child->textLength.isAuto && current->textLength.isAuto)
                    && (!child->properties.hasNonInheritableProperties() || !current->properties.hasNonInheritableProperties())) {
                if (current->properties.hasNonInheritableProperties()) {
                    KoSvgTextProperties props = current->properties;
                    props.setAllButNonInheritableProperties(child->properties);
                    child->properties = props;
                } else {
                    child->properties.inheritFrom(current->properties);
                }
                *current = *child;
                tree.erase(child);
            }
        } else {
            QVector<KisForest<KoSvgTextContentElement>::child_iterator> deleteList;
            for (auto child = childBegin(current); child != childEnd(current); child++) {
                if (cleanUpImpl(tree, child)) {
                    deleteList.append(child);
                }
            }
            while (!deleteList.isEmpty()) {
                auto child = deleteList.takeFirst();
                KIS_ASSERT_RECOVER_NOOP(childBegin(child) == childEnd(child));
                tree.erase(child);
            }
            if (childCount(current) <= 1) {
                return cleanUpImpl(tree, current);
            }
        }
        return false;
    }

    static void cleanUp(KisForest<KoSvgTextContentElement> &tree) {
        if (tree.childBegin() != tree.childEnd()) {
            cleanUpImpl(tree, tree.childBegin());
        }
    }

    static KisForest<KoSvgTextContentElement>::child_iterator iteratorForTreeIndex(const QVector<int> treeIndex, KisForest<KoSvgTextContentElement>::child_iterator parent) {
        if (treeIndex.isEmpty()) return parent;
        QVector<int> idx = treeIndex;
        int count = idx.takeFirst();
        for (auto child = KisForestDetail::childBegin(parent); child != KisForestDetail::childEnd(parent); child++) {
            if (count == 0) {
                if ((KisForestDetail::childBegin(child) != KisForestDetail::childEnd(child))) {
                    return iteratorForTreeIndex(idx, child);
                } else {
                    return child;
                }
            }
            count -= 1;
        }
        return KisForestDetail::childEnd(parent);
    }

    static bool startIndexOfIterator(KisForest<KoSvgTextContentElement>::child_iterator parent, KisForest<KoSvgTextContentElement>::child_iterator target, int &currentIndex) {
        for (auto child = KisForestDetail::childBegin(parent); child != KisForestDetail::childEnd(parent); child++) {
            if (child == target) {
                return true;
            } else if ((KisForestDetail::childBegin(child) != KisForestDetail::childEnd(child))) {
                if (startIndexOfIterator(child, target, currentIndex)) {
                    return true;
                }
            } else {
                currentIndex += numChars(child);
            }
        }
        return false;
    }

    KoSvgTextNodeIndex createTextNodeIndex(KisForest<KoSvgTextContentElement>::child_iterator textElement) const;

    static void removeTextPathId(KisForest<KoSvgTextContentElement>::child_iterator parent, const QString &name) {
        for (auto it = childBegin(parent); it != childEnd(parent); it++) {
            if (it->textPathId == name) {
                it->textPathId = QString();
                break;
            }
        }
    }

    static void makeTextPathNameUnique(QList<KoShape*> textPaths, KoShape *textPath) {
        bool textPathNameUnique = false;
        int textPathNumber = textPaths.size();
        QString newTextPathName = textPath->name();
        while(!textPathNameUnique) {
            textPathNameUnique = true;
            Q_FOREACH(KoShape *shape, textPaths) {
                if (shape->name() == newTextPathName) {
                    textPathNameUnique = false;
                    textPathNumber += 1;
                    break;
                }
            }
            if (textPathNameUnique && !newTextPathName.isEmpty()) {
                textPath->setName(newTextPathName);
            } else {
                textPathNameUnique = false;
            }
            newTextPathName = QString("textPath"+QString::number(textPathNumber));
        }
    }

    static QRectF boundingBoxFromTree(KisForest<KoSvgTextContentElement> tree, const KoSvgTextShape *rootShape, bool includeStrokeInset) {
        QRectF result;
        QList<KoShapeStrokeModelSP> parentStrokes;
        for (auto it = tree.compositionBegin(); it != tree.compositionEnd(); it++) {
            if (it.state() == KisForestDetail::Enter) {
                if (it->properties.hasProperty(KoSvgTextProperties::StrokeId)) {
                    parentStrokes.append(it->properties.property(KoSvgTextProperties::StrokeId).value<KoSvgText::StrokeProperty>().property);
                }
            } else {
                KoShapeStrokeModelSP stroke = parentStrokes.size() > 0? parentStrokes.last(): nullptr;
                QRectF bb = it->associatedOutline.boundingRect();
                QMap<KoSvgText::TextDecoration, QPainterPath> decorations = it->textDecorations;
                for (int i = 0; i < decorations.values().size(); ++i) {
                    bb |= decorations.values().at(i).boundingRect();
                }
                if (!bb.isEmpty()) {
                    if (stroke && includeStrokeInset) {
                        KoInsets insets;
                        stroke->strokeInsets(rootShape, insets);
                        result |= bb.adjusted(-insets.left, -insets.top, insets.right, insets.bottom);
                    } else {
                        result |= bb;
                    }
                }
                if (it->properties.hasProperty(KoSvgTextProperties::StrokeId)) {
                    parentStrokes.pop_back();
                }
            }
        }
        return result;
    }
};

#endif
