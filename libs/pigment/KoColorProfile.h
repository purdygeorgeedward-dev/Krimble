/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef _KO_COLOR_PROFILE_H_
#define _KO_COLOR_PROFILE_H_

#include <boost/operators.hpp>
#include <QString>
#include <QVector>
#include <QVariant>

#include "KoColorProfileConstants.h"
#include "kritapigment_export.h"
#include <KoColorimetryUtils.h>
#include <optional>

class KRITAPIGMENT_EXPORT KoColorProfile : public boost::equality_comparable<KoColorProfile>
{

public:

    explicit KoColorProfile(const QString &fileName = QString());
    KoColorProfile(const KoColorProfile& profile);
    virtual ~KoColorProfile();

    virtual QString type() const {
        return QString();
    }

    virtual KoColorProfile* clone() const = 0;

    virtual bool load();

    virtual bool save(const QString &fileName);

    virtual bool valid() const = 0;

    QString name() const;
    QString info() const;
    QString manufacturer() const;
    QString copyright() const;
    QString fileName() const;
    void setFileName(const QString &filename);

    virtual float version() const = 0;

    virtual QString colorModelID() const {
        return QString();
    };
    virtual bool isSuitableForOutput() const = 0;
    virtual bool isSuitableForInput() const = 0;
    virtual bool isSuitableForWorkspace() const = 0;
    virtual bool isSuitableForPrinting() const = 0;
    virtual bool isSuitableForDisplay() const = 0;

    virtual bool supportsPerceptual() const = 0;
    virtual bool supportsSaturation() const = 0;
    virtual bool supportsAbsolute() const = 0;
    virtual bool supportsRelative() const = 0;
    virtual bool hasColorants() const = 0;
    virtual QVector <KoColorimetryUtils::XYZ> getColorantsXYZ() const = 0;
    virtual QVector <KoColorimetryUtils::xyY> getColorantsxyY() const = 0;
    virtual KoColorimetryUtils::XYZ getWhitePointXYZ() const = 0;
    virtual KoColorimetryUtils::xyY getWhitePointxyY() const = 0;

    virtual std::optional<double> hdrReferenceWhite() const = 0;

    virtual QVector <qreal> getEstimatedTRC() const = 0;

    virtual bool hasTRC() const = 0;
    virtual bool isLinear() const = 0;
    virtual void linearizeFloatValue(QVector <qreal> & Value) const = 0;
    virtual void delinearizeFloatValue(QVector <qreal> & Value) const = 0;
    virtual void linearizeFloatValueFast(QVector <qreal> & Value) const = 0;
    virtual void delinearizeFloatValueFast(QVector <qreal> & Value) const = 0;

    virtual bool compareTRC(TransferCharacteristics characteristics, float error) const = 0;

    virtual QByteArray uniqueId() const = 0;

    virtual bool operator==(const KoColorProfile&) const = 0;

    virtual QByteArray rawData() const {
        return QByteArray();
    }

    virtual ColorPrimaries getColorPrimaries() const;

    static QString getColorPrimariesName(ColorPrimaries primaries);
    static void colorantsForType(ColorPrimaries primaries, KoColorimetryUtils::xy &whitePoint, QList<KoColorimetryUtils::xy> &colorants, const bool prequantized = false);

    virtual TransferCharacteristics getTransferCharacteristics() const;

    static QString getTransferCharacteristicName(TransferCharacteristics curve);

protected:
    void setName(const QString &name);
    void setInfo(const QString &info);
    void setManufacturer(const QString &manufacturer);
    void setCopyright(const QString &copyright);

    void setCharacteristics(ColorPrimaries primaries, TransferCharacteristics curve);

private:
    struct Private;
    Private* const d;
};

#endif
