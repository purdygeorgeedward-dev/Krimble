/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2022 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "KoSvgTextShape.h"
#include "KoSvgTextShape_p.h"

#include "KisTofuGlyph.h"
#include "KoFontLibraryResourceUtils.h"

#include <FlakeDebug.h>
#include <KoPathShape.h>

#include <kis_global.h>

#include <QPainterPath>
#include <QtMath>

#include <utility>
#include <variant>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_COLOR_H
#include FT_BITMAP_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_TABLES_H

#include <hb.h>
#include <hb-ft.h>

#include <raqm.h>

static QPainterPath convertFromFreeTypeOutline(FT_GlyphSlotRec *glyphSlot);
static QImage convertFromFreeTypeBitmap(FT_GlyphSlotRec *glyphSlot);

static QString glyphFormatToStr(const FT_Glyph_Format _v)
{
    const unsigned int v = _v;
    QString s;
    s += QChar((v >> 24) & 0xFF);
    s += QChar((v >> 16) & 0xFF);
    s += QChar((v >> 8) & 0xFF);
    s += QChar((v >> 0) & 0xFF);
    return s;
}

static void
emboldenGlyphIfNeeded(const FT_Face ftface, const CharacterResult &charResult, int *x_advance, int *y_advance)
{
    constexpr int WEIGHT_SEMIBOLD = 600;
    if (charResult.fontWeight >= WEIGHT_SEMIBOLD) {
        if (ftface->style_flags & FT_STYLE_FLAG_BOLD) {
            return;
        }

        if (FT_HAS_MULTIPLE_MASTERS(ftface)) {
            return;
        }

        hb_font_t_sp hbFont(hb_ft_font_create_referenced(ftface));
        if (hb_style_get_value(hbFont.data(), HB_STYLE_TAG_WEIGHT) >= WEIGHT_SEMIBOLD) {
            return;
        }

        FT_Pos strength =
            FT_MulFix(ftface->units_per_EM, ftface->size->metrics.y_scale) / 48;

        if (ftface->glyph->format == FT_GLYPH_FORMAT_BITMAP) {

            strength &= ~63;
            if (strength == 0) {
                strength = 64;
            }

            FT_GlyphSlot_Own_Bitmap(ftface->glyph);

            const FT_Pos strengthY = strength - 64;
            FT_Bitmap_Embolden(ftface->glyph->library, &ftface->glyph->bitmap, strength, strengthY);

            if (x_advance && *x_advance != 0) {
                *x_advance += strength;
            }
            if (y_advance && *y_advance != 0) {
                *y_advance -= strengthY;
            }
        } else {
            FT_Outline_Embolden(&ftface->glyph->outline, strength);

            if (x_advance && *x_advance != 0) {
                *x_advance += strength;
            }
            if (y_advance && *y_advance != 0) {
                *y_advance -= strength;
            }
        }
    }
}

bool faceIsItalic(const FT_Face face) {
    hb_font_t_sp hbFont(hb_ft_font_create_referenced(face));
    bool isItalic = hb_style_get_value(hbFont.data(), HB_STYLE_TAG_ITALIC) > 0;
    bool isOblique = false;

    if (FT_HAS_MULTIPLE_MASTERS(face)) {
        isOblique = hb_style_get_value(hbFont.data(), HB_STYLE_TAG_SLANT_ANGLE) != 0;
    }

    return isItalic || isOblique;
}

static std::pair<QTransform, QTransform> calcOutlineGlyphTransform(const QTransform &ftTF,
                                                                   const raqm_glyph_t &currentGlyph,
                                                                   const CharacterResult &charResult,
                                                                   const bool isHorizontal)
{
    QTransform outlineGlyphTf = QTransform::fromTranslate(currentGlyph.x_offset, currentGlyph.y_offset);
    QTransform glyphObliqueTf;

    if (charResult.fontStyle != QFont::StyleNormal && !faceIsItalic(currentGlyph.ftface)) {
        constexpr double SLANT_14DEG = 0.24932800284318069162403993780486;
        if (isHorizontal) {
            glyphObliqueTf.shear(SLANT_14DEG, 0);
        } else {
            glyphObliqueTf.shear(0, -SLANT_14DEG);
        }
        outlineGlyphTf *= glyphObliqueTf;
    }
    outlineGlyphTf *= ftTF;
    return {outlineGlyphTf, glyphObliqueTf};
}

