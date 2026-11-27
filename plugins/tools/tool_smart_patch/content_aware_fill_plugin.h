/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CONTENT_AWARE_FILL_PLUGIN_H_
#define CONTENT_AWARE_FILL_PLUGIN_H_

#include <QObject>
#include <QVariant>
#include <KisActionPlugin.h>

class ContentAwareFillPlugin : public KisActionPlugin
{
    Q_OBJECT
public:
    ContentAwareFillPlugin(QObject *parent, const QVariantList &);
    ~ContentAwareFillPlugin() override;

private Q_SLOTS:
    void slotContentAwareFill();
};

#endif
