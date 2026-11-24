/*
 *  SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "psd_resource_block.h"

#include <QBuffer>
#include <QDataStream>
#include <QIODevice>

#include <kis_debug.h>

#include "psd.h"
#include "psd_resource_section.h"
#include "psd_utils.h"

PSDResourceBlock::PSDResourceBlock()
    : KisAnnotation("PSD Resource Block", "", QByteArray())
    , identifier(PSDImageResourceSection::UNKNOWN)
    , dataSize(0)
    , resource(0)
{
}

bool PSDResourceBlock::read(QIODevice &io)
{
    dbgFile << "Reading resource block";
    if (io.atEnd()) {
        error = "Could not read resource block: no bytes left.";
        return false;
    }

    QByteArray b;
    b = io.read(4);
    if (b.size() != 4 || QString(b) != "8BIM") {
        error = QString("Could not read resource block signature. Got %1.").arg(QString(b));
        return false;
    }

    if (!psdread(io, identifier)) {
        error = "Could not read resource block identifier";
        return false;
    }

    dbgFile << "\tresource block identifier" << PSDImageResourceSection::idToString((PSDImageResourceSection::PSDResourceID)identifier) << identifier;

    m_type = QString("PSD Resource Block: %1").arg(identifier);

    if (!psdread_pascalstring(io, name, 2)) {
        error = "Could not read name of resource block";
        return false;
    }

    dbgFile << "\tresource block name" << name;

    if (!psdread(io, dataSize)) {
        error = QString("Could not read datasize for resource block with name %1 of type %2").arg(name).arg(identifier);
        return false;
    }

    if ((dataSize & 0x01) != 0) {
        dataSize++;
    }

    dbgFile << "\tresource block size" << dataSize;

    m_description = PSDImageResourceSection::idToString((PSDImageResourceSection::PSDResourceID)identifier);

    data = io.read(dataSize);
    if (data.size() != (int)dataSize) {
        error = QString("Could not read data for resource block with name %1 of type %2").arg(name).arg(identifier);
        return false;
    }

    m_annotation = data;

    switch (identifier) {
    case PSDImageResourceSection::RESN_INFO:
        resource = new RESN_INFO_1005;
        break;
    case PSDImageResourceSection::GRID_GUIDE:
        resource = new GRID_GUIDE_1032;
        break;
    case PSDImageResourceSection::GLOBAL_ANGLE:
        resource = new GLOBAL_ANGLE_1037;
        break;
    case PSDImageResourceSection::ICC_PROFILE:
        resource = new ICC_PROFILE_1039;
        break;
    case PSDImageResourceSection::GLOBAL_ALT:
        resource = new GLOBAL_ALT_1049;
        break;
    default:;
    }

    if (resource) {
        resource->interpretBlock(data);
    }

    return valid();
}

bool PSDResourceBlock::write(QIODevice &io) const
{
    dbgFile << "Writing Resource Block" << PSDImageResourceSection::idToString((PSDImageResourceSection::PSDResourceID)identifier) << identifier;

    if (resource && !resource->valid()) {
        error = QString("Cannot write an invalid Resource Block");
        return false;
    }

    if (identifier == PSDImageResourceSection::LAYER_STATE || identifier == PSDImageResourceSection::LAYER_GROUP
        || identifier == PSDImageResourceSection::LAYER_COMPS || identifier == PSDImageResourceSection::LAYER_GROUP_ENABLED_ID
        || identifier == PSDImageResourceSection::LAYER_SELECTION_ID) {
        dbgFile << "Skip writing resource block" << identifier << displayText();
        return true;
    }

    QByteArray ba;

    if (resource && !resource->createBlock(ba)) {
        error = resource->error;
        return false;
    } else if (!resource) {
        QBuffer buf(&ba);
        buf.open(QBuffer::WriteOnly);
        psdwrite(buf, "8BIM");
        psdwrite(buf, identifier);
        psdwrite_pascalstring(buf, name);
        psdwrite(buf, dataSize);
        buf.write(data);
        buf.close();
    }
    if (io.write(ba.constData(), ba.size()) != ba.size()) {
        error = QString("Could not write complete resource");
        return false;
    }

    return true;
}

bool PSDResourceBlock::valid()
{
    if (identifier == PSDImageResourceSection::UNKNOWN) {
        error = QString("Unknown ID: %1").arg(identifier);
        return false;
    }
    if (data.size() != (int)dataSize) {
        error = QString("Needed %1 bytes, got %2 bytes of data").arg(dataSize).arg(data.length());
        return false;
    }
    return true;
}

bool RESN_INFO_1005::interpretBlock(QByteArray data)
{
    dbgFile << "Reading RESN_INFO_1005";

    QDataStream ds(data);
    ds.setByteOrder(QDataStream::BigEndian);

    ds >> hRes >> hResUnit >> widthUnit >> vRes >> vResUnit >> heightUnit;

    dbgFile << "hres" << hRes << "vres" << vRes;

    hRes = hRes / 65536.0;
    vRes = vRes / 65536.0;

    dbgFile << hRes << hResUnit << widthUnit << vRes << vResUnit << heightUnit;

    return ds.atEnd();
}

bool RESN_INFO_1005::createBlock(QByteArray &data)
{
    dbgFile << "Writing RESN_INFO_1005";
    QBuffer buf(&data);

    startBlock(buf, PSDImageResourceSection::RESN_INFO, 16);

    Fixed h = hRes * 65536.0 + 0.5;
    dbgFile << "h" << h << "hRes" << hRes;
    psdwrite(buf, h);
    psdwrite(buf, hResUnit);
    psdwrite(buf, widthUnit);

    Fixed v = vRes * 65536.0 + 0.5;
    dbgFile << "v" << v << "vRes" << vRes;
    psdwrite(buf, v);
    psdwrite(buf, vResUnit);
    psdwrite(buf, heightUnit);

    buf.close();

    return true;
}

bool ICC_PROFILE_1039::interpretBlock(QByteArray data)
{
    dbgFile << "Reading ICC_PROFILE_1039";

    icc = data;

    return true;
}

bool ICC_PROFILE_1039::createBlock(QByteArray &data)
{
    dbgFile << "Writing ICC_PROFILE_1039";
    if (icc.size() == 0) {
        error = "ICC_PROFILE_1039: Trying to save an empty profile";
        return false;
    }
    QBuffer buf(&data);
    startBlock(buf, PSDImageResourceSection::ICC_PROFILE, icc.size());
    buf.write(icc.constData(), icc.size());
    buf.close();

    return true;
}
