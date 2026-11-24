/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOSVGTEXTCONTENTELEMENT_H
#define KOSVGTEXTCONTENTELEMENT_H

#include "KoSvgText.h"
#include "KoSvgTextProperties.h"
#include "SvgLoadingContext.h"
#include "SvgSavingContext.h"

#include <kritaflake_export.h>
#include <QScopedPointer>

struct KRITAFLAKE_EXPORT KoSvgTextContentElement
{
public:
    KoSvgTextContentElement();
    KoSvgTextContentElement(const KoSvgTextContentElement &rhs)
        : properties(rhs.properties)
        , localTransformations(rhs.localTransformations)
        , textPathInfo(rhs.textPathInfo)
        , textPathId(rhs.textPathId)
        , textLength(rhs.textLength)
        , lengthAdjust(rhs.lengthAdjust)
        , textDecorations(rhs.textDecorations)
        , text(rhs.text)
        , finalResultIndex(rhs.finalResultIndex)
        , associatedOutline(rhs.associatedOutline)
    {

    }

    ~KoSvgTextContentElement() = default;

    KoSvgTextProperties properties;

    QVector<KoSvgText::CharTransformation> localTransformations;

    KoSvgText::TextOnPathInfo textPathInfo;

    QString textPathId;

    KoSvgText::AutoValue textLength;
    KoSvgText::LengthAdjust lengthAdjust = KoSvgText::LengthAdjustSpacing;

    QMap<KoSvgText::TextDecoration, QPainterPath> textDecorations;

    QString text;

    int finalResultIndex = -1;

    QPainterPath associatedOutline;

    bool loadSvg(const QDomElement &element, SvgLoadingContext &context, bool rootNode = false);

    bool loadSvgTextNode(const QDomText &text, SvgLoadingContext &context);

    bool saveSvg(SvgSavingContext &context, bool rootText, bool saveText, QMap<QString, QString> shapeSpecificAttributes, KoShape *textPath = nullptr);

    int numChars(bool withControls = false, KoSvgTextProperties resolvedProps = KoSvgTextProperties()) const;

    QString getTransformedString(QVector<QPair<int, int>> &positions, KoSvgTextProperties resolvedProps = KoSvgTextProperties()) const;

    void insertText(int start, QString insertText);
    void removeText(int &start, int length);
};

#endif
