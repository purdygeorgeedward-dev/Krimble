/*
 *  SPDX-FileCopyrightText: 2016 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 *  SPDX-FileCopyrightText: 2021 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_double_parse_unit_spin_box.h"
#include "kis_spin_box_unit_manager.h"
#include <klocalizedstring.h>

#include <QLineEdit>
#include <QMenu>
#include <QAction>
#include <QtMath>
#include <QRegularExpression>
#include <QString>
#include <QActionGroup>

class Q_DECL_HIDDEN KisDoubleParseUnitSpinBox::Private
{
public:
    Private(double low, double up, double step, KisSpinBoxUnitManager* unitManager)
        : lowerInPoints(low),
          upperInPoints(up),
          stepInPoints(step),
          unit(KoUnit(KoUnit::Point)),
          outPutSymbol(""),
          unitManager(unitManager),
          defaultUnitManager(unitManager)
    {
    }

    double minStepForPrec {0};
    double lowerInPoints {0.0};
    double upperInPoints {0.0};
    double stepInPoints {0.0};
    KoUnit unit;

    double previousValueInPoint {0.0};
    QString previousSymbol;
    QString outPutSymbol;

    KisSpinBoxUnitManager* unitManager {0};
    KisSpinBoxUnitManager* defaultUnitManager {0};

    bool isDeleting {false};

    bool unitHasBeenChangedFromOutSideOnce {false};
    bool letUnitBeChangedFromOutsideMoreThanOnce {true};

    bool displayUnit {true};

    bool allowResetDecimals {true};

    bool mustUsePreviousText {false};
};

KisDoubleParseUnitSpinBox::KisDoubleParseUnitSpinBox(QWidget *parent) :
    KisDoubleParseSpinBox(parent),
    d(new Private(-9999, 9999, 1, KisSpinBoxUnitManagerFactory::buildDefaultUnitManager(this)))
{
    setUnit( KoUnit(KoUnit::Point) );
    setAlignment( Qt::AlignRight );

    connect(this, SIGNAL(valueChanged(double)), this, SLOT(privateValueChanged()));
    connect(lineEdit(), SIGNAL(textChanged(QString)),
            this, SLOT(detectUnitChanges()) );

    connect(d->unitManager, (void (KisSpinBoxUnitManager::*)()) &KisSpinBoxUnitManager::unitAboutToChange, this, (void (KisDoubleParseUnitSpinBox::*)()) &KisDoubleParseUnitSpinBox::prepareUnitChange);
    connect(d->unitManager, (void (KisSpinBoxUnitManager::*)( QString )) &KisSpinBoxUnitManager::unitChanged, this, (void (KisDoubleParseUnitSpinBox::*)( QString const& )) &KisDoubleParseUnitSpinBox::internalUnitChange);

    setDecimals(d->unitManager->getApparentUnitRecommendedDecimals());
}

KisDoubleParseUnitSpinBox::~KisDoubleParseUnitSpinBox()
{
    d->isDeleting = true;
    delete d->defaultUnitManager;
    delete d;
}

void KisDoubleParseUnitSpinBox::setUnitManager(KisSpinBoxUnitManager* unitManager)
{
    if (unitManager == d->unitManager) {
        return;
    }

    KisSpinBoxUnitManager* oldUnitManager = 0;

    if (d->unitManager) {
        oldUnitManager = d->unitManager;

        disconnect(oldUnitManager, &QObject::destroyed,
                this, &KisDoubleParseUnitSpinBox::disconnectExternalUnitManager);
        disconnect(oldUnitManager, (void (KisSpinBoxUnitManager::*)()) &KisSpinBoxUnitManager::unitAboutToChange,
                this, (void (KisDoubleParseUnitSpinBox::*)()) &KisDoubleParseUnitSpinBox::prepareUnitChange);
        disconnect(oldUnitManager, (void (KisSpinBoxUnitManager::*)( QString )) &KisSpinBoxUnitManager::unitChanged,
                this, (void (KisDoubleParseUnitSpinBox::*)( QString const& )) &KisDoubleParseUnitSpinBox::internalUnitChange);
    }

    d->unitManager = unitManager;

    if (d->allowResetDecimals) {
        setDecimals(d->unitManager->getApparentUnitRecommendedDecimals());
    }

    qreal newVal = 0.0;

    double newMin;
    double newMax;
    double newStep;

    if (oldUnitManager == 0 ||
        (oldUnitManager &&
        (d->unitManager->getApparentUnitSymbol() != oldUnitManager->getApparentUnitSymbol() ||
         d->unitManager->getUnitDimensionType() == oldUnitManager->getUnitDimensionType()))) {

        if (oldUnitManager && d->unitManager->getUnitDimensionType() == oldUnitManager->getUnitDimensionType()) {
            newVal = d->unitManager->getApparentValue(oldUnitManager->getReferenceValue(KisDoubleParseSpinBox::value()));
        } else {
            newVal = d->unitManager->getApparentValue(d->lowerInPoints);
        }

        newMin = d->unitManager->getApparentValue(d->lowerInPoints);
        newMax = d->unitManager->getApparentValue(d->upperInPoints);
        newStep = d->unitManager->getApparentValue(d->stepInPoints);

        if (d->unitManager->getApparentUnitSymbol() == KoUnit(KoUnit::Pixel).symbol()) {
            newStep = 1.0;
        }

        KisDoubleParseSpinBox::setMinimum(newMin);
        KisDoubleParseSpinBox::setMaximum(newMax);
        KisDoubleParseSpinBox::setSingleStep(newStep);
    }

    connect(d->unitManager, &QObject::destroyed,
            this, &KisDoubleParseUnitSpinBox::disconnectExternalUnitManager);
    connect(d->unitManager, (void (KisSpinBoxUnitManager::*)()) &KisSpinBoxUnitManager::unitAboutToChange,
            this, (void (KisDoubleParseUnitSpinBox::*)()) &KisDoubleParseUnitSpinBox::prepareUnitChange);
    connect(d->unitManager, (void (KisSpinBoxUnitManager::*)( QString )) &KisSpinBoxUnitManager::unitChanged,
            this, (void (KisDoubleParseUnitSpinBox::*)( QString const& )) &KisDoubleParseUnitSpinBox::internalUnitChange);

    KisDoubleParseSpinBox::setValue(newVal);
}

void KisDoubleParseUnitSpinBox::changeValue( double newValue )
{
    double apparentValue;
    double fact = 0.0;
    double cons = 0.0;

    if (d->outPutSymbol.isEmpty()) {
        apparentValue = d->unitManager->getApparentValue(newValue);
    } else {

        fact = d->unitManager->getConversionFactor(d->unitManager->getUnitDimensionType(), d->outPutSymbol);
        cons = d->unitManager->getConversionConstant(d->unitManager->getUnitDimensionType(), d->outPutSymbol);

        apparentValue = fact*newValue + cons;
    }

    if (apparentValue == KisDoubleParseSpinBox::value()) {
        return;
    }

    if (d->outPutSymbol.isEmpty()) {
        KisDoubleParseSpinBox::setValue( apparentValue );
    } else {
        KisDoubleParseSpinBox::setValue( d->unitManager->getApparentValue((newValue - cons)/fact) );
    }
}

void KisDoubleParseUnitSpinBox::changeValuePt( double newValue )
{
    double apparentValue = d->unitManager->getApparentValue(newValue);

    if (apparentValue == KisDoubleParseSpinBox::value()) {
        return;
    }
    KisDoubleParseSpinBox::setValue( apparentValue );
}

void KisDoubleParseUnitSpinBox::setUnit( const KoUnit & unit)
{
    if (d->unitHasBeenChangedFromOutSideOnce && !d->letUnitBeChangedFromOutsideMoreThanOnce) {
        return;
    }

    if (d->unitManager->getUnitDimensionType() != KisSpinBoxUnitManager::LENGTH) {
        d->unitManager->setUnitDimension(KisSpinBoxUnitManager::LENGTH);
    }

    setUnit(unit.symbol());
    d->unit = unit;
}

void KisDoubleParseUnitSpinBox::setUnit(const QString &symbol)
{
    d->unitManager->setApparentUnitFromSymbol(symbol);
}

void KisDoubleParseUnitSpinBox::setReturnUnit(const QString & symbol)
{
    d->outPutSymbol = symbol;
}

QString KisDoubleParseUnitSpinBox::returnUnit() const
{
    return d->outPutSymbol;
}

void KisDoubleParseUnitSpinBox::prepareUnitChange() {

    d->previousValueInPoint = d->unitManager->getReferenceValue(KisDoubleParseSpinBox::value());
    d->previousSymbol = d->unitManager->getApparentUnitSymbol();
}

void KisDoubleParseUnitSpinBox::internalUnitChange(const QString &symbol) {

    if (d->unitManager->getApparentUnitSymbol() == d->previousSymbol) {
        return;
    }

    if (d->allowResetDecimals) {
        setDecimals(d->unitManager->getApparentUnitRecommendedDecimals());
    }

    KisDoubleParseSpinBox::setMinimum( d->unitManager->getApparentValue( d->lowerInPoints ) );
    KisDoubleParseSpinBox::setMaximum( d->unitManager->getApparentValue( d->upperInPoints ) );

    qreal step = d->unitManager->getApparentValue( d->stepInPoints );

    if (symbol == KoUnit(KoUnit::Pixel).symbol()) {
        step = 1.0;
    }

    setSingleStep( step );
    KisDoubleParseSpinBox::setValue( d->unitManager->getApparentValue( d->previousValueInPoint ) );

    d->unitHasBeenChangedFromOutSideOnce = true;
}

void KisDoubleParseUnitSpinBox::setDimensionType(int dim)
{
    if (!KisSpinBoxUnitManager::isUnitId(dim)) {
        return;
    }

    d->unitManager->setUnitDimension((KisSpinBoxUnitManager::UnitDimension) dim);
}

double KisDoubleParseUnitSpinBox::value( ) const
{
    if (d->outPutSymbol.isEmpty()) {
        return d->unitManager->getReferenceValue( KisDoubleParseSpinBox::value() );
    }

    double ref = d->unitManager->getReferenceValue( KisDoubleParseSpinBox::value() );
    double fact = d->unitManager->getConversionFactor(d->unitManager->getUnitDimensionType(), d->outPutSymbol);
    double cons = d->unitManager->getConversionConstant(d->unitManager->getUnitDimensionType(), d->outPutSymbol);

    return fact*ref + cons;
}

double KisDoubleParseUnitSpinBox::valuePt( ) const
{
    return d->unitManager->getReferenceValue( KisDoubleParseSpinBox::value() );
}

void KisDoubleParseUnitSpinBox::setValuePt(double value, bool overwriteExpression)
{
    KisDoubleParseSpinBox::setValue(d->unitManager->getApparentValue(value), overwriteExpression);
}

void KisDoubleParseUnitSpinBox::setMinimum(double min)
{
    d->lowerInPoints = d->unitManager->getReferenceValue(min);
    KisDoubleParseSpinBox::setMinimum( min );
}

void KisDoubleParseUnitSpinBox::setMinimumPt(double min)
{
    d->lowerInPoints = min;
    KisDoubleParseSpinBox::setMinimum( d->unitManager->getApparentValue( min ) );
}

void KisDoubleParseUnitSpinBox::setMaximum(double max)
{
    d->upperInPoints = d->unitManager->getReferenceValue(max);
    KisDoubleParseSpinBox::setMaximum( max );
}

void KisDoubleParseUnitSpinBox::setMaximumPt(double max)
{
    d->upperInPoints = max;
    KisDoubleParseSpinBox::setMaximum( d->unitManager->getApparentValue( max ) );
}

void KisDoubleParseUnitSpinBox::setLineStep(double step)
{
    d->stepInPoints = d->unitManager->getReferenceValue(step);
    KisDoubleParseSpinBox::setSingleStep( step );
}

void KisDoubleParseUnitSpinBox::setLineStepPt(double step)
{
    d->stepInPoints = step;
    KisDoubleParseSpinBox::setSingleStep( d->unitManager->getApparentValue( step ) );
}

void KisDoubleParseUnitSpinBox::setMinMaxStep( double min, double max, double step )
{
    setMinimum( min );
    setMaximum( max );
    setLineStep( step );
}

void KisDoubleParseUnitSpinBox::setMinMaxStepPt( double min, double max, double step )
{
    setMinimumPt( min );
    setMaximumPt( max );
    setLineStepPt( step );
}

QString KisDoubleParseUnitSpinBox::textFromValue( double value ) const
{
    if (d->mustUsePreviousText) {
        return cleanText();
    }
    QString txt = KisDoubleParseSpinBox::textFromValue(value);
    if (d->displayUnit) {
        if (!txt.endsWith(d->unitManager->getApparentUnitSymbol())) {
            txt += " " + d->unitManager->getApparentUnitSymbol();
        }
    }
    return txt;
}

QString KisDoubleParseUnitSpinBox::veryCleanText() const
{
    return makeTextClean(cleanText());
}

double KisDoubleParseUnitSpinBox::valueFromText( const QString& str ) const
{
    QString txt = makeTextClean(str);
    return KisDoubleParseSpinBox::valueFromText(txt);
}

void KisDoubleParseUnitSpinBox::setUnitChangeFromOutsideBehavior(bool toggle)
{
    d->letUnitBeChangedFromOutsideMoreThanOnce = toggle;
}

void KisDoubleParseUnitSpinBox::setDisplayUnit(bool toggle)
{
    d->displayUnit = toggle;
}

void KisDoubleParseUnitSpinBox::preventDecimalsChangeFromUnitManager(bool prevent)
{
    d->allowResetDecimals = !prevent;
}

void KisDoubleParseUnitSpinBox::privateValueChanged()
{
    Q_EMIT valueChangedPt( value() );
}

QString KisDoubleParseUnitSpinBox::detectUnit()
{
    QString str = veryCleanText().trimmed();

    QRegularExpression regexp ("([ ]*[a-zA-Z]+[ ]*)$");
    int res = str.indexOf( regexp );

    if (res > -1) {
        QString expr ( str.right( str.size() - res ) );
        expr = expr.trimmed();
        return expr;
    }

    return "";
}

void KisDoubleParseUnitSpinBox::detectUnitChanges()
{
    QString unitSymb = detectUnit();

    if (unitSymb.isEmpty()) {
        return;
    }

    QString oldUnitSymb = d->unitManager->getApparentUnitSymbol();

    setUnit(unitSymb);
    d->mustUsePreviousText = true;
    setValue(valueFromText(cleanText()));
    d->mustUsePreviousText = false;

    if (oldUnitSymb != d->unitManager->getApparentUnitSymbol()) {
        setUnitChangeFromOutsideBehavior(false);
    }
}

QString KisDoubleParseUnitSpinBox::makeTextClean(QString const& txt) const
{
    QString expr = txt;
    QString symbol = d->unitManager->getApparentUnitSymbol();

    if ( expr.endsWith(suffix()) ) {
        expr.remove(expr.size()-suffix().size(), suffix().size());
    }

    expr = expr.trimmed();

    if ( expr.endsWith(symbol) ) {
        expr.remove(expr.size()-symbol.size(), symbol.size());
    }

    return expr.trimmed();
}

void KisDoubleParseUnitSpinBox::disconnectExternalUnitManager()
{
    d->unitManager = 0;

    if (!d->isDeleting)
    {
        setUnitManager(d->defaultUnitManager);
    }
}

void KisDoubleParseUnitSpinBox::setDecimals(int prec)
{
    KisDoubleParseSpinBox::setDecimals(prec);
    d->minStepForPrec = 1/qPow(10, prec);

    setSingleStep(singleStep());
}

void KisDoubleParseUnitSpinBox::setSingleStep(double val)
{
    KisDoubleParseSpinBox::setSingleStep(qMax(d->minStepForPrec, val));
}

void KisDoubleParseUnitSpinBox::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu* menu = lineEdit()->createStandardContextMenu();
    if (!menu)
        return;

    menu->addSeparator();
    const uint se = stepEnabled();
    QAction *up = menu->addAction(tr("&Step up"));
    up->setEnabled(se & StepUpEnabled);
    QAction *down = menu->addAction(tr("Step &down"));
    down->setEnabled(se & StepDownEnabled);
    menu->addSeparator();

    QMenu* menuUnit = menu->addMenu(i18n("Unit"));
    QActionGroup* unitActions = new QActionGroup(this);
    Q_FOREACH(QString unitSymbol, d->unitManager->getsUnitSymbolList(false)) {
        QString unitLabel = KoUnit::unitDescription(KoUnit::fromSymbol(unitSymbol).type());

        switch (d->unitManager->getUnitDimensionType()) {
            case KisSpinBoxUnitManager::UnitDimension::LENGTH:
            case KisSpinBoxUnitManager::UnitDimension::IMLENGTH:
                if (unitSymbol == "%") {
                    unitLabel = i18n("Percent (%)");
                } else if (unitSymbol == "vw") {
                    unitLabel = i18n("percent of view width (vw)");
                } else if (unitSymbol == "vh") {
                    unitLabel = i18n("percent of view height (vh)");
                }
                break;

            case KisSpinBoxUnitManager::ANGLE:
                if (unitSymbol == "°") {
                    unitLabel = i18n("degrees (°)");
                } else if (unitSymbol == "rad") {
                    unitLabel = i18n("radians (rad)");
                } else if (unitSymbol == "gon") {
                    unitLabel = i18n("gons (gon)");
                } else if (unitSymbol == "%") {
                    unitLabel = i18n("percent of circle (%)");
                }
                break;

            case KisSpinBoxUnitManager::TIME:
                if (unitSymbol == "f") {
                    unitLabel = i18n("frames (f)");
                } else if (unitSymbol == "s") {
                    unitLabel = i18n("seconds (s)");
                } else if (unitSymbol == "%") {
                    unitLabel = i18n("percent of animation (%)");
                }
                break;
        }

        QAction *unitAction = menuUnit->addAction(unitLabel);
        unitAction->setProperty("symbol", unitSymbol);
        unitAction->setCheckable(true);
        unitAction->setActionGroup(unitActions);
        unitAction->setChecked(unitSymbol == d->unitManager->getApparentUnitSymbol());
    }

    const QPoint pos = (event->reason() == QContextMenuEvent::Mouse)
        ? event->globalPos() : mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(width() / 2, height() / 2);
    const QAction *action = menu->exec(pos);

    if (action) {
        if (action == up) {
            stepBy(1);
        } else if (action == down) {
            stepBy(-1);
        } else {
            QVariant symbol = action->property("symbol");
            if (symbol.isValid()) {
                d->unitManager->setApparentUnitFromSymbol(symbol.toString());
            }
        }
    }

    delete static_cast<QMenu *>(menuUnit);
    delete static_cast<QMenu *>(menu);
    event->accept();
}

