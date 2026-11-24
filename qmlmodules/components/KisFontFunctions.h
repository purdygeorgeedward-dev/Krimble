/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISFONTFUNCTIONS_H
#define KISFONTFUNCTIONS_H

#include <QObject>
#include <QQmlEngine>
class QQmlEngine;
class QJSEngine;
class KisFontFunctions: public QObject {

    Q_OBJECT
    QML_NAMED_ELEMENT(FontFunctions)

public:

    Q_INVOKABLE static QString wwsFontFamilyName(QString familyName);

    Q_INVOKABLE static QVariant wwsFontFamilyNameVariant(QString familyName);

    Q_INVOKABLE static QString sampleTagForQLocale(const QLocale &locale);

    Q_INVOKABLE static QVariantMap getMapFromQVariant(QVariant var);

    Q_INVOKABLE static bool isBitmapType(int type);
    Q_INVOKABLE static bool isPostScriptType(int type);
    Q_INVOKABLE static bool isOpenType(int type);

};

#endif
