/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KIS_DLG_FILE_LAYER_H
#define KIS_DLG_FILE_LAYER_H

#include <KoDialog.h>
#include <QString>

#include <kis_file_layer.h>

#include "ui_wdgdlgfilelayer.h"

class KisDlgFileLayer : public KoDialog
{
public:

    Q_OBJECT

public:

    KisDlgFileLayer(const QString &basePath, const QString &name, QWidget *parent = 0);
    QString fileName() const;
    QString layerName() const;
    KisFileLayer::ScalingMethod scaleToImageResolution() const;
    QString scalingFilter() const;

    void setFileName(QString fileName);
    void setScalingMethod(KisFileLayer::ScalingMethod method);
    void setScalingFilter(QString filter);

protected Q_SLOTS:
    void slotNameChanged(const QString &);
    void slotMethodChanged(const bool &);

private:

    Ui_WdgDlgFileLayer dlgWidget;
    QString m_basePath;
};

#endif
