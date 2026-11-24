/*
 *  SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_NODE_QUERY_PATH_H_
#define _KIS_NODE_QUERY_PATH_H_

#include <kis_types.h>
#include <kritaimage_export.h>

class KRITAIMAGE_EXPORT KisNodeQueryPath
{
    KisNodeQueryPath();
public:
    ~KisNodeQueryPath();
    KisNodeQueryPath(const KisNodeQueryPath&);
    KisNodeQueryPath& operator=(const KisNodeQueryPath&);
    QList<KisNodeSP> queryNodes(KisImageWSP image, KisNodeSP currentNode) const;
    KisNodeSP queryUniqueNode(KisImageWSP image, KisNodeSP currentNode = 0) const;
    bool isRelative() const;
    QString toString() const;
    static KisNodeQueryPath fromString(const QString& path);
    static KisNodeQueryPath absolutePath(KisNodeSP node);
private:
    struct Private;
    Private* const d;
};

#endif
