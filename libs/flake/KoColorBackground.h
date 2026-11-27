/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCOLORBACKGROUND_H
#define KOCOLORBACKGROUND_H

#include "KoShapeBackground.h"
#include "kritaflake_export.h"
#include <Qt>
#include <QSharedDataPointer>

class KoColorBackgroundPrivate;
class QColor;
class QBrush;

class KRITAFLAKE_EXPORT KoColorBackground : public KoShapeBackground
{
public:
    KoColorBackground();

    explicit KoColorBackground(const QColor &color, Qt::BrushStyle style = Qt::SolidPattern);

    ~KoColorBackground() override;

    KoColorBackground(const KoColorBackground &);
    KoColorBackground &operator=(const KoColorBackground &);

    bool compareTo(const KoShapeBackground *other) const override;

    QColor color() const;

    void setColor(const QColor &color);

    Qt::BrushStyle style() const;

    QBrush brush() const;

    void paint(QPainter &painter, const QPainterPath &fillPath) const override;

private:
    class Private;
    QSharedDataPointer<Private> d;
};

#endif
