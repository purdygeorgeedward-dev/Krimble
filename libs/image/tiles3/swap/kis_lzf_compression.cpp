/*
 *  SPDX-FileCopyrightText: 2005-2006 Ariya Hidayat <ariya@kde.org>
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_lzf_compression.h"
#include "kis_debug.h"

#define HASH_LOG  12
#define HASH_SIZE (1<< HASH_LOG)
#define HASH_MASK  (HASH_SIZE-1)

#if !defined _MSC_VER
#pragma GCC diagnostic ignored "-Wcast-align"
#endif
#define UPDATE_HASH(v,p) { v = *((quint16*)p); v ^= *((quint16*)(p+1))^(v>>(16-HASH_LOG)); }

#define MAX_COPY       32
#define MAX_LEN       264
#define MAX_DISTANCE 8192

int lzff_compress(const void* input, int length, void* output, int )
{
    const quint8* ip = (const quint8*) input;
    const quint8* ip_limit = ip + length - MAX_COPY - 4;
    quint8* op = (quint8*) output;

    const quint8* htab[HASH_SIZE];
    const quint8** hslot;
    quint32 hval;

    quint8* ref;
    qint32 copy;
    qint32 len;
    qint32 distance;
    quint8* anchor;

    for (hslot = htab; hslot < htab + HASH_SIZE; hslot++)
        *hslot = ip;

    copy = 0;
    *op++ = MAX_COPY - 1;

    while (ip < ip_limit) {
        UPDATE_HASH(hval, ip);
        hslot = htab + (hval & HASH_MASK);
        ref = (quint8*) * hslot;

        *hslot = ip;

        if (ip == ref)
            goto literal;

        if (*((quint16*)ref) != *((quint16*)ip))
            goto literal;

        if (ref[2] != ip[2])
            goto literal;

        distance = ip - ref;

        if (distance >= MAX_DISTANCE)
            goto literal;

        anchor = (quint8*)ip;
        len = 3;
        ref += 3;
        ip += 3;

        if (ip < ip_limit - MAX_LEN) {
            while (len < MAX_LEN - 8) {
                if (*ref++ != *ip++) break;
                if (*ref++ != *ip++) break;
                if (*ref++ != *ip++) break;
                if (*ref++ != *ip++) break;
                if (*ref++ != *ip++) break;
                if (*ref++ != *ip++) break;
                if (*ref++ != *ip++) break;
                if (*ref++ != *ip++) break;
                len += 8;
            }
            ip--;
        }
        len = ip - anchor;

        ip = anchor + len;

        if (copy) {
            anchor = anchor - copy - 1;
            *(op - copy - 1) = copy - 1;
            copy = 0;
        } else
            op--;

        len -= 2;

        distance--;

        if (len < 7)
            *op++ = (len << 5) + (distance >> 8);
        else {
            *op++ = (7 << 5) + (distance >> 8);
            *op++ = len - 7;
        }
        *op++ = (distance & 255);

        *op++ = MAX_COPY - 1;

        --ip;
        UPDATE_HASH(hval, ip);
        htab[hval & HASH_MASK] = ip;
        ip++;

        continue;

    literal:
        *op++ = *ip++;
        copy++;
        if (copy >= MAX_COPY) {
            copy = 0;
            *op++ = MAX_COPY - 1;
        }
    }

    ip_limit = (const quint8*)input + length;
    while (ip < ip_limit) {
        *op++ = *ip++;
        copy++;
        if (copy == MAX_COPY) {
            copy = 0;
            *op++ = MAX_COPY - 1;
        }
    }

    if (copy)
        *(op - copy - 1) = copy - 1;
    else
        op--;

    return op - (quint8*)output;
}

int lzff_decompress(const void* input, int length, void* output, int maxout)
{
    const quint8* ip = (const quint8*) input;
    const quint8* ip_limit  = ip + length - 1;
    quint8* op = (quint8*) output;
    quint8* op_limit = op + maxout;
    quint8* ref;

    while (ip < ip_limit) {
        quint32 ctrl = (*ip) + 1;
        quint32 ofs = ((*ip) & 31) << 8;
        quint32 len = (*ip++) >> 5;

        if (ctrl < 33) {
            if (op + ctrl > op_limit)
                return 0;

            if (ctrl) {
                *op++ = *ip++;
                ctrl--;

                if (ctrl) {
                    *op++ = *ip++;
                    ctrl--;

                    if (ctrl) {
                        *op++ = *ip++;
                        ctrl--;

                        for (; ctrl; ctrl--)
                            *op++ = *ip++;
                    }
                }
            }
        } else {
            len--;
            ref = op - ofs;
            ref--;

            if (len == 7 - 1)
                len += *ip++;

            ref -= *ip++;

            if (op + len + 3 > op_limit)
                return 0;

            if (ref < (quint8 *)output)
                return 0;

            *op++ = *ref++;
            *op++ = *ref++;
            *op++ = *ref++;
            if (len)
                for (; len; --len)
                    *op++ = *ref++;
        }
    }

    return op - (quint8*)output;
}

KisLzfCompression::KisLzfCompression()
{

}

KisLzfCompression::~KisLzfCompression()
{
}

qint32 KisLzfCompression::compress(const quint8* input, qint32 inputLength, quint8* output, qint32 outputLength)
{
    return lzff_compress(input, inputLength, output, outputLength);
}

qint32 KisLzfCompression::decompress(const quint8* input, qint32 inputLength, quint8* output, qint32 outputLength)
{
    return lzff_decompress(input, inputLength, output, outputLength);
}

qint32 KisLzfCompression::outputBufferSize(qint32 dataSize)
{
    return dataSize + dataSize / 16 + 64 + 3;
}
