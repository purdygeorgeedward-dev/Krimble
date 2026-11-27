/*
 * SPDX-FileCopyrightText: 2017 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2020-2021 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef GMIC_H
#define GMIC_H

#include <QString>

enum class OutputMode { InPlace, NewLayers, NewActiveLayers, NewImage, Unspecified = 100 };

enum class InputLayerMode {
    NoInput,
    Active,
    All,
    ActiveAndBelow,
    ActiveAndAbove,
    AllVisible,
    AllInvisible,
    AllVisiblesDesc_DEPRECATED,
    AllInvisiblesDesc_DEPRECATED,
    AllDesc_DEPRECATED,
    Unspecified = 100
};

template<typename T>
struct gmic_image {
    unsigned int _width{0};
    unsigned int _height{0};
    unsigned int _depth{1};
    unsigned int _spectrum{3};
    bool _is_shared{false};
    T *_data{nullptr};
    QString name{};

    ~gmic_image()
    {
        if (!_is_shared)
            delete[] _data;
    }

    void assign(unsigned int w, unsigned int h, unsigned int d, unsigned int s)
    {
        _width = w;
        _height = h;
        _depth = d;
        _spectrum = s;
    }
};

#endif
