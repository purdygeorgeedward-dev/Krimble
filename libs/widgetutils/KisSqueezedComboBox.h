/* ============================================================
 * Author: Tom Albers <tomalbers@kde.nl>
 * Date  : 2005-01-01
 * Description :
 *
 * SPDX-FileCopyrightText: 2005 Tom Albers <tomalbers@kde.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef KISSQUEEZEDCOMBOBOX_H
#define KISSQUEEZEDCOMBOBOX_H

class QTimer;
class QResizeEvent;
class QWidget;

#include <QComboBox>
#include <QWidget>
#include <QIcon>

#include "kritawidgetutils_export.h"

class KRITAWIDGETUTILS_EXPORT KisSqueezedComboBox : public QComboBox
{
    Q_OBJECT

public:

    KisSqueezedComboBox(QWidget *parent = 0, const char *name = 0);
    ~KisSqueezedComboBox() override;

    bool contains(const QString & text) const;

    qint32 findOriginalText(const QString & text) const;

    QStringList originalTexts() const;

    void resetOriginalTexts(const QStringList &texts);

    void insertSqueezedItem(const QString& newItem, int index, QVariant userData = QVariant());
    void insertSqueezedItem(const QIcon &icon, const QString& newItem, int index, QVariant userData = QVariant());

    void addSqueezedItem(const QString& newItem, QVariant userData = QVariant());

    void addSqueezedItem(const QIcon &icon, const QString& newItem, QVariant userData = QVariant());

    void setCurrent(const QString& itemText);

    QString currentUnsqueezedText();

    void removeSqueezedItem(int index);

    QSize sizeHint() const override;

    static QString squeezeText(const QString& original, const QWidget *widget);

private Q_SLOTS:
    void slotTimeOut();

private:
    void resizeEvent(QResizeEvent *) override;

    void setCurrentText(const QString& itemText);
    void insertItem(const QString &text);
    void insertItem(qint32 index, const QString &text);
    void addItem(const QString &text);

    QMap<int, QString> m_originalItems;
    QTimer *m_timer;

};

#endif
