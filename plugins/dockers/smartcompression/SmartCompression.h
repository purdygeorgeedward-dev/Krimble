/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  Krimble 2026-11-13 (task 52): George: "When the user edits the selection for SmartCompression they should start with the smart selection." and "This is how XAT handled it." (the Work with Regions mode, the AutoCompress wand, Remove All).
 */

#ifndef _SMARTCOMPRESSION_H_
#define _SMARTCOMPRESSION_H_

#include <QObject>
#include <QVariant>

class SmartCompressionPlugin : public QObject
{
    Q_OBJECT
public:
    SmartCompressionPlugin(QObject *parent, const QVariantList &);
    ~SmartCompressionPlugin() override;
};

#endif
