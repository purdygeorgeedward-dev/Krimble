/*
 * SPDX-FileCopyrightText: 2016 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 * SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef LIBKIS_INTPARSESPINBOX_H
#define LIBKIS_INTPARSESPINBOX_H

#include "kis_int_parse_spin_box.h"

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT IntParseSpinBox : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(IntParseSpinBox)

public:
    explicit IntParseSpinBox();
    ~IntParseSpinBox() override;

public Q_SLOTS:

    QSpinBox* widget() const;

    void stepBy(int steps);
    void setValue(int value, bool overwriteExpression = false);
    bool isLastValid() const;
    virtual QString veryCleanText() const;

Q_SIGNALS:
    void errorWhileParsing(const QString &expr) const;
    void noMoreParsingError() const;

private:
    struct Private;
    Private *const d;

};

#endif