class ColorLayersLoader
{
public:
    ColorLayersLoader(FT_Face face, FT_UInt baseGlyph)
        : m_face(face)
        , m_baseGlyph(baseGlyph)
    {
        const unsigned short paletteIndex = 0;
        if (FT_Palette_Select(m_face, paletteIndex, &m_palette) != 0) {
            m_palette = nullptr;
        }
        m_haveLayers = moveNext();
    }

    operator bool() const
    {
        return m_haveLayers && m_palette;
    }

    std::tuple<QPainterPath, QBrush, bool>
    layer(const CharacterResult &charResult, const FT_Int32 faceLoadFlags, int *x_advance, int *y_advance)
    {
        QBrush layerColor;
        bool isForeGroundColor = false;

        if (m_layerColorIndex == 0xFFFF) {
            layerColor = Qt::black;
            isForeGroundColor = true;
        } else {
            const FT_Color color = m_palette[m_layerColorIndex];
            layerColor = QColor(color.red, color.green, color.blue, color.alpha);
        }
        if (const FT_Error err = FT_Load_Glyph(m_face, m_layerGlyphIndex, faceLoadFlags)) {
            warnFlake << "Failed to load glyph, freetype error" << err;
            return {};
        }
        if (m_face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
            emboldenGlyphIfNeeded(m_face, charResult, x_advance, y_advance);

            const QPainterPath p = convertFromFreeTypeOutline(m_face->glyph);
            return {p, layerColor, isForeGroundColor};
        } else {
            warnFlake << "Unsupported glyph format" << glyphFormatToStr(m_face->glyph->format) << "in glyph layers";
            return {};
        }
    }

    bool moveNext()
    {
        m_haveLayers =
            FT_Get_Color_Glyph_Layer(m_face, m_baseGlyph, &m_layerGlyphIndex, &m_layerColorIndex, &m_iterator);
        return m_haveLayers;
    }

private:
    FT_UInt m_layerGlyphIndex{};
    FT_UInt m_layerColorIndex{};
    FT_LayerIterator m_iterator{};
    FT_Color *m_palette{};
    FT_Face m_face;
    FT_UInt m_baseGlyph;
    bool m_haveLayers;
};

