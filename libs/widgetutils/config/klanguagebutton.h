/*
 * klangbutton.h - Button with language selection drop down menu.
 *                 Derived from the KLangCombo class by Hans Petter Bieker.
 *
 * SPDX-FileCopyrightText: 1999-2003 Hans Petter Bieker <bieker@kde.org>
 * SPDX-FileCopyrightText: 2001 Martijn Klingens <klingens@kde.org>
 * SPDX-FileCopyrightText: 2007 David Jarvie <software@astrojar.org.uk>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KLANGUAGEBUTTON_H
#define KLANGUAGEBUTTON_H

#include "kritawidgetutils_export.h"
#include <QWidget>

class QAction;
class KLanguageButtonPrivate;

class KRITAWIDGETUTILS_EXPORT KLanguageButton : public QWidget
{
    Q_OBJECT

public:
    explicit KLanguageButton(QWidget *parent = 0);

    explicit KLanguageButton(const QString &text, QWidget *parent = 0);

    ~KLanguageButton() override;

    void setLocale(const QString &locale);

    void setText(const QString &text);

    void showLanguageCodes(bool show);

    void loadAllLanguages();

    void insertLanguage(const QString &languageCode, const QString &name = QString(), int index = -1);

    void insertSeparator(int index = -1);

    int count() const;

    void clear();

    QString current() const;

    bool contains(const QString &languageCode) const;

    void setCurrentItem(const QString &languageCode);

Q_SIGNALS:
    void activated(const QString &languageCode);
    void highlighted(const QString &languageCode);

private Q_SLOTS:
    void slotTriggered(QAction *);
    void slotHovered(QAction *);

private:
    KLanguageButtonPrivate *const d;
};

#endif
