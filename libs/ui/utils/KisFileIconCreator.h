/*
 *  SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_FILE_ICON_CREATOR_H
#define KIS_FILE_ICON_CREATOR_H

#include <QIcon>

#include <KisPreviewFileDialog.h>

#include "kritaui_export.h"

class KRITAUI_EXPORT KisFileIconCreator : public KisAbstractFileIconCreator
{

public:
    bool createFileIcon(QString path, QIcon &icon, qreal devicePixelRatioF, QSize iconSize) override;
};

#endif