std::pair<QTransform, qreal> KoSvgTextShape::Private::loadGlyphOnly(const QTransform &ftTF,
                                                                    const FT_Int32 faceLoadFlags,
                                                                    const bool isHorizontal,
                                                                    raqm_glyph_t &currentGlyph,
                                                                    CharacterResult &charResult,
                                                                    const KoSvgText::TextRendering rendering)
{
    QTransform glyphObliqueTf;

    qreal bitmapScale = 1.0;

    if (currentGlyph.index == 0) {
        debugFlake << "Missing glyph";
        return {glyphObliqueTf, bitmapScale};
    }

    if (ColorLayersLoader loader{currentGlyph.ftface, currentGlyph.index}) {
        Glyph::ColorLayers *colorGlyph = std::get_if<Glyph::ColorLayers>(&charResult.glyph);
        if (!colorGlyph) {
            if (!std::holds_alternative<std::monostate>(charResult.glyph)) {
                warnFlake << "Glyph contains other type than ColorLayers:" << charResult.glyph.index();
            }
            colorGlyph = &charResult.glyph.emplace<Glyph::ColorLayers>();
        }

        QTransform outlineGlyphTf;

        std::tie(outlineGlyphTf, glyphObliqueTf) =
            calcOutlineGlyphTransform(ftTF, currentGlyph, charResult, isHorizontal);

        const int orig_x_advance = currentGlyph.x_advance;
        const int orig_y_advance = currentGlyph.y_advance;
        int new_x_advance{};
        int new_y_advance{};
        do {
            new_x_advance = orig_x_advance;
            new_y_advance = orig_y_advance;
            QPainterPath p;
            QBrush layerColor;
            bool isForeGroundColor = false;
            std::tie(p, layerColor, isForeGroundColor) = loader.layer(charResult, faceLoadFlags, &new_x_advance, &new_y_advance);
            if (!p.isEmpty()) {
                p = outlineGlyphTf.map(p);
                if (charResult.visualIndex > -1) {
                    p = p.translated(charResult.advance);
                }
                colorGlyph->paths.append(p);
                colorGlyph->colors.append(layerColor);
                colorGlyph->replaceWithForeGroundColor.append(isForeGroundColor);
            }
        } while (loader.moveNext());
        currentGlyph.x_advance = new_x_advance;
        currentGlyph.y_advance = new_y_advance;
    } else {
        if (const FT_Error err = FT_Load_Glyph(currentGlyph.ftface, currentGlyph.index, faceLoadFlags)) {
            warnFlake << "Failed to load glyph, freetype error" << err;
            return {glyphObliqueTf, bitmapScale};
        }

        emboldenGlyphIfNeeded(currentGlyph.ftface, charResult, &currentGlyph.x_advance, &currentGlyph.y_advance);

        if (currentGlyph.ftface->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
            Glyph::Outline _discard;
            Glyph::Outline *outlineGlyph = std::get_if<Glyph::Outline>(&charResult.glyph);
            if (!outlineGlyph) {
                if (std::holds_alternative<Glyph::ColorLayers>(charResult.glyph)) {
                    outlineGlyph = &_discard;
                } else if (!std::holds_alternative<std::monostate>(charResult.glyph)) {
                    warnFlake << "Glyph contains other type than Outline:" << charResult.glyph.index();
                }
                if (!outlineGlyph) {
                    outlineGlyph = &charResult.glyph.emplace<Glyph::Outline>();
                }
            }

            QTransform outlineGlyphTf;

            std::tie(outlineGlyphTf, glyphObliqueTf) =
                calcOutlineGlyphTransform(ftTF, currentGlyph, charResult, isHorizontal);

            QPainterPath glyph = convertFromFreeTypeOutline(currentGlyph.ftface->glyph);
            glyph = outlineGlyphTf.map(glyph);

            if (charResult.visualIndex > -1) {
                outlineGlyph->path.addPath(glyph.translated(charResult.advance));
            } else {
                outlineGlyph->path = glyph;
            }
        } else {
            QTransform bitmapTf;

            if (currentGlyph.ftface->glyph->format == FT_GLYPH_FORMAT_BITMAP) {
                if (FT_HAS_COLOR(currentGlyph.ftface)) {
                    FT_Matrix matrix;
                    FT_Vector delta;
                    FT_Get_Transform(currentGlyph.ftface, &matrix, &delta);
                    constexpr qreal FACTOR_16 = 1.0 / 65536.0;
                    bitmapTf.setMatrix(matrix.xx * FACTOR_16, matrix.xy * FACTOR_16, 0, matrix.yx * FACTOR_16, matrix.yy * FACTOR_16, 0, 0, 0, 1);
                    KIS_SAFE_ASSERT_RECOVER_NOOP(bitmapTf.m11() == bitmapTf.m22());
                    bitmapScale = bitmapTf.m11();
                    QPointF anchor(-currentGlyph.ftface->glyph->bitmap_left, currentGlyph.ftface->glyph->bitmap_top);
                    bitmapTf = QTransform::fromTranslate(-anchor.x(), -anchor.y()) * bitmapTf
                        * QTransform::fromTranslate(anchor.x(), anchor.y());
                }
            } else if (currentGlyph.ftface->glyph->format == FT_GLYPH_FORMAT_SVG) {
                debugFlake << "Unsupported glyph format" << glyphFormatToStr(currentGlyph.ftface->glyph->format);
                return {glyphObliqueTf, bitmapScale};
            } else {
                debugFlake << "Unsupported glyph format" << glyphFormatToStr(currentGlyph.ftface->glyph->format)
                           << "asking freetype to render it for us";
                FT_Render_Mode mode = FT_LOAD_TARGET_MODE(faceLoadFlags);
                if (mode == FT_RENDER_MODE_NORMAL && (faceLoadFlags & FT_LOAD_MONOCHROME)) {
                    mode = FT_RENDER_MODE_MONO;
                }
                if (const FT_Error err = FT_Render_Glyph(currentGlyph.ftface->glyph, mode)) {
                    warnFlake << "Failed to render glyph, freetype error" << err;
                    return {glyphObliqueTf, bitmapScale};
                }
            }

            Glyph::Bitmap *bitmapGlyph = std::get_if<Glyph::Bitmap>(&charResult.glyph);
            if (!bitmapGlyph) {
                if (!std::holds_alternative<std::monostate>(charResult.glyph)) {
                    warnFlake << "Glyph contains other type than Bitmap:" << charResult.glyph.index();
                }
                bitmapGlyph = &charResult.glyph.emplace<Glyph::Bitmap>();
            }

            QImage image = convertFromFreeTypeBitmap(currentGlyph.ftface->glyph);
            bitmapGlyph->images.append(image);

            if (charResult.fontStyle != QFont::StyleNormal
                && !(currentGlyph.ftface->style_flags & FT_STYLE_FLAG_ITALIC)) {
                constexpr double SLANT_BITMAP = 0.25;
                QTransform shearTf;
                QPoint shearAt;
                if (isHorizontal) {
                    shearTf.shear(-SLANT_BITMAP, 0);
                    glyphObliqueTf.shear(SLANT_BITMAP, 0);
                    shearAt = QPoint(0, currentGlyph.ftface->glyph->bitmap_top);
                } else {
                    shearTf.shear(0, SLANT_BITMAP);
                    glyphObliqueTf.shear(0, -SLANT_BITMAP);
                    shearAt = QPoint(image.width() / 2, 0);
                }
                bitmapTf = (QTransform::fromTranslate(-shearAt.x(), -shearAt.y()) * shearTf
                    * QTransform::fromTranslate(shearAt.x(), shearAt.y())) * bitmapTf;
            }

            if (!bitmapTf.isIdentity()) {
                const QSize srcSize = image.size();
                bitmapGlyph->images.replace(bitmapGlyph->images.size()-1, std::move(image).transformed(
                    bitmapTf,
                    rendering == KoSvgText::RenderingOptimizeSpeed ? Qt::FastTransformation : Qt::SmoothTransformation));

                const QPoint offset = bitmapTf.mapRect(QRectF({0, 0}, srcSize)).toAlignedRect().topLeft();
                currentGlyph.ftface->glyph->bitmap_left += offset.x();
                currentGlyph.ftface->glyph->bitmap_top -= offset.y();
            }
        }
    }
    return {glyphObliqueTf, bitmapScale};
}

