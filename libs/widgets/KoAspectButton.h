/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOASPECTBUTTON_H
#define KOASPECTBUTTON_H

#include <QToolButton>

#include "kritawidgets_export.h"

class KRITAWIDGETS_EXPORT KoAspectButton : public QToolButton {
    Q_OBJECT
public:
    explicit KoAspectButton(QWidget *parent);
    ~KoAspectButton() override;

    bool keepAspectRatio() const;

public Q_SLOTS:
    void setKeepAspectRatio(bool keep);

    void buttonReleased();

Q_SIGNALS:
    void keepAspectRatioChanged(bool keep);

private:
    class Private;
    Private * const d;
};

#endif
