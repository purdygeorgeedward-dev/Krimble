/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2007 Andreas Hartmetz <ahartmetz@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KSHORTCUTWIDGET_H
#define KSHORTCUTWIDGET_H

#include <kritawidgetutils_export.h>

#include <QKeySequence>
#include <QList>
#include <QWidget>

class KisKActionCollection;
class KisKShortcutWidgetPrivate;

class KRITAWIDGETUTILS_EXPORT KisKShortcutWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool modifierlessAllowed READ isModifierlessAllowed WRITE setModifierlessAllowed)
public:
    KisKShortcutWidget(QWidget *parent = 0);
    ~KisKShortcutWidget() override;

    void setModifierlessAllowed(bool allow);
    bool isModifierlessAllowed();

    void setClearButtonsShown(bool show);

    QList<QKeySequence> shortcut() const;

    void setCheckActionCollections(const QList<KisKActionCollection *> &actionCollections);

Q_SIGNALS:
    void shortcutChanged(const QList<QKeySequence> &cut);

public Q_SLOTS:
    void setShortcut(const QList<QKeySequence> &cut);
    void clearShortcut();

    void applyStealShortcut();

private:
    Q_PRIVATE_SLOT(d, void priKeySequenceChanged(const QKeySequence &))
    Q_PRIVATE_SLOT(d, void altKeySequenceChanged(const QKeySequence &))

private:
    friend class KisKShortcutWidgetPrivate;
    KisKShortcutWidgetPrivate *const d;
};

#endif