bool KoSvgTextShape::Private::loadGlyph(const KoSvgText::ResolutionHandler &resHandler,
                                        const FT_Int32 faceLoadFlags,
                                        const bool isHorizontal,
                                        const char32_t firstCodepoint,
                                        const KoSvgText::TextRendering rendering,
                                        raqm_glyph_t &currentGlyph,
                                        CharacterResult &charResult,
                                        QPointF &totalAdvanceFTFontCoordinates)
{
    const QTransform ftTF = resHandler.freeTypeToPointTransform();

    QTransform glyphObliqueTf;

    qreal bitmapScale = 1.0;

    std::tie(glyphObliqueTf, bitmapScale) = loadGlyphOnly(ftTF, faceLoadFlags, isHorizontal, currentGlyph, charResult, rendering);

    if (charResult.visualIndex == -1) {
        CursorInfo cursorInfo = charResult.cursorInfo;
        qreal lineHeight = (charResult.metrics.ascender-charResult.metrics.descender) * bitmapScale;
        qreal descender = charResult.metrics.descender * bitmapScale;
        qint32 offset = charResult.metrics.caretOffset;
        if (isHorizontal) {
            qreal slope = 0;
            if (charResult.metrics.caretRun != 0 && charResult.metrics.caretRise !=0) {
                slope = double(charResult.metrics.caretRun)/double(charResult.metrics.caretRise);
                if (offset == 0) {
                    offset = descender * slope;
                }
            }
            QLineF caret(QPointF(), QPointF(lineHeight*slope, lineHeight));
            caret.translate(-QPointF(-offset, -descender));
            cursorInfo.caret = ftTF.map(glyphObliqueTf.map(caret));

        } else {
            qreal slope = 0;
            if (charResult.metrics.caretRun != 0 && charResult.metrics.caretRise !=0) {
                slope = double(charResult.metrics.caretRise)/double(charResult.metrics.caretRun);
                if (offset == 0) {
                    offset = descender * slope;
                }
            }
            QLineF caret(QPointF(), QPointF(lineHeight, lineHeight*slope));
            caret.translate(-QPointF(-descender, -offset));
            cursorInfo.caret = ftTF.map(glyphObliqueTf.map(caret));
        }

        charResult.cursorInfo = cursorInfo;
    }

    {
        QPointF advance(currentGlyph.x_advance, currentGlyph.y_advance);
        if (charResult.tabSize) {
            charResult.glyph.emplace<Glyph::Outline>();
        }

        if (std::holds_alternative<std::monostate>(charResult.glyph)) {
            const auto height = ftTF.map(QPointF(currentGlyph.ftface->size->metrics.height, 0)).x() * 0.6;
            QPainterPath glyph = KisTofuGlyph::create(firstCodepoint, height);
            if (isHorizontal) {
                glyph.translate(0, -height);
                const qreal newAdvance =
                    ftTF.inverted().map(QPointF(glyph.boundingRect().width() + height * (1. / 15.), 0)).x();
                if (newAdvance > advance.x()) {
                    advance.setX(newAdvance);
                }
            } else {
                glyph.translate(glyph.boundingRect().width() * -0.5, (ftTF.map(advance).y() - height) * 0.5);
            }
            charResult.glyph.emplace<Glyph::Outline>(Glyph::Outline{glyph});
        }

        charResult.advance += ftTF.map(advance);

        Glyph::Bitmap *const bitmapGlyph = std::get_if<Glyph::Bitmap>(&charResult.glyph);

        if (bitmapGlyph) {
            const int width = bitmapGlyph->images.last().width();
            const int height = bitmapGlyph->images.last().height();
            const int left = currentGlyph.ftface->glyph->bitmap_left;
            const int top = currentGlyph.ftface->glyph->bitmap_top - height;
            QRect bboxPixel(left, top, width, height);
            if (!isHorizontal) {
                bboxPixel.moveLeft(-(bboxPixel.width() / 2));
                bboxPixel.moveTop(-bboxPixel.height());
            }
            QRectF drawRect = ftTF.mapRect(QRectF(bboxPixel.topLeft() * resHandler.freeTypePixel, bboxPixel.size() * resHandler.freeTypePixel));
            drawRect.translate(charResult.advance - ftTF.map(advance));
            bitmapGlyph->drawRects.append(drawRect);
        }

        QRectF bbox;
        if (isHorizontal) {
            bbox = QRectF(0,
                          charResult.metrics.descender * bitmapScale,
                          ftTF.inverted().map(charResult.advance).x(),
                          (charResult.metrics.ascender - charResult.metrics.descender) * bitmapScale);

        } else {
            bbox = QRectF(charResult.metrics.descender * bitmapScale,
                          0,
                          (charResult.metrics.ascender - charResult.metrics.descender) * bitmapScale,
                          ftTF.inverted().map(charResult.advance).y());
        }
        bbox = glyphObliqueTf.mapRect(bbox);
        charResult.isHorizontal = isHorizontal;
        QRectF scaledBBox = resHandler.adjust(ftTF.mapRect(bbox));
        charResult.scaledHalfLeading = resHandler.adjust(ftTF.map(QPointF(charResult.fontHalfLeading, charResult.fontHalfLeading))).x();
        charResult.scaledAscent = isHorizontal? scaledBBox.top(): scaledBBox.right();
        charResult.scaledDescent = isHorizontal? scaledBBox.bottom(): scaledBBox.left();

        if (charResult.tabSize) {
            charResult.tabSize = ftTF.map(QPointF(*charResult.tabSize, *charResult.tabSize)).x();
        }

        if(!qFuzzyCompare(bitmapScale, 1.0)) {
            charResult.metrics.scaleBaselines(bitmapScale);
        }
        if (charResult.extraFontScaling < 1.0) {
            charResult.scaleCharacterResult(charResult.extraFontScaling, charResult.extraFontScaling);
        }

        if (bitmapGlyph) {
            charResult.inkBoundingBox |= bitmapGlyph->drawRects.last();
        } else if (const auto *outlineGlyph = std::get_if<Glyph::Outline>(&charResult.glyph)) {
            charResult.inkBoundingBox |= outlineGlyph->path.boundingRect();
        } else if (const auto *colorGlyph = std::get_if<Glyph::ColorLayers>(&charResult.glyph)) {
            Q_FOREACH (const QPainterPath &p, colorGlyph->paths) {
                charResult.inkBoundingBox |= p.boundingRect();
            }
        } else if (!std::holds_alternative<std::monostate>(charResult.glyph)) {
            warnFlake << "Unhandled glyph type" << charResult.glyph.index();
        }
        totalAdvanceFTFontCoordinates += advance;
        charResult.cssPosition = ftTF.map(totalAdvanceFTFontCoordinates) - charResult.advance;
    }
    return true;
}

