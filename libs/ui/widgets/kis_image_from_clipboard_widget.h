/*
 * SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_IMAGE_FROM_CLIPBOARD_WIDGET_H
#define KIS_IMAGE_FROM_CLIPBOARD_WIDGET_H

#include "kis_global.h"
#include "kis_properties_configuration.h"
#include "kis_custom_image_widget.h"

class KisImageFromClipboardWidget : public KisCustomImageWidget
{
    Q_OBJECT
public:
    KisImageFromClipboardWidget(QWidget *parent, qint32 defWidth, qint32 defHeight, double resolution, const QString & defColorModel, const QString & defColorDepth, const QString & defColorProfile, const QString & imageName);
    ~KisImageFromClipboardWidget() override;

protected:

    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    void createClipboardPreview();
    void createImage();
    void clipboardDataChanged();
    void enableImageCreation(const QImage &);
};

#endif

