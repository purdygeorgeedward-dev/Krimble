/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgSavingContext.h"
#include "SvgUtil.h"

#include <KoXmlWriter.h>
#include <KoShape.h>
#include <KoShapeGroup.h>
#include <KoShapeLayer.h>

#include <QTemporaryFile>

#include <QImage>
#include <QTransform>
#include <QBuffer>
#include <QHash>
#include <QFile>
#include <QFileInfo>
#include <KisMimeDatabase.h>

class Q_DECL_HIDDEN SvgSavingContext::Private
{
public:
    Private(QIODevice *_mainDevice, QIODevice *_styleDevice)
        : mainDevice(_mainDevice)
        , styleDevice(_styleDevice)
        , styleWriter(0)
        , shapeWriter(0)
        , saveInlineImages(true)
    {
        styleWriter.reset(new KoXmlWriter(&styleBuffer, 1));
        styleWriter->startElement("defs");
        shapeWriter.reset(new KoXmlWriter(&shapeBuffer, 1));

        const qreal scaleToUserSpace = SvgUtil::toUserSpace(1.0);
        userSpaceMatrix.scale(scaleToUserSpace, scaleToUserSpace);
    }

    ~Private()
    {
    }

    QIODevice *mainDevice;
    QIODevice *styleDevice;
    QBuffer styleBuffer;
    QBuffer shapeBuffer;
    QScopedPointer<KoXmlWriter> styleWriter;
    QScopedPointer<KoXmlWriter> shapeWriter;

    QHash<QString, int> uniqueNames;
    QHash<const KoShape*, QString> shapeIds;
    QTransform userSpaceMatrix;
    bool saveInlineImages;
    bool strippedTextMode = false;
};

SvgSavingContext::SvgSavingContext(QIODevice &outputDevice, bool saveInlineImages)
    : d(new Private(&outputDevice, 0))
{
    d->saveInlineImages = saveInlineImages;
}

SvgSavingContext::SvgSavingContext(QIODevice &shapesDevice, QIODevice &styleDevice, bool saveInlineImages)
    : d(new Private(&shapesDevice, &styleDevice))
{
    d->saveInlineImages = saveInlineImages;
}

SvgSavingContext::~SvgSavingContext()
{
    d->styleWriter->endElement();

    if (d->styleDevice) {
        d->styleDevice->write(d->styleBuffer.data());
    } else {
        d->mainDevice->write(d->styleBuffer.data());
        d->mainDevice->write("\n");
    }

    d->mainDevice->write(d->shapeBuffer.data());

    delete d;
}

KoXmlWriter &SvgSavingContext::styleWriter()
{
    return *d->styleWriter;
}

KoXmlWriter &SvgSavingContext::shapeWriter()
{
    return *d->shapeWriter;
}

QString SvgSavingContext::createUID(const QString &base)
{
    QString idBase = base.isEmpty() ? "defitem" : base;
    int counter = d->uniqueNames.value(idBase);
    QString res;
    do {
        res = idBase + QString::number(counter);
        counter++;
    } while (d->uniqueNames.contains(res));

    d->uniqueNames.insert(idBase, counter);
    d->uniqueNames.insert(res, 1);
    return res;
}

QString SvgSavingContext::getID(const KoShape *obj)
{
    QString id;
    if (d->shapeIds.contains(obj)) {
        id = d->shapeIds[obj];
    } else {
        id = obj->name();
        if (!id.isEmpty() && !d->uniqueNames.contains(id)) {
            d->uniqueNames.insert(id, 1);
        } else {
            if (id.isEmpty()) {
                if (dynamic_cast<const KoShapeGroup*>(obj))
                    id = "group";
                else if (dynamic_cast<const KoShapeLayer*>(obj))
                    id = "layer";
                else
                    id = "shape";
            }
            id = createUID(id);
        }
        d->shapeIds.insert(obj, id);
    }
    return id;
}

QTransform SvgSavingContext::userSpaceTransform() const
{
    return d->userSpaceMatrix;
}

bool SvgSavingContext::isSavingInlineImages() const
{
    return d->saveInlineImages;
}

QString SvgSavingContext::createFileName(const QString &extension)
{
    QFile *file = qobject_cast<QFile*>(d->mainDevice);
    if (!file)
        return QString();

    QFileInfo fi(file->fileName());
    QString path = fi.absolutePath();
    QString dstBaseFilename = fi.completeBaseName();

    QString fname = dstBaseFilename + '_' + createUID("file");

    int i = 0;
    QString counter;
    while (QFile(path + fname + counter + extension).exists()) {
        counter = QString("_%1").arg(++i);
    }

    return fname + counter + extension;
}

QString SvgSavingContext::saveImage(const QImage &image)
{
    if (isSavingInlineImages()) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        if (image.save(&buffer, "PNG")) {
            const QString header("data:image/x-png;base64,");
            return header + buffer.data().toBase64();
        }
    } else {
        QTemporaryFile imgFile;
        if (image.save(&imgFile, "PNG")) {
            QString dstFilename = createFileName(".png");
            if (QFile::copy(imgFile.fileName(), dstFilename)) {
                return dstFilename;
            }
            else {
                QFile f(imgFile.fileName());
                f.remove();
            }
        }
    }

    return QString();
}

void SvgSavingContext::setStrippedTextMode(bool value)
{
    d->strippedTextMode = value;
}

bool SvgSavingContext::strippedTextMode() const
{
    return d->strippedTextMode;
}
