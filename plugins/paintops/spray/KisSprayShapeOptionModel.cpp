/*
 *  SPDX-FileCopyrightText: 2022 Agata Cacko <cacko.azh@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "KisSprayShapeOptionModel.h"

#include <KisLager.h>
#include <klocalizedstring.h>

namespace {

auto makeSizePack = lager::lenses::getset(
    [](const std::tuple<KisSprayShapeOptionData, int, qreal> &value) -> SprayShapeSizePack {
        auto [optionData, diameter, scale] = value;

        SprayShapeSizePack pack;
        pack.isProportional = optionData.proportional;

        if (optionData.proportional) {
            pack.pxSize = optionData.size * qreal(diameter) * scale / 100.0;
            pack.proportionalSize = optionData.size;
        } else {
            pack.pxSize = optionData.size;
            pack.proportionalSize = optionData.size * 100.0 / (qreal(diameter) * scale);
        }
        pack.diameter = diameter;
        pack.scale = scale;

        return pack;
    },
    [] (std::tuple<KisSprayShapeOptionData, int, qreal> value, const SprayShapeSizePack &pack) -> std::tuple<KisSprayShapeOptionData, int, qreal> {
        auto [optionData, diameter, scale] = value;

        optionData.size = pack.isProportional ? pack.proportionalSize : pack.pxSize;
        optionData.proportional = pack.isProportional;

        return std::make_tuple(optionData,
                               diameter,
                               scale);
    }
    );

auto calcEffectiveSize = lager::lenses::getset(
    [](const SprayShapeSizePack &pack) -> QSize {
        return pack.isProportional ? pack.proportionalSize : pack.pxSize;
    },
    [] (SprayShapeSizePack pack, const QSize &size) -> SprayShapeSizePack {
        if (pack.isProportional) {
            pack.proportionalSize = size;
            pack.pxSize = size * pack.diameter * pack.scale / 100.0;
        } else {
            pack.proportionalSize = size * 100.0 / (pack.diameter * pack.scale);
            pack.pxSize = size;
        }
        return pack;
    }
    );
}

KisSprayShapeOptionModel::KisSprayShapeOptionModel(lager::cursor<KisSprayShapeOptionData> _optionData, lager::cursor<int> diameter, lager::cursor<qreal> scale)
    : optionData(_optionData)
    , sizePack {lager::with(
                   _optionData,
                   diameter,
                   scale
                   ).zoom(makeSizePack)}
    , LAGER_QT(shape) {_optionData[&KisSprayShapeOptionData::shape].zoom(kislager::lenses::do_static_cast<quint8, int>)}
    , LAGER_QT(effectiveSize) {sizePack.zoom(calcEffectiveSize)}
    , LAGER_QT(effectiveProportional) {sizePack[&SprayShapeSizePack::isProportional]}
    , LAGER_QT(enabled) {_optionData[&KisSprayShapeOptionData::enabled]}
    , LAGER_QT(imageUrl) {_optionData[&KisSprayShapeOptionData::imageUrl]}
    , LAGER_QT(sizeSuffix) { sizePack[&SprayShapeSizePack::isProportional].map([] (bool isProportional) { return isProportional ? i18n("%") : i18n(" px"); } ) }
{
}
