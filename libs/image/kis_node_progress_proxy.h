/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_NODE_PROGRESS_PROXY_H_
#define _KIS_NODE_PROGRESS_PROXY_H_

#include <KoProgressProxy.h>
#include <QObject>

#include <kis_types.h>

#include "kritaimage_export.h"

#include <kis_node.h>
class KRITAIMAGE_EXPORT KisNodeProgressProxy : public QObject, public KoProgressProxy
{
    Q_OBJECT
    friend class KisNode;
    explicit KisNodeProgressProxy(KisNode* _node);
    ~KisNodeProgressProxy() override;

public:
    int maximum() const override;
    void setValue(int value) override;
    void setRange(int minimum, int maximum) override;
    void setFormat(const QString & format) override;
    int percentage() const;
Q_SIGNALS:
    void percentageChanged(int _percentage, const KisNodeSP& _node);

private:
    void prepareDestroying();

private:
    struct Private;
    Private* const d;
};

#endif
