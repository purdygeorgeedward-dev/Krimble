/*
 *    This file is part of the KDE project
 *    SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *    SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *    SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *    SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *    SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISTAGTOOLBUTTON_H
#define KISTAGTOOLBUTTON_H

#include <QWidget>
#include <KisTag.h>

class KisTagModel;

#include <KoResource.h>

class KisTagToolButton : public QWidget
{
    Q_OBJECT

private:
    explicit KisTagToolButton(QWidget* parent = 0);
    ~KisTagToolButton() override;

    void readOnlyMode(bool activate);

    void setUndeletionCandidate(const KisTagSP deletedTag);
    KisTagSP undeletionCandidate() const;

    void setCurrentTag(const KisTagSP tag);

    void loadIcon();

Q_SIGNALS:
    void newTagRequested(const QString &tagName);

    void renamingOfCurrentTagRequested(const QString &tagName);

    void deletionOfCurrentTagRequested();

    void undeletionOfTagRequested(const KisTagSP tag);
    void popupMenuAboutToShow();

private Q_SLOTS:
    void onTagUndeleteClicked();

private:
    class Private;
    Private* const d;
    friend class KisTagChooserWidget;
};

#endif
