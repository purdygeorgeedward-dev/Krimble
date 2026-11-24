/*
 *  SPDX-FileCopyrightText: 2013 Juan Palacios <jpalaciosdev@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISSIZEGROUP_H
#define KISSIZEGROUP_H

#include <QObject>
#include "kritaui_export.h"

class QWidget;
class KisSizeGroupPrivate;

class KRITAUI_EXPORT KisSizeGroup : public QObject
{
    Q_OBJECT

public:
    enum mode
    {
        KIS_SIZE_GROUP_NONE = 0,
        KIS_SIZE_GROUP_HORIZONTAL = 1 << 0,
        KIS_SIZE_GROUP_VERTICAL = 1 << 1,
        KIS_SIZE_GROUP_BOTH = (KIS_SIZE_GROUP_HORIZONTAL | KIS_SIZE_GROUP_VERTICAL)
    };

    explicit KisSizeGroup(QObject* parent = 0,
                         KisSizeGroup::mode mode = KisSizeGroup::KIS_SIZE_GROUP_HORIZONTAL,
                         bool ignoreHidden = false);

    ~KisSizeGroup() override;

    void setMode(KisSizeGroup::mode mode);

    KisSizeGroup::mode getMode() const;

    void setIgnoreHidden(bool ignoreHidden);

    bool isIgnoreHidden() const;

    void addWidget(QWidget *widget);

    void removeWidget(QWidget *widget);

private:
    KisSizeGroupPrivate* const d;
};

#endif
