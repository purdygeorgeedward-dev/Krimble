/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_CUSTOM_MODIFIERS_CATCHER_H
#define __KIS_CUSTOM_MODIFIERS_CATCHER_H

#include <QScopedPointer>
#include <QObject>

class KisCustomModifiersCatcher : public QObject
{
public:
    KisCustomModifiersCatcher(QObject *parent);
    ~KisCustomModifiersCatcher() override;

    bool eventFilter(QObject* object, QEvent* event) override;

    void addModifier(const QString &id, Qt::Key modifier);

    bool modifierPressed(const QString &id);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
