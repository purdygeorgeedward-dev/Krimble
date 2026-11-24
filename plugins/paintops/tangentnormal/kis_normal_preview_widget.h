/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2015 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISNORMALPREVIEWWIDGET_H
#define KISNORMALPREVIEWWIDGET_H

#include <QLabel>

class KisNormalPreviewWidget : public QLabel
{
    Q_OBJECT
public:

    KisNormalPreviewWidget(QWidget* parent = 0);
    ~KisNormalPreviewWidget() override;

public Q_SLOTS:
    void setRedChannel(int index);
    void setGreenChannel(int index);
    void setBlueChannel(int index);

private:

    void updateImage();
    QImage swizzleTransformPreview (QImage preview);
    int previewTransform(int const horizontal, int const vertical, int const depth, int index, int maxvalue);

    int m_redChannel;
    int m_greenChannel;
    int m_blueChannel;
    QString m_fileName;
};

#endif
