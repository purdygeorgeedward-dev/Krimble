/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISINPUTBUTTON_H
#define KISINPUTBUTTON_H

#include <QPushButton>

#include "input/kis_shortcut_configuration.h"

class KisInputButton : public QPushButton
{
    Q_OBJECT
public:
    enum ButtonType {
        MouseType,
        KeyType,
        WheelType,
    };

    explicit KisInputButton(QWidget *parent = nullptr);
    ~KisInputButton() override;

    ButtonType type() const;
    void setType(ButtonType newType);

    QList<Qt::Key> keys() const;
    void setKeys(const QList<Qt::Key> &newKeys);

    Qt::MouseButtons buttons() const;
    void setButtons(Qt::MouseButtons newButtons);

    KisShortcutConfiguration::MouseWheelMovement wheel() const;
    void setWheel(KisShortcutConfiguration::MouseWheelMovement wheel);

public Q_SLOTS:
    void clear();

Q_SIGNALS:
    void dataChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private Q_SLOTS:
    void reset();

private:
    class Private;
    Private *const d {nullptr};
};

#endif
