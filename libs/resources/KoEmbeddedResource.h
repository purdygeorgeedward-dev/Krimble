/*
 *  SPDX-FileCopyrightText: 2021 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOEMBEDDEDRESOURCE_H
#define KOEMBEDDEDRESOURCE_H

#include <KoResourceSignature.h>

class KRITARESOURCES_EXPORT KoEmbeddedResource
{
public:
    KoEmbeddedResource();
    KoEmbeddedResource(const KoResourceSignature &sig, const QByteArray &data);

    const KoResourceSignature &signature() const;
    QByteArray data() const;

    bool sanityCheckMd5() const;

    bool isValid() const;

private:
    KoResourceSignature m_sig;
    QByteArray m_data;
};

#endif
