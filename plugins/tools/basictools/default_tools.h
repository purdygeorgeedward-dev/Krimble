/*
 *  SPDX-FileCopyrightText: 2003 Boudewijn Rempt (boud@valdyas.org)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef DEFAULT_TOOLS_H_
#define DEFAULT_TOOLS_H_

#include <QObject>
#include <QVariant>

class DefaultTools : public QObject
{
    Q_OBJECT
public:
    DefaultTools(QObject *parent, const QVariantList &);
    ~DefaultTools() override;

};

#endif
