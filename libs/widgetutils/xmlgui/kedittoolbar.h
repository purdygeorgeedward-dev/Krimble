/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
   SPDX-FileCopyrightText: 2006 Hamish Rodda <rodda@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef KEDITTOOLBAR_H
#define KEDITTOOLBAR_H

#include <QDialog>

#include <kritawidgetutils_export.h>

class KisKActionCollection;

class KisKEditToolBarPrivate;
class KisKXMLGUIFactory;
class KRITAWIDGETUTILS_EXPORT KisKEditToolBar : public QDialog
{
    Q_OBJECT
public:
    explicit KisKEditToolBar(KisKXMLGUIFactory *factory,
                          QWidget *parent = 0);

    ~KisKEditToolBar() override;

    void setDefaultToolBar(const QString &toolBarName);

    void setResourceFile(const QString &file, bool global = true);

    static void setGlobalDefaultToolBar(const char *toolBarName);

Q_SIGNALS:
    void newToolBarConfig();

    QT_MOC_COMPAT void newToolbarConfig();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    friend class KisKEditToolBarPrivate;
    KisKEditToolBarPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_slotButtonClicked(QAbstractButton *))
    Q_PRIVATE_SLOT(d, void _k_acceptOK(bool))
    Q_PRIVATE_SLOT(d, void _k_enableApply(bool))

    Q_DISABLE_COPY(KisKEditToolBar)
};

#endif