QVector<QPointF> KoSvgTextShape::Private::getLigatureCarets(const KoSvgText::ResolutionHandler &resHandler, const bool isHorizontal, raqm_glyph_t &currentGlyph)
{
    QVector<QPointF> positions;
    const QTransform ftTF = resHandler.freeTypeToPointTransform();
    hb_direction_t dir = isHorizontal? HB_DIRECTION_LTR: HB_DIRECTION_TTB;
    hb_font_t_sp font(hb_ft_font_create_referenced(currentGlyph.ftface));

    uint numCarets = hb_ot_layout_get_ligature_carets(font.data(), dir, currentGlyph.index, 0, nullptr, nullptr);
    for (uint i=0; i<numCarets; i++) {
        hb_position_t caretPos = 0;
        uint caretCount = 1;
        hb_ot_layout_get_ligature_carets(font.data(), dir, currentGlyph.index, 0, &caretCount, &caretPos);
        QPointF pos = isHorizontal? QPointF(caretPos, 0): QPointF(0, caretPos);
        positions.append(ftTF.map(pos));
    }
    return positions;
}

static QPainterPath convertFromFreeTypeOutline(FT_GlyphSlotRec *glyphSlot)
{
    QPointF cp = QPointF();
    QPainterPath glyph;
    glyph.setFillRule(Qt::WindingFill);
    int i = 0;
    for (int j = 0; j < glyphSlot->outline.n_contours; ++j) {
        int last_point = glyphSlot->outline.contours[j];
        QPointF start = QPointF(glyphSlot->outline.points[i].x, glyphSlot->outline.points[i].y);
        if (!(glyphSlot->outline.tags[i] & 1)) {
            if (!(glyphSlot->outline.tags[last_point] & 1)) {
                start = (QPointF(glyphSlot->outline.points[last_point].x, glyphSlot->outline.points[last_point].y) + start) / 2.0;
            } else {
                start = QPointF(glyphSlot->outline.points[last_point].x, glyphSlot->outline.points[last_point].y);
            }
            --i;
        }
        start += cp;
        glyph.moveTo(start);
        std::array<QPointF, 4> curve;
        curve[0] = start;
        size_t n = 1;
        while (i < last_point) {
            ++i;
            curve.at(n) = cp + QPointF(glyphSlot->outline.points[i].x, glyphSlot->outline.points[i].y);
            ++n;
            switch (glyphSlot->outline.tags[i] & 3) {
            case 2:
                if (n < 4)
                    continue;
                curve[3] = (curve[3] + curve[2]) / 2;
                --i;
                break;
            case 0:
                if (n < 3)
                    continue;
                curve[3] = (curve[1] + curve[2]) / 2;
                curve[2] = (2 * curve[1] + curve[3]) / 3;
                curve[1] = (2 * curve[1] + curve[0]) / 3;
                --i;
                break;
            case 1:
            case 3:
                if (n == 2) {
                    glyph.lineTo(curve[1]);
                    curve[0] = curve[1];
                    n = 1;
                    continue;
                } else if (n == 3) {
                    curve[3] = curve[2];
                    curve[2] = (2 * curve[1] + curve[3]) / 3;
                    curve[1] = (2 * curve[1] + curve[0]) / 3;
                }
                break;
            }
            glyph.cubicTo(curve[1], curve[2], curve[3]);
            curve[0] = curve[3];
            n = 1;
        }
        if (n == 1) {
            glyph.closeSubpath();
        } else {
            curve[3] = start;
            if (n == 2) {
                curve[2] = (2 * curve[1] + curve[3]) / 3;
                curve[1] = (2 * curve[1] + curve[0]) / 3;
            }
            glyph.cubicTo(curve[1], curve[2], curve[3]);
        }
        ++i;
    }
    return glyph;
}

