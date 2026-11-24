/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2016 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISDOUBLEPARSESPINBOX_H
#define KISDOUBLEPARSESPINBOX_H

#include <QDoubleSpinBox>
#include <QScopedPointer>

#include <kritawidgetutils_export.h>

template <typename SpinBoxTypeTP, typename BaseSpinBoxTypeTP>
class KisParseSpinBoxPrivate;

class KRITAWIDGETUTILS_EXPORT KisDoubleParseSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    KisDoubleParseSpinBox(QWidget *parent = 0);
    ~KisDoubleParseSpinBox() override;

    void stepBy(int steps) override;
    void setValue(double value, bool overwriteExpression = false);
    bool isLastValid() const;
    virtual QString veryCleanText() const;

Q_SIGNALS:
    void errorWhileParsing(const QString &expr) const;
    void noMoreParsingError() const;

protected:
    QValidator::State validate(QString &input, int &pos) const override;
    double valueFromText(const QString &text) const override;
    QString textFromValue(double value) const override;

private:
    template <typename SpinBoxTypeTP, typename BaseSpinBoxTypeTP>
    friend class KisParseSpinBoxPrivate;
    QScopedPointer<KisParseSpinBoxPrivate<KisDoubleParseSpinBox, QDoubleSpinBox>> d;
};

#endif
