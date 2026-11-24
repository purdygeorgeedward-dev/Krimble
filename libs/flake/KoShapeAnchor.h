/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 * SPDX-FileCopyrightText: 2013 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOSHAPEANCHOR_H
#define KOSHAPEANCHOR_H

#include "kritaflake_export.h"

class KoShape;
#include <QDomDocument>
class KoShapeLoadingContext;
class KoShapeSavingContext;
class KoShapeAnchorPrivate;

class QTextDocument;
class QPointF;
class QString;

class KRITAFLAKE_EXPORT KoShapeAnchor
{
public:
    class PlacementStrategy {
    public:
        PlacementStrategy(){};
        virtual ~PlacementStrategy(){};

        virtual void detachFromModel() = 0;

        virtual void updateContainerModel() = 0;
    };

    class TextLocation {
    public:
        TextLocation(){};
        virtual ~TextLocation(){};
        virtual const QTextDocument *document() const = 0;
        virtual int position() const = 0;
    };

    enum HorizontalPos {
        HCenter,
        HFromInside,
        HFromLeft,
        HInside,
        HLeft,
        HOutside,
        HRight
    };

    enum HorizontalRel {
        HChar,
        HPage,
        HPageContent,
        HPageStartMargin,
        HPageEndMargin,
        HFrame,
        HFrameContent,
        HFrameEndMargin,
        HFrameStartMargin,
        HParagraph,
        HParagraphContent,
        HParagraphEndMargin,
        HParagraphStartMargin
    };

    enum VerticalPos {
        VBelow,
        VBottom,
        VFromTop,
        VMiddle,
        VTop
    };

    enum VerticalRel {
        VBaseline,
        VChar,
        VFrame,
        VFrameContent,
        VLine,
        VPage,
        VPageContent,
        VParagraph,
        VParagraphContent,
        VText
    };

    enum AnchorType {
        AnchorAsCharacter,
        AnchorToCharacter,
        AnchorParagraph,
        AnchorPage
    };

    explicit KoShapeAnchor(KoShape *shape);
    virtual ~KoShapeAnchor();

    KoShape *shape() const;

    AnchorType anchorType() const;

    void setAnchorType(AnchorType type);

    void setHorizontalPos(HorizontalPos);

    HorizontalPos horizontalPos() const;

    void setHorizontalRel(HorizontalRel);

    HorizontalRel horizontalRel() const;

    void setVerticalPos(VerticalPos);

    VerticalPos verticalPos() const;

    void setVerticalRel(VerticalRel);

    VerticalRel verticalRel() const;

    QString wrapInfluenceOnPosition() const;

    bool flowWithText() const;

    int pageNumber() const;

    const QPointF &offset() const;

    void setOffset(const QPointF &offset);

    TextLocation *textLocation() const;

    void setTextLocation(TextLocation *textLocation);

    PlacementStrategy *placementStrategy() const;

    void setPlacementStrategy(PlacementStrategy *placementStrategy);

private:
    class Private;
    Private * const d;
};

#endif