static QImage convertFromFreeTypeBitmap(FT_GlyphSlotRec *glyphSlot)
{
    KIS_ASSERT(glyphSlot->bitmap.width <= INT32_MAX);
    KIS_ASSERT(glyphSlot->bitmap.rows <= INT32_MAX);
    QImage img;
    const int height = static_cast<int>(glyphSlot->bitmap.rows);
    const QSize size(static_cast<int>(glyphSlot->bitmap.width), height);

    if (glyphSlot->bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
        img = QImage(size, QImage::Format_Mono);
        uchar *src = glyphSlot->bitmap.buffer;
        KIS_ASSERT(glyphSlot->bitmap.pitch >= 0);
        for (int y = 0; y < height; y++) {
            memcpy(img.scanLine(y), src, static_cast<size_t>(glyphSlot->bitmap.pitch));
            src += glyphSlot->bitmap.pitch;
        }
    } else if (glyphSlot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY) {
        img = QImage(size, QImage::Format_Grayscale8);
        uchar *src = glyphSlot->bitmap.buffer;
        KIS_ASSERT(glyphSlot->bitmap.pitch >= 0);
        for (int y = 0; y < height; y++) {
            memcpy(img.scanLine(y), src, static_cast<size_t>(glyphSlot->bitmap.pitch));
            src += glyphSlot->bitmap.pitch;
        }
    } else if (glyphSlot->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA) {
        img = QImage(size, QImage::Format_ARGB32_Premultiplied);
        const uint8_t *src = glyphSlot->bitmap.buffer;
        for (int y = 0; y < height; y++) {
            auto *argb = reinterpret_cast<QRgb *>(img.scanLine(y));
            for (unsigned int x = 0; x < glyphSlot->bitmap.width; x++) {
                argb[x] = qRgba(src[2], src[1], src[0], src[3]);
                src += 4;
            }
        }
    }

    return img;
}
