/*
 *  SPDX-FileCopyrightText: 2019 Anna Medonosova <anna.medonosova@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISMIRRORAXISCONFIG_H
#define KISMIRRORAXISCONFIG_H

#include <QScopedPointer>

#include "kritaui_export.h"
#include <boost/operators.hpp>

class QDomElement;
class QDomDocument;

class KRITAUI_EXPORT KisMirrorAxisConfig : public QObject, boost::equality_comparable<KisMirrorAxisConfig>
{
    Q_OBJECT

public:
    KisMirrorAxisConfig();
    ~KisMirrorAxisConfig();

    KisMirrorAxisConfig(const KisMirrorAxisConfig &rhs);
    KisMirrorAxisConfig& operator=(const KisMirrorAxisConfig& rhs);
    bool operator==(const KisMirrorAxisConfig& rhs) const;

    bool mirrorHorizontal() const;
    void setMirrorHorizontal(bool state);

    bool mirrorVertical() const;
    void setMirrorVertical(bool state);

    bool lockHorizontal() const;
    void setLockHorizontal(bool state);

    bool lockVertical() const;
    void setLockVertical(bool state);

    bool hideVerticalDecoration() const;
    void setHideVerticalDecoration(bool state);

    bool hideHorizontalDecoration() const;
    void setHideHorizontalDecoration(bool state);

    float handleSize() const;
    void setHandleSize(float size);

    float horizontalHandlePosition() const;
    void setHorizontalHandlePosition(float position);

    float verticalHandlePosition() const;
    void setVerticalHandlePosition(float position);

    QPointF axisPosition() const;
    void setAxisPosition(QPointF position);

    QDomElement saveToXml(QDomDocument& doc, const QString &tag) const;

    bool loadFromXml(const QDomElement &parent);

    bool isDefault() const;

private:
    class Private;
    const QScopedPointer<Private> d;
};

#endif
