/*
 * SPDX-FileCopyrightText: 2016 Julian Thijssen <julianthijssen@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "canvas/kis_display_filter.h"

#include <QOpenGLShaderProgram>
#include <QByteArray>
#include <QString>

#include <unordered_map>
#include <string>
#include <stdexcept>

#include <map>

enum Uniform { ModelViewProjection, TextureMatrix, ViewportScale,
               TexelSize, Texture0, Texture1, FixedLodLevel, FragmentColor };

class KisShaderProgram : public QOpenGLShaderProgram {
public:
    static std::map<Uniform, const char *> names;

    int location(Uniform uniform) {
        std::map<Uniform, int>::const_iterator it = locationMap.find(uniform);
        if (it != locationMap.end()) {
            return it->second;
        } else {
            int location = uniformLocation(names[uniform]);
            locationMap[uniform] = location;
            return location;
        }
    }
private:
    std::map<Uniform, int> locationMap;
};

class ShaderLoaderException : public std::runtime_error {
public:
    ShaderLoaderException(QString error) : std::runtime_error(error.toStdString()) { }
};

class KisOpenGLShaderLoader {
public:
    KisShaderProgram *loadDisplayShader(QSharedPointer<KisDisplayFilter> displayFilter, bool useHiQualityFiltering);
    KisShaderProgram *loadCheckerShader();
    KisShaderProgram *loadSolidColorShader();

private:
    KisShaderProgram *loadShader(QString vertPath, QString fragPath, QByteArray vertHeader, QByteArray fragHeader);
};
