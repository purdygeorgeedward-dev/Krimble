/*
 *  SPDX-FileCopyrightText: 2017 Laurent Valentin Jospin <laurent.valentin@famillejospin.ch>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISSPINBOXUNITMANAGER_H
#define KISSPINBOXUNITMANAGER_H

#include <QObject>
#include <QStringList>
#include <QAbstractListModel>

#include "kritawidgetutils_export.h"

class KisSpinBoxUnitManager;
class KisSpinBoxUnitManagerBuilder;
class KisSpinBoxUnitManagerFactory;

class KRITAWIDGETUTILS_EXPORT KisSpinBoxUnitManagerFactory
{
public:

    static KisSpinBoxUnitManager* buildDefaultUnitManager(QObject* parent);
    static void setDefaultUnitManagerBuilder(KisSpinBoxUnitManagerBuilder* pBuilder);
    static void clearUnitManagerBuilder();

private:

    static KisSpinBoxUnitManagerBuilder* builder;

};

class KRITAWIDGETUTILS_EXPORT KisSpinBoxUnitManagerBuilder
{

public:

    virtual ~KisSpinBoxUnitManagerBuilder() {}

    virtual KisSpinBoxUnitManager* buildUnitManager(QObject* parent) = 0;
};

class KRITAWIDGETUTILS_EXPORT KisSpinBoxUnitManager : public  QAbstractListModel
{
    Q_OBJECT

public:

    enum UnitDimension{
        LENGTH = 0,
        IMLENGTH = 1,
        ANGLE = 2,
        TIME = 3
    };

    static inline bool isUnitId(int code) { return (code == LENGTH || code == ANGLE || code == TIME); }

    static const QStringList referenceUnitSymbols;

    enum Constrain{
        NOCONSTR = 0,
        REFISINT = 1,
        VALISINT = 2

    };

    Q_DECLARE_FLAGS(Constrains, Constrain)

    explicit KisSpinBoxUnitManager(QObject *parent = 0);
    ~KisSpinBoxUnitManager() override;

    int getUnitDimensionType() const;
    QString getReferenceUnitSymbol() const;
    QString getApparentUnitSymbol() const;

    int getApparentUnitId() const;

    int getApparentUnitRecommendedDecimals() const;

    virtual QStringList getsUnitSymbolList(bool withName = false) const;

    qreal getReferenceValue(double apparentValue) const;
    qreal getApparentValue(double refValue) const;

    virtual qreal getConversionFactor(int dim, QString symbol) const;
    virtual qreal getConversionConstant(int dim, QString symbol) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

Q_SIGNALS:

    void unitDimensionChanged(int dimCode);
    void unitAboutToChange();
    void unitChanged(QString symbol);
    void unitChanged(int index);
    void conversionFactorChanged(qreal newConversionFactor, qreal oldConversionFactor) const;
    void conversionConstantChanged(qreal newConversionFactor, qreal oldConversionFactor) const;
    void unitListChanged();

public Q_SLOTS:

    void setUnitDimension(UnitDimension dimension);
    void setApparentUnitFromSymbol(QString pSymbol);
    void selectApparentUnitFromIndex(int index);

    void syncWithOtherUnitManager(KisSpinBoxUnitManager* other);
    void clearSyncWithOtherUnitManager(KisSpinBoxUnitManager* other);

protected:

    class Private;
    Private * d;

    void newUnitSymbolToUnitIndex(QString symbol);

    virtual bool hasPercent(int unitDim) const;

    static const QStringList documentRelativeLengthUnitSymbols;
    static const QStringList documentRelativeTimeUnitSymbols;

    void recomputeConversionFactor() const;
    void recomputeConversionConstant() const;

    void grantDocumentRelativeUnits();

};

#endif
