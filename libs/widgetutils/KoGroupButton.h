/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2007 Aurélien Gâteau <agateau@kde.org>
   SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
   SPDX-FileCopyrightText: 2012 Jarosław Staniek <staniek@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef KOGROUPBUTTON_H
#define KOGROUPBUTTON_H

#include "kritawidgetutils_export.h"

#include <KisHighlightedToolButton.h>

class KRITAWIDGETUTILS_EXPORT KoGroupButton : public KisHighlightedToolButton
{
    Q_OBJECT
    Q_ENUMS( GroupPosition )
    Q_PROPERTY( GroupPosition groupPosition READ groupPosition WRITE setGroupPosition )
public:
    enum GroupPosition {
        NoGroup,
        GroupLeft,
        GroupRight,
        GroupCenter
    };

    explicit KoGroupButton(GroupPosition position, QWidget* parent = 0);

    explicit KoGroupButton(QWidget* parent = 0);

    ~KoGroupButton() override;

    void setGroupPosition(KoGroupButton::GroupPosition groupPosition);

    KoGroupButton::GroupPosition groupPosition() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    class Private;
    Private *const d;
};

#endif
