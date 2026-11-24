/*
 * SPDX-FileCopyrightText: 2009 Shawn T. Rutledge (shawn.t.rutledge@gmail.com)
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "qgiflibhandler.h"
#include <QDebug>
#include <QVariant>
#include <gif_lib.h>
#include <string.h>
#include <QPainter>

extern int _GifError;

static const int InterlacedOffset[] = { 0, 4, 2, 1 };
static const int InterlacedJumps[] = { 8, 8, 4, 2 };

int doOutput(GifFileType* gif, const GifByteType * data, int i)
{
    QIODevice* out = (QIODevice*)gif->UserData;
    return out->write((const char*)data, i);
}

int doInput(GifFileType* gif, GifByteType* data, int i)
{
    QIODevice* in = (QIODevice*)gif->UserData;
    return in->read((char*)data, i);
}

QGIFLibHandler::QGIFLibHandler()
    : QImageIOHandler()
{
}

bool QGIFLibHandler::canRead () const
{
    if (canRead(device())) {
        setFormat("gif");
        return true;
    }
    return false;
}

bool QGIFLibHandler::read ( QImage * image )
{

    int err;
    GifFileType* gifFile = DGifOpen(device(), doInput, &err);
    if (!gifFile) {
        qWarning() << "Received error code" << err;
        return false;
    }

    *image = QImage(gifFile->SWidth, gifFile->SHeight, QImage::Format_Indexed8);

    GifRecordType recordType;
    ColorMapObject* ColorMap;

    int	i, row, imageNum = 0, topRow, width, height;
    int transColor = -1;
    do
    {
        DGifGetRecordType(gifFile, &recordType);
        switch (recordType)
        {
        case IMAGE_DESC_RECORD_TYPE:
            if (DGifGetImageDesc(gifFile) == GIF_ERROR)
            {
                qWarning("QGIFLibHandler::read: error %d", gifFile->Error);
                return false;
            }
            topRow = gifFile->Image.Top;
            width = gifFile->Image.Width;
            height = gifFile->Image.Height;
            if (gifFile->Image.Left + width > gifFile->SWidth ||
                    gifFile->Image.Top + height > gifFile->SHeight)
            {
                qWarning("Image %d is not confined to screen dimension, aborted.", imageNum);
                return false;
            }

            image->fill(gifFile->SBackGroundColor);

            if (gifFile->Image.Interlace)
            {
                for (i = 0; i < 4; i++)
                    for (row = topRow + InterlacedOffset[i]; row < topRow + height;
                         row += InterlacedJumps[i])
                    {
                        if (DGifGetLine(gifFile, image->scanLine(row), width) == GIF_ERROR)
                        {
                            qWarning("QGIFLibHandler::read: error %d", gifFile->Error);
                            return false;
                        }
                    }
            }
            else
            {
                for (row = 0; row < height; row++)
                {
                    if (DGifGetLine(gifFile, image->scanLine(row), width) == GIF_ERROR)
                    {
                        qWarning("QGIFLibHandler::read: error %d", gifFile->Error);
                        return false;
                    }
                }
            }
            break;
        case EXTENSION_RECORD_TYPE:
        {
            int extCode;
            GifByteType* extData;
            if (DGifGetExtension(gifFile, &extCode, &extData) == GIF_ERROR)
            {
                qWarning("QGIFLibHandler::read: error %d", gifFile->Error);
                return false;
            }
            while (extData != NULL)
            {
                int len = extData[0];
                switch (extCode)
                {
                case GRAPHICS_EXT_FUNC_CODE:
                    if (extData[1] & 0x01)
                    {
                        transColor = extData[3];
                    }
                    break;
                case COMMENT_EXT_FUNC_CODE:
                {
                    QByteArray comment((char*)(extData + 1), len);
                    image->setText("Description", comment);
                }
                    break;
                case PLAINTEXT_EXT_FUNC_CODE:
                    break;
                }
                if (DGifGetExtensionNext(gifFile, &extData) == GIF_ERROR)
                {
                    qWarning("QGIFLibHandler::read: error %d", gifFile->Error);
                    return false;
                }
            }
        }
            break;
        case TERMINATE_RECORD_TYPE:
            break;
        default:
            break;
        }
    }
    while (recordType != TERMINATE_RECORD_TYPE);

    ColorMap = (gifFile->Image.ColorMap
                ? gifFile->Image.ColorMap
                : gifFile->SColorMap);
    if (!ColorMap)
    {
        qWarning("QGIFLibHandler::read: Image does not have a colormap");
        return false;
    }
    int ccount = ColorMap->ColorCount;
    image->setColorCount(ccount);
    for (i = 0; i < ccount; ++i)
    {
        GifColorType gifColor = ColorMap->Colors[i];
        QRgb color = gifColor.Blue | (gifColor.Green << 8) | (gifColor.Red << 16);
        if (i != transColor)
            color |= 0xff << 24;
        image->setColor(i, color);
    }

    return true;
}

bool QGIFLibHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("QGIFLibHandler::canRead() called with no device");
        return false;
    }

    char head[6];
    if (device->peek(head, sizeof(head)) == sizeof(head))
        return qstrncmp(head, "GIF87a", 6) == 0
                || qstrncmp(head, "GIF89a", 6) == 0;
    return false;
}

bool QGIFLibHandler::write ( const QImage & image )
{
    QImage toWrite(image);
    if (toWrite.colorCount() == 0 || toWrite.colorCount() > 256)
        toWrite = image.convertToFormat(QImage::Format_Indexed8);

    QVector<QRgb> colorTable = toWrite.colorTable();
    ColorMapObject cmap;
    int colorCount = 1 << GifBitSize(toWrite.colorCount());
    cmap.ColorCount = colorCount;
    cmap.BitsPerPixel = 8;
    GifColorType* colorValues = (GifColorType*)malloc(cmap.ColorCount * sizeof(GifColorType));
    cmap.Colors = colorValues;
    int c = 0;
    for(; c < toWrite.colorCount(); ++c)
    {
        colorValues[c].Red = qRed(colorTable[c]);
        colorValues[c].Green = qGreen(colorTable[c]);
        colorValues[c].Blue = qBlue(colorTable[c]);
    }
    for (; c < colorCount; ++c)
    {
        colorValues[c].Red = 0;
        colorValues[c].Green = 0;
        colorValues[c].Blue = 0;
    }

    int err;
    GifFileType *gif = EGifOpen(device(), doOutput, &err);

    EGifSetGifVersion(gif, true);

    if (EGifPutScreenDesc(gif, toWrite.width(), toWrite.height(), colorCount, 0, &cmap) == GIF_ERROR) {
        qWarning("EGifPutScreenDesc returned error %d", gif->Error);
    }

    QVariant descText = option(QImageIOHandler::Description);
    if (descText.type() == QMetaType::QString)
    {
        QString comment = descText.toString();
        int idx = comment.indexOf(": ");
        if (idx >= 0)
            comment.remove(0, idx + 2);
        if (!comment.isEmpty())
            EGifPutComment(gif, comment.toUtf8().constData());
    }

    if (EGifPutImageDesc(gif, 0, 0, toWrite.width(), toWrite.height(), 0, &cmap) == GIF_ERROR)
        qWarning("EGifPutImageDesc returned error %d", gif->Error);

    int lc = toWrite.height();

    int llen = toWrite.width();

    for (int l = 0; l < lc; ++l)
    {
        uchar* line = toWrite.scanLine(l);
        if (EGifPutLine(gif, (GifPixelType*)line, llen) == GIF_ERROR)
        {
            int i = gif->Error;
            qWarning("EGifPutLine returned error %d", i);
        }
    }

    EGifCloseFile(gif, &err);
    free(colorValues);

    return true;
}

bool QGIFLibHandler::supportsOption ( ImageOption option ) const
{
    switch (option)
    {
    case QImageIOHandler::ImageFormat:
    case QImageIOHandler::Size:
    case QImageIOHandler::Description:
        return true;
        break;
    default:
        return false;
    }
}

void QGIFLibHandler::setOption ( ImageOption option, const QVariant & value )
{
    if (option == QImageIOHandler::Description)
        m_description = value.toString();
}

QVariant QGIFLibHandler::option( ImageOption option ) const
{
    switch (option)
    {
    case QImageIOHandler::ImageFormat:
        return QVariant();
        break;
    case QImageIOHandler::Size:
        return QVariant();
        break;
    case QImageIOHandler::Description:
        return QVariant(m_description);
        break;
    default:
        return QVariant();
    }
}
