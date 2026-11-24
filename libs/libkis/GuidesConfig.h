/*
 *  SPDX-FileCopyrightText: 2024 Grum999 <grum999@grum.fr>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_GUIDESCONFIG_H
#define LIBKIS_GUIDESCONFIG_H

#include <QObject>
#include "kis_guides_config.h"

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT GuidesConfig : public QObject
{
    Q_OBJECT

public:
    GuidesConfig(KisGuidesConfig *guidesConfig);

    explicit GuidesConfig(QObject *parent = 0);
    ~GuidesConfig() override;

    bool operator==(const GuidesConfig &other) const;
    bool operator!=(const GuidesConfig &other) const;

public Q_SLOTS:

    QColor color() const;

    void setColor(const QColor &color) const;

    QString lineType() const;

    void setLineType(const QString &lineType);

    bool hasGuides() const;

    bool hasSamePositionAs(const GuidesConfig &guideConfig) const;

    QList<qreal> horizontalGuides() const;

    void setHorizontalGuides(const QList<qreal> &lines);

    QList<qreal> verticalGuides() const;

    void setVerticalGuides(const QList<qreal> &lines);

    bool fromXml(const QString &xmlContent) const;

    QString toXml() const;

    void removeAllGuides();

    bool visible() const;

    void setVisible(const bool value);

    bool locked() const;

    void setLocked(const bool value);

    bool snap() const;

    void setSnap(const bool value);

private:
    friend class Document;

    KisGuidesConfig guidesConfig() const;

private:
    struct Private;
    Private *d;

};

#endif
