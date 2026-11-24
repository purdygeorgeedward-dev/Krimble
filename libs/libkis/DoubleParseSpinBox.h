/*
 * SPDX-FileCopyrightText: 2016 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef LIBKIS_DOUBLEPARSESPINBOX_H
#define LIBKIS_DOUBLEPARSESPINBOX_H

#include "kis_double_parse_spin_box.h"

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT DoubleParseSpinBox : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DoubleParseSpinBox)

public:
    explicit DoubleParseSpinBox();
    ~DoubleParseSpinBox() override;

public Q_SLOTS:
    QDoubleSpinBox* widget() const;

    void stepBy(int steps);
    void setValue(double value, bool overwriteExpression = false);
    bool isLastValid() const;
    virtual QString veryCleanText() const;

Q_SIGNALS:
    void errorWhileParsing(const QString &expr) const;
    void noMoreParsingError() const;

Q_SIGNALS:

private:
    struct Private;
    Private *const d;

};

#endif
