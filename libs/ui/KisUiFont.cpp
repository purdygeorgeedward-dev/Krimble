/*
 * SPDX-FileCopyrightText: 2016 The Qt Company Ltd.
 * SPDX-FileCopyrightText: 2020 Halla Rempt <halla@valdyas.org>
 * SPDX-FileCopyrightText: 2021 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 *
 * This file contains code that was borrowed and modified from the Qt Project,
 * originally licensed under LGPL-3.0-or-later or GPL-2.0-or-later.
 */

#include "KisUiFont.h"

#include <kis_config.h>

#include <boost/optional.hpp>

#include <QtGlobal>
#include <QFontDatabase>

#if defined(Q_OS_WIN) && QT_VERSION < 0x060000
# include <qt_windows.h>
#endif

namespace KisUiFont
{

static const QString useCustomSystemFontCfgName = QStringLiteral("use_custom_system_font");
static const QString customSystemFontCfgName = QStringLiteral("custom_system_font");
static const QString customFontSizeCfgName = QStringLiteral("custom_font_size");

static QFont systemDefaultUiFont();

static boost::optional<QFont> userCfgUiFont();

#if defined(Q_OS_WIN) && QT_VERSION < 0x060000
static QFont windowsSystemUiFont();
#endif

QFont systemDefaultUiFont()
{
#if defined(Q_OS_WIN) && QT_VERSION < 0x060000
    return windowsSystemUiFont();
#else
    return QFontDatabase::systemFont(QFontDatabase::GeneralFont);
#endif
}

boost::optional<QFont> userCfgUiFont()
{
    KisConfig cfg(true);
    if (cfg.readEntry<bool>(useCustomSystemFontCfgName, false)) {
        QString fontName = cfg.readEntry<QString>(customSystemFontCfgName, QString());
        if (fontName.isEmpty()) {
            return boost::none;
        }
        int fontSize = cfg.readEntry<int>(customFontSizeCfgName, -1);
        if (fontSize <= 6) {
            QFont systemFont = systemDefaultUiFont();
            fontSize = systemFont.pointSize();
            if (fontSize == -1) {
                fontSize = systemFont.pixelSize();
            }
        }
        return QFont(fontName, fontSize);
    } else {
        return boost::none;
    }
}

QFont normalFont()
{
    QFont font;
    if (boost::optional<QFont> userFont = userCfgUiFont()) {
        font = *userFont;
    } else {
        font = systemDefaultUiFont();
    }
#ifdef Q_OS_WIN
    font.setHintingPreference(QFont::PreferFullHinting);
#endif
    return font;
}

QFont dockFont()
{
    QFont font = normalFont();
    qreal ratio = 0.9;
    if (font.pixelSize() == -1) {
        font.setPointSizeF(font.pointSizeF() * ratio);
    } else {
        font.setPixelSize(qRound(font.pixelSize() * ratio));
    }
    return font;
}

#if defined(Q_OS_WIN) && QT_VERSION < 0x060000

static int defaultVerticalDPI()
{
    static int vDPI = -1;
    if (vDPI == -1) {
        if (HDC defaultDC = GetDC(0)) {
            vDPI = GetDeviceCaps(defaultDC, LOGPIXELSY);
            ReleaseDC(0, defaultDC);
        } else {
            vDPI = 96;
        }
    }
    return vDPI;
}

QFont::Weight weightFromInteger(int weight)
{
    if (weight < 150) {
        return QFont::Thin;
    }
    if (weight < 250) {
        return QFont::ExtraLight;
    }
    if (weight < 350) {
        return QFont::Light;
    }
    if (weight < 450) {
        return QFont::Normal;
    }
    if (weight < 550) {
        return QFont::Medium;
    }
    if (weight < 650) {
        return QFont::DemiBold;
    }
    if (weight < 750) {
        return QFont::Bold;
    }
    if (weight < 850) {
        return QFont::ExtraBold;
    }
    return QFont::Black;
}

static QFont LOGFONT_to_QFont(const LOGFONTW& logFont)
{
    const int verticalDPI_In = defaultVerticalDPI();
    QFont qFont(QString::fromWCharArray(logFont.lfFaceName));
    qFont.setItalic(logFont.lfItalic);
    if (logFont.lfWeight != FW_DONTCARE) {
        qFont.setWeight(weightFromInteger(logFont.lfWeight));
    }
    const qreal logFontHeight = qAbs(logFont.lfHeight);
    qFont.setPointSizeF(logFontHeight * 72.0 / qreal(verticalDPI_In));
    qFont.setUnderline(logFont.lfUnderline);
    qFont.setOverline(false);
    qFont.setStrikeOut(logFont.lfStrikeOut);
    return qFont;
}

static QFont windowsSystemUiFont()
{
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICSW, lfMessageFont) + sizeof(LOGFONTW);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, ncm.cbSize , &ncm, 0);
    const QFont systemFont = LOGFONT_to_QFont(ncm.lfMessageFont);
    return systemFont;
}
#endif

}
