/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Rob Buis (buis@kde.org)
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoUnitDoubleSpinBox.h"

#include <KoUnit.h>

#include <QRegularExpression>

#include <WidgetsDebug.h>

#include <klocalizedstring.h>
#include <qnumeric.h>

class Q_DECL_HIDDEN KoUnitDoubleSpinBox::Private
{
public:
    Private(double low, double up, double step)
        : lowerInPoints(low),
        upperInPoints(up),
        stepInPoints(step),
        unit(KoUnit(KoUnit::Point))
    {
    }

    double lowerInPoints;
    double upperInPoints;
    double stepInPoints;
    KoUnit unit;
};

KoUnitDoubleSpinBox::KoUnitDoubleSpinBox( QWidget *parent)
    : QDoubleSpinBox( parent ),
    d( new Private(-9999, 9999, 1))
{
    QDoubleSpinBox::setDecimals( 2 );
    setUnit( KoUnit(KoUnit::Point) );
    setAlignment( Qt::AlignRight );

    connect(this, SIGNAL(valueChanged(double)), SLOT(privateValueChanged()));
}

KoUnitDoubleSpinBox::~KoUnitDoubleSpinBox()
{
    delete d;
}

QValidator::State KoUnitDoubleSpinBox::validate(QString &input, int &pos) const
{
#ifdef DEBUG_VALIDATOR
    debugWidgets <<"KoUnitDoubleSpinBox::validate :" << input <<" at" << pos;
#else
    Q_UNUSED(pos);
#endif

    QRegularExpression regexp ("([ a-zA-Z]+)$");
    QRegularExpressionMatch match;
    const int res = input.indexOf(regexp, 0, &match);

    if ( res == -1 )
    {
#ifdef DEBUG_VALIDATOR
        debugWidgets <<"Intermediate (no unit)";
#endif
        return QValidator::Intermediate;
    }

    const QString number ( input.left( res ).trimmed() );
    const QString unitName ( match.captured( 1 ).trimmed().toLower() );

#ifdef DEBUG_VALIDATOR
    debugWidgets <<"Split:" << number <<":" << unitName <<":";
#endif

    const double value = valueFromText( number );
    double newVal = 0.0;
    if (!qIsNaN(value)) {
        bool ok;
        const KoUnit unit = KoUnit::fromSymbol(unitName, &ok);
        if ( ok )
            newVal = unit.fromUserValue( value );
        else
        {
#ifdef DEBUG_VALIDATOR
            debugWidgets <<"Intermediate (unknown unit)";
#endif
            return QValidator::Intermediate;
        }
    }
    else
    {
        warnWidgets << "Not a number: " << number;
        return QValidator::Invalid;
    }
    newVal = d->unit.toUserValuePrecise(newVal);

    return QValidator::Acceptable;
}

void KoUnitDoubleSpinBox::changeValue( double val )
{
    QDoubleSpinBox::setValue( d->unit.toUserValue( val ) );
}

void KoUnitDoubleSpinBox::privateValueChanged() {
    Q_EMIT valueChangedPt( value () );
}

void KoUnitDoubleSpinBox::setUnit( const KoUnit &unit )
{
    if (unit == d->unit) return;

    double oldvalue = d->unit.fromUserValue( QDoubleSpinBox::value() );
    QDoubleSpinBox::setMinimum( unit.toUserValue( d->lowerInPoints ) );
    QDoubleSpinBox::setMaximum( unit.toUserValue( d->upperInPoints ) );

    qreal step = unit.toUserValue( d->stepInPoints );

    if (unit.type() == KoUnit::Pixel) {
        step = qMax(qreal(1.0), step);
    }

    QDoubleSpinBox::setSingleStep( step );
    d->unit = unit;
    QDoubleSpinBox::setValue(unit.toUserValuePrecise(oldvalue));
    setSuffix(unit.symbol().prepend(QLatin1Char(' ')));
}

double KoUnitDoubleSpinBox::value( ) const
{
    return d->unit.fromUserValue( QDoubleSpinBox::value() );
}

void KoUnitDoubleSpinBox::setMinimum( double min )
{
  d->lowerInPoints = min;
  QDoubleSpinBox::setMinimum( d->unit.toUserValue( min ) );
}

void KoUnitDoubleSpinBox::setMaximum( double max )
{
  d->upperInPoints = max;
  QDoubleSpinBox::setMaximum( d->unit.toUserValue( max ) );
}

void KoUnitDoubleSpinBox::setLineStep( double step )
{
  d->stepInPoints = KoUnit(KoUnit::Point).toUserValue(step);
  QDoubleSpinBox::setSingleStep( step );
}

void KoUnitDoubleSpinBox::setLineStepPt( double step )
{
  d->stepInPoints = step;
  QDoubleSpinBox::setSingleStep( d->unit.toUserValue( step ) );
}

void KoUnitDoubleSpinBox::setMinMaxStep( double min, double max, double step )
{
  setMinimum( min );
  setMaximum( max );
  setLineStepPt( step );
}

QString KoUnitDoubleSpinBox::textFromValue( double value ) const
{
    return QLocale().toString( value, 'f', decimals() );
}

double KoUnitDoubleSpinBox::valueFromText( const QString& str ) const
{
    QString str2( str );
    str2.remove(d->unit.symbol());
    return QLocale().toDouble(str2);
}
