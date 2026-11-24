/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KIS_FONT_FAMILY_COMBO_BOX_H
#define KIS_FONT_FAMILY_COMBO_BOX_H

#include <QObject>
#include <KisSqueezedComboBox.h>
#include <QFont>
#include <QFontDatabase>
#include <QStyledItemDelegate>

#include "kritawidgetutils_export.h"

class KRITAWIDGETUTILS_EXPORT KisFontComboBoxes : public QWidget
{
    Q_OBJECT
public:
    KisFontComboBoxes(QWidget *parent = 0);

    void setCurrentFont(QFont font);

    void setCurrentFamily(const QString family);
    void setCurrentStyle(QString style);

    QString currentFamily() const;
    QString currentStyle() const;

    QFont currentFont(int pointSize = 10) const;

    void refillComboBox(QVector<QFontDatabase::WritingSystem> writingSystems = QVector<QFontDatabase::WritingSystem>());
    void setInitialized();

Q_SIGNALS:
    void fontChanged(QString);
private Q_SLOTS:
    void fontFamilyChanged();
    void fontChange();
private:
    QComboBox *m_family;
    QComboBox *m_styles;
};

class KRITAWIDGETUTILS_EXPORT PinnedFontsSeparator : public QStyledItemDelegate {
public:
    PinnedFontsSeparator(QAbstractItemDelegate *_default, QWidget *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setSeparatorIndex(int index);
    void setSeparatorAdded();
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    int m_separatorIndex;
    bool m_separatorAdded;
    QAbstractItemDelegate *m_defaultDelegate;
};

class KRITAWIDGETUTILS_EXPORT KisFontFamilyComboBox : public QComboBox
{
    Q_OBJECT
public:
    KisFontFamilyComboBox(QWidget *parent = 0);

    void refillComboBox(QVector<QFontDatabase::WritingSystem> writingSystems = QVector<QFontDatabase::WritingSystem>());
    void setTopFont(const QString &family);
    void setInitialized();

private Q_SLOTS:
    void slotTextChanged(const QString &input);

private:
    QStringList m_pinnedFonts;
    QStringList m_blacklistedFonts;
    bool m_initialized {false};
    bool m_initializeFromConfig;
    int m_separatorIndex;
    PinnedFontsSeparator *m_fontSeparator;
};

#endif
