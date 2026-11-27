/*
 *  SPDX-FileCopyrightText: 2016 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 *  SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_DOUBLEPARSEUNITSPINBOX_H
#define KIS_DOUBLEPARSEUNITSPINBOX_H

#include <KoUnit.h>
#include <QContextMenuEvent>

#include "kis_double_parse_spin_box.h"
#include "kritawidgetutils_export.h"

class KisSpinBoxUnitManager;

class KRITAWIDGETUTILS_EXPORT  KisDoubleParseUnitSpinBox : public KisDoubleParseSpinBox
{

    Q_OBJECT

public:
    KisDoubleParseUnitSpinBox(QWidget* parent = 0);
    ~KisDoubleParseUnitSpinBox() override;

    void setUnitManager(KisSpinBoxUnitManager* unitManager);

    virtual void changeValue( double newValue );

    void changeValuePt( double newValue );

    virtual void setUnit(const KoUnit &unit);
    virtual void setUnit(const QString & symbol);

    void setReturnUnit(const QString & symbol);

    QString returnUnit() const;

    virtual void setDimensionType(int dim);

    double value( ) const;

    double valuePt( ) const;

    void setValuePt(double value, bool overWriteExpression = false);

    void setMinimum(double min);

    void setMinimumPt(double min);

    void setMaximum(double max);

    void setMaximumPt(double max);

    void setLineStep(double step);

    void setLineStepPt(double step);

    void setMinMaxStep( double min, double max, double step );

    void setMinMaxStepPt( double min, double max, double step );

    QString textFromValue( double value ) const override;

    QString veryCleanText() const override;

    double valueFromText( const QString& str ) const override;

    void setUnitChangeFromOutsideBehavior(bool toggle);

    void setDisplayUnit(bool toggle);

    void preventDecimalsChangeFromUnitManager(bool prevent);

    void setDecimals(int prec);
    void setSingleStep(double val);

Q_SIGNALS:
    void valueChangedPt( qreal );

private:
    class Private;
    Private * const d;

    QString detectUnit();
    QString makeTextClean(QString const& txt) const;

    void internalUnitChange(QString const& symbol);
    void prepareUnitChange();

    void contextMenuEvent(QContextMenuEvent *event) override;

private Q_SLOTS:
    void privateValueChanged();
    void detectUnitChanges();
    void disconnectExternalUnitManager();

};

#endif
