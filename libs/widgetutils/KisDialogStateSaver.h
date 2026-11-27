/*
 *  SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISDIALOGSTATESAVER_H
#define KISDIALOGSTATESAVER_H

#include "kritawidgetutils_export.h"

#include <QWidget>
#include <QString>
#include <QMap>
#include <QVariant>

namespace KisDialogStateSaver
{
    KRITAWIDGETUTILS_EXPORT void saveState(QWidget *parent, const QString &dialogName);

    KRITAWIDGETUTILS_EXPORT void restoreState(QWidget *parent, const QString &dialogName, const QMap<QString, QVariant> &defaults = QMap<QString, QVariant>());
};

#endif
