/*
 * SPDX-FileCopyrightText: 2017 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef HTMLSAVINGCONTEXT_H
#define HTMLSAVINGCONTEXT_H

#include <QScopedPointer>

class KoXmlWriter;
class KoShape;
class QIODevice;
class QString;
class QTransform;
class QImage;

class HtmlSavingContext
{
public:
    HtmlSavingContext(QIODevice &shapeDevice);
    virtual ~HtmlSavingContext();
    KoXmlWriter &shapeWriter();
private:
    Q_DISABLE_COPY(HtmlSavingContext)
private:
    struct Private;
    const QScopedPointer<Private> d;
};

#endif
