/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Rob Buis (buis@kde.org)
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOUNITDOUBLESPINBOX_H
#define KOUNITDOUBLESPINBOX_H

#include "kritawidgets_export.h"

#include <QDoubleSpinBox>

class KoUnit;

class KRITAWIDGETS_EXPORT KoUnitDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit KoUnitDoubleSpinBox( QWidget *parent = 0);
    ~KoUnitDoubleSpinBox() override;

    virtual void changeValue( double newValue );
    virtual void setUnit( const KoUnit &);

    double value( ) const;

    void setMinimum(double min);

    void setMaximum(double max);

    void setLineStep(double step);

    void setLineStepPt(double step);

    void setMinMaxStep( double min, double max, double step );

    QValidator::State validate(QString &input, int &pos) const override;

    QString textFromValue( double value ) const override;
    double valueFromText( const QString& str ) const override;

Q_SIGNALS:
    void valueChangedPt( qreal );

private:
    class Private;
    Private * const d;

private Q_SLOTS:
    void privateValueChanged();
};

#endif

