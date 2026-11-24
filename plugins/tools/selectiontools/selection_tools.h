/*
 *  SPDX-FileCopyrightText: 2003 Boudewijn Rempt (boud@valdyas.org)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef SELECTION_TOOLS_H_
#define SELECTION_TOOLS_H_

#include <QObject>
#include <QVariant>

class SelectionTools : public QObject
{
    Q_OBJECT
public:
    SelectionTools(QObject *parent, const QVariantList &);
    ~SelectionTools() override;

};

#endif
