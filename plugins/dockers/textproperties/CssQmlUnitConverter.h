/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef CSSQMLUNITCONVERTER_H
#define CSSQMLUNITCONVERTER_H

#include <QObject>
#include <QVariant>

class KoSvgTextPropertiesModel;
class CssQmlUnitConverter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal dpi READ dpi WRITE setDpi NOTIFY dpiChanged)

    Q_PROPERTY(qreal dataMultiplier READ dataMultiplier WRITE setDataMultiplier NOTIFY dataMultiplierChanged)

    Q_PROPERTY(qreal dataValue READ dataValue WRITE setDataValue NOTIFY dataValueChanged)
    Q_PROPERTY(int dataUnit READ dataUnit WRITE setDataUnit NOTIFY dataUnitChanged)

    Q_PROPERTY(qreal userValue READ userValue WRITE setUserValue NOTIFY userValueChanged)
    Q_PROPERTY(int userUnit READ userUnit WRITE setUserUnit NOTIFY userUnitChanged)

    Q_PROPERTY(QVariantList userUnitModel READ userUnitModel NOTIFY userUnitModelChanged)

    Q_PROPERTY(QString symbol READ symbol NOTIFY userUnitChanged)

    Q_PROPERTY(qreal percentageReference READ percentageReference WRITE setPercentageReference NOTIFY percentageReferenceChanged)

public:
    CssQmlUnitConverter(QObject *parent = nullptr);

    ~CssQmlUnitConverter();

    enum UserUnits {
        Px,
        Pt,
        Mm,
        Cm,
        Inch,
        Percentage,
        Em,
        Ex,
        Cap,
        Ch,
        Ic,
        Lh,
        Lines,
        Spaces,
    };

    Q_ENUM(UserUnits)

    Q_INVOKABLE void setDataUnitMap(const QVariantList &unitMap);

    Q_INVOKABLE void setFontMetricsFromTextPropertiesModel(KoSvgTextPropertiesModel *textPropertiesModel, bool isFontSize = false, bool isLineHeight = false);

    Q_INVOKABLE void setFromNormalLineHeight();

    Q_INVOKABLE void setDataValueAndUnit(const qreal value, const int unit);

    qreal dpi() const;
    void setDpi(qreal newDpi);

    qreal dataMultiplier() const;
    void setDataMultiplier(qreal newDataMultiplier);

    qreal dataValue() const;
    void setDataValue(qreal newDataValue);

    int dataUnit() const;
    void setDataUnit(int newDataUnit);

    qreal userValue() const;
    void setUserValue(qreal newUserValue);

    int userUnit() const;
    void setUserUnit(int newUserUnit);

    QVariantList userUnitModel() const;

    QString symbol() const;

    qreal percentageReference() const;
    void setPercentageReference(qreal newPercentageReference);

Q_SIGNALS:

    void dpiChanged();
    void dataMultiplierChanged();

    void dataValueChanged();

    void dataUnitChanged();

    void userValueChanged();

    void userUnitChanged();

    void userUnitModelChanged();

    void percentageReferenceChanged();

private:
    qreal convertFromRelativeValue(const qreal value, const UserUnits type) const;
    qreal convertToRelativeValue(const qreal value, const UserUnits type)const ;

    struct Private;
    const QScopedPointer<Private> d;
};

#endif
