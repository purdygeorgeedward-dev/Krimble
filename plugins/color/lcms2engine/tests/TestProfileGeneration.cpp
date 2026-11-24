/*
 *  SPDX-FileCopyrightText: 2021 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TestProfileGeneration.h"
#include "colorprofiles/LcmsColorProfileContainer.h"
#include "colorprofiles/IccColorProfile.h"

#include <QTest>
#include <cmath>
#include <lcms2.h>

#include <KoColorConversions.h>
#include <KoColorTransferFunctions.h>
#include <KoColorProfileQuery.h>
#include <kis_debug.h>
#include <KoColorSpaceEngine.h>
#include <KoColorProfile.h>

#include <testpigment.h>

void TestProfileGeneration::testTransferFunctions()
{

    cmsFloat64Number srgb_parameters[5] =
    { 2.4, 1.0 / 1.055,  0.055 / 1.055, 1.0 / 12.92, 0.04045 };
    cmsFloat64Number rec709_parameters[5] =
    { 1.0 / 0.45, 1.0 / 1.099,  0.099 / 1.099,  1.0 / 4.5, 0.081 };

    cmsFloat64Number SMPTE_240M_parameters[5] =
    { 1.0 / 0.45, 1.0 / 1.1115,  0.1115 / 1.1115,  1.0 / 4.0, 0.0913 };

    cmsFloat64Number prophoto_parameters[5] =
    { 1.8, 1.0,  0, 1.0 / 16, (16/512) };

    cmsFloat64Number log_100[5] = {1.0, 10, 2.0, -2.0, 0.0};
    cmsFloat64Number log_100_sqrt[5] = {1.0, 10, 2.5, -2.5, 0.0};

    QVector<cmsFloat32Number> inputValues = {0};
    QVector<double> inputValuesRaw = {0};
    for (int i = 0; i < 60; i++) {
        inputValues.append(    .01 * (i+1) );
        inputValuesRaw.append( .01 * (i+1) );
    }

    QVector<double> col;
    cmsToneCurve *curve  = cmsBuildParametricToneCurve(NULL, 4, rec709_parameters);

    for (cmsFloat32Number value : inputValues) {
        cmsFloat32Number cValue = value;

        if (value > 0.018){
            cValue = 1.099 * powf(value, 0.45) - (.099) ;
        } else if (value < 0.018 && value > 0.0){
            cValue = 4.5 * value;
        } else {
            cValue = 0.0;
        }

        cmsFloat32Number lValue = cmsEvalToneCurveFloat(curve, cValue);
        QVERIFY2(fabs(lValue - value) < 0.000001, QString("Values don't match for rec 709: %1 %2").arg(value).arg(lValue).toLatin1());
    }

    curve = cmsBuildParametricToneCurve(NULL, 4, srgb_parameters);

    for (cmsFloat32Number value : inputValues) {
        cmsFloat32Number cValue = value;

        if (value > 0.0031308){
            cValue = 1.055 * powf(value, 1/2.4) - (.055) ;
        } else if (value < 0.0031308 && value > 0.0){
            cValue = 12.92 * value;
        } else {
            cValue = 0.0;
        }

        cmsFloat32Number lValue = cmsEvalToneCurveFloat(curve, cValue);
        QVERIFY2(fabs(lValue - value) < 0.000001, QString("Values don't match for sRGB: %1 %2").arg(value).arg(lValue).toLatin1());
    }

    curve = cmsBuildParametricToneCurve(NULL, 4, SMPTE_240M_parameters);

    for (cmsFloat32Number value : inputValues) {
        cmsFloat32Number cValue = value;

        if (value > 0.0228){
            cValue = 1.1115 * powf(value, 0.45) - (.1115) ;
        } else if (value < 0.0228 && value > 0.0){
            cValue = 4.0 * value;
        } else {
            cValue = 0.0;
        }

        cmsFloat32Number lValue = cmsEvalToneCurveFloat(curve, cValue);
        QVERIFY2(fabs(lValue - value) < 0.000001, QString("Values don't match for SMPTE 240M: %1 %2").arg(value).arg(lValue).toLatin1());
    }

    curve = cmsBuildParametricToneCurve(NULL, 4, prophoto_parameters);

    for (cmsFloat32Number value : inputValues) {
        cmsFloat32Number cValue = value;

        if (value > 1.0/512.0){
            cValue = powf(value, 1/1.8) ;
        } else if (value < 0.0228 && value > 0.0){
            cValue = 16.0 * value;
        }

        cmsFloat32Number lValue = cmsEvalToneCurveFloat(curve, cValue);
        QVERIFY2(fabs(lValue - value) < 0.000001, QString("Values don't match for ProPhoto: %1 %2").arg(value).arg(lValue).toLatin1());
    }

    for (double value : inputValuesRaw) {
        double cValue = value;

        if (value > 0.018){
            cValue = 1.099 * powf(value, 0.45) - (.099) ;
        } else if (value < 0.018 && value > -0.018){
            cValue = 4.5 * value;
        } else {
            cValue = -1.099 * powf(-value, 0.45) - (-.099) ;
        }

        double lValue;
        if (cValue > 0.018){
            lValue = powf((.099+cValue)*1/1.099, 1/ 0.45);
        } else if (cValue < -0.018) {
             lValue = powf( (cValue/-1.099) + (-.099/-1.099), 1/ 0.45) * -1;
        } else {
            lValue = cValue * 1/4.5;
        }

        QVERIFY2(fabs(lValue - value) < 0.001, QString("Values don't match for IEC 61966-2-4: %1 %2").arg(value).arg(lValue).toLatin1());
    }

    for (double value : inputValuesRaw) {
        double cValue = value;

        if (value > 0.018){
            cValue = 1.099 * powf(value, 0.45) - (.099) ;
        } else if (value < 0.018 && value > -0.0045){
            cValue = 4.5 * value;
        } else {
            cValue = -(1.099 * powf(-4 * value, 0.45) - (.099)) * 0.25 ;
        }

        double lValue;
        if (cValue > 0.018){
            lValue = powf((.099/1.099)+(cValue*1/1.099), 1/ 0.45);
        } else if (cValue < -0.0045) {
            lValue = powf( (cValue * 4 / -1.099 ) - (0.099 / -1.099), 1/ 0.45) * -.25;
        } else {
            lValue = cValue * 1/4.5;
        }

        QVERIFY2(fabs(lValue - value) < 0.001, QString("Values don't match for bt. 1361: %1 %2").arg(value).arg(lValue).toLatin1());
    }

    curve = cmsBuildParametricToneCurve(NULL, 8, log_100);

    for (cmsFloat32Number value : inputValues) {
        cmsFloat32Number cValue = value;

        if (value > 0.01){
            cValue = 1.0+log10(value) *.5 ;
        } else{
            cValue = 0.0;
        }

        cmsFloat32Number lValue = cmsEvalToneCurveFloat(curve, cValue);

        if (value > cmsFloat32Number(0.01)) {
            QVERIFY2(fabs(lValue - value) < 0.000001, QString("Values don't match for log 100: %1 %2").arg(value).arg(lValue).toLatin1());
        }
    }

    curve = cmsBuildParametricToneCurve(NULL, 8, log_100_sqrt);

    for (cmsFloat32Number value : inputValues) {
        cmsFloat32Number cValue = value;

        if (value > (sqrt(10)/1000)){
            cValue = 1.0+log10(value) * (1/2.5) ;
        } else{
            cValue = 0.0;
        }

        cmsFloat32Number lValue = cmsEvalToneCurveFloat(curve, cValue);
        if (value > cmsFloat32Number(sqrt(10)/1000)) {
            QVERIFY2(fabs(lValue - value) < 0.000001, QString("Values don't match for log 100 sqrt: %1 %2").arg(value).arg(lValue).toLatin1());
        }

    }

    for (double value : inputValuesRaw) {
        double cValue = value;

        cValue = applySMPTE_ST_428Curve(value);

        double lValue = (52.37/48.0 * powf( cValue , 2.6 ));

        QVERIFY2(fabs(lValue - value) < 0.000001, QString("Values don't match for SMPTE ST 428 1: %1 %2").arg(value).arg(lValue).toLatin1());

    }

    for (double value : inputValuesRaw) {

        double cValue2 = applyHLGCurve(value);

        double lValue2 = removeHLGCurve(cValue2);

        QVERIFY2(fabs(lValue2 - value) < 0.000001, QString("Values don't match for HLG: %1 %2").arg(value).arg(lValue2).toLatin1());

    }

    cmsFreeToneCurve(curve);

}

Q_DECLARE_METATYPE(ColorPrimaries)
Q_DECLARE_METATYPE(TransferCharacteristics)

void TestProfileGeneration::testCICPwriting_data()
{
    QTest::addColumn<ColorPrimaries>("primaries");
    QTest::addColumn<TransferCharacteristics>("transfer");

    QTest::addRow("rec2020") << PRIMARIES_ITU_R_BT_2020_2_AND_2100_0 << TRC_ITU_R_BT_2020_2_12bit;
    QTest::addRow("rec2100 PQ") << PRIMARIES_ITU_R_BT_2020_2_AND_2100_0 << TRC_ITU_R_BT_2100_0_PQ;
    QTest::addRow("rec709") << PRIMARIES_ITU_R_BT_709_5 << TRC_ITU_R_BT_709_5;
}

void TestProfileGeneration::testCICPwriting()
{
    QFETCH(ColorPrimaries, primaries);
    QFETCH(TransferCharacteristics, transfer);
    KoColorProfileQuery query(primaries, transfer);
    if (transfer == TRC_ITU_R_BT_2100_0_PQ) {
        query.hdrReferenceWhite = std::make_optional(203.0);
    }
    const KoColorProfile *profile = KoColorSpaceRegistry::instance()->profileFor(query);

    QVERIFY(profile);

    QVERIFY(profile->getColorPrimaries() == primaries);
    QVERIFY(profile->getTransferCharacteristics() == transfer);

    const IccColorProfile *icc = dynamic_cast<const IccColorProfile*>(profile);
    QVERIFY(icc);
    const LcmsColorProfileContainer *lcms = icc->asLcms();
    QVERIFY(lcms);

    QVERIFY(lcms->hasCicpValues());
    QVERIFY(lcms->cicpPrimaries() == primaries);
    QVERIFY(lcms->cicpTransfer() == transfer);
}

void TestProfileGeneration::testRetrieveNits_data()
{
    QTest::addColumn<ColorPrimaries>("primaries");
    QTest::addColumn<TransferCharacteristics>("transfer");
    QTest::addColumn<qreal>("requestedReferenceWhite");
    QTest::addColumn<qreal>("expectedReferenceWhite");

    QTest::addRow("rec709-srgbtrc") << PRIMARIES_ITU_R_BT_709_5 << TRC_IEC_61966_2_1 << -1.0 << -1.0;

    QTest::addRow("rec2020-g10") << PRIMARIES_ITU_R_BT_2020_2_AND_2100_0 << TRC_LINEAR << -1.0 << -1.0;
    QTest::addRow("rec709-g10") << PRIMARIES_ITU_R_BT_709_5 << TRC_LINEAR << -1.0 << -1.0;

    QTest::addRow("rec2100 PQ default nits") << PRIMARIES_ITU_R_BT_2020_2_AND_2100_0 << TRC_ITU_R_BT_2100_0_PQ << -1.0 << 203.0;
    QTest::addRow("rec2100 PQ 80 nits") << PRIMARIES_ITU_R_BT_2020_2_AND_2100_0 << TRC_ITU_R_BT_2100_0_PQ << 80.0 << 80.0;
    QTest::addRow("rec2100 PQ 203 nits") << PRIMARIES_ITU_R_BT_2020_2_AND_2100_0 << TRC_ITU_R_BT_2100_0_PQ << 203.0 << 203.0;

    QTest::addRow("display-p3 srgbtrc") << PRIMARIES_SMPTE_EG_432_1 << TRC_IEC_61966_2_1 << -1.0 << -1.0;

    QTest::addRow("display-p3 PQ default nits") << PRIMARIES_SMPTE_EG_432_1 << TRC_ITU_R_BT_2100_0_PQ << -1.0 << 203.0;
    QTest::addRow("display-p3 PQ 80 nits") << PRIMARIES_SMPTE_EG_432_1 << TRC_ITU_R_BT_2100_0_PQ << 80.0 << 80.0;
    QTest::addRow("display-p3 PQ 203 nits") << PRIMARIES_SMPTE_EG_432_1 << TRC_ITU_R_BT_2100_0_PQ << 203.0 << 203.0;
}

void TestProfileGeneration::testRetrieveNits()
{
    QFETCH(ColorPrimaries, primaries);
    QFETCH(TransferCharacteristics, transfer);
    QFETCH(qreal, requestedReferenceWhite);
    QFETCH(qreal, expectedReferenceWhite);
    KoColorProfileQuery query(primaries, transfer);
    if (requestedReferenceWhite > 0) {
        query.hdrReferenceWhite = requestedReferenceWhite;
    }
    const KoColorProfile *profile = KoColorSpaceRegistry::instance()->profileForInternal(query, true);
    QVERIFY(profile);

    QCOMPARE(profile->getColorPrimaries(), primaries);
    QCOMPARE(profile->getTransferCharacteristics(), transfer);

    if (expectedReferenceWhite > 0) {
        QVERIFY(profile->hdrReferenceWhite().has_value());
        QCOMPARE(*profile->hdrReferenceWhite(), expectedReferenceWhite);
    } else {
        QVERIFY(!profile->hdrReferenceWhite().has_value());
    }
}

KISTEST_MAIN(TestProfileGeneration)
