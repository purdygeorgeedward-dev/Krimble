/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGSAVINGCONTEXT_H
#define SVGSAVINGCONTEXT_H

#include <QtGlobal>

class KoXmlWriter;
class KoShape;
class QIODevice;
class QString;
class QTransform;
class QImage;

#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT SvgSavingContext
{
public:
    explicit SvgSavingContext(QIODevice &outputDevice, bool saveInlineImages = true);
    explicit SvgSavingContext(QIODevice &shapesDevice, QIODevice &styleDevice, bool saveInlineImages = true);

    virtual ~SvgSavingContext();

    KoXmlWriter &styleWriter();

    KoXmlWriter &shapeWriter();

    QString createUID(const QString &base);

    QString getID(const KoShape *obj);

    QTransform userSpaceTransform() const;

    bool isSavingInlineImages() const;

    QString createFileName(const QString &extension);

    QString saveImage(const QImage &image);

    void setStrippedTextMode(bool value);
    bool strippedTextMode() const;

private:
    Q_DISABLE_COPY(SvgSavingContext)

private:
    class Private;
    Private * const d;
};

#endif
