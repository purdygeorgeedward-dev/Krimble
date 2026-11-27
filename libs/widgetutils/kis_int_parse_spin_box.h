/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2016 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISINTPARSESPINBOX_H
#define KISINTPARSESPINBOX_H

#include <QSpinBox>
#include <QScopedPointer>

#include <kritawidgetutils_export.h>

template <typename SpinBoxTypeTP, typename BaseSpinBoxTypeTP>
class KisParseSpinBoxPrivate;

class KRITAWIDGETUTILS_EXPORT KisIntParseSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    KisIntParseSpinBox(QWidget *parent = 0);
    ~KisIntParseSpinBox() override;

    void stepBy(int steps) override;
    void setValue(int value, bool overwriteExpression = false);
    bool isLastValid() const;
    virtual QString veryCleanText() const;

Q_SIGNALS:
    void errorWhileParsing(const QString &expr) const;
    void noMoreParsingError() const;

protected:
    QValidator::State validate(QString &input, int &pos) const override;
    int valueFromText(const QString &text) const override;
    QString textFromValue(int value) const override;

private:
    template <typename SpinBoxTypeTP, typename BaseSpinBoxTypeTP>
    friend class KisParseSpinBoxPrivate;
    QScopedPointer<KisParseSpinBoxPrivate<KisIntParseSpinBox, QSpinBox>> d;
};

#endif
