/*
 *  SPDX-FileCopyrightText: 2024 Grum999 <grum999@grum.fr>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_GRIDCONFIG_H
#define LIBKIS_GRIDCONFIG_H

#include <QObject>
#include "kis_grid_config.h"

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT GridConfig : public QObject
{
    Q_OBJECT

public:
    GridConfig(KisGridConfig *guidesConfig);

    explicit GridConfig(QObject *parent = 0);
    ~GridConfig() override;

    bool operator==(const GridConfig &other) const;
    bool operator!=(const GridConfig &other) const;

public Q_SLOTS:

    bool visible() const;

    void setVisible(bool visible);

    bool snap() const;

    void setSnap(bool snap);

    QPoint offset() const;

    void setOffset(QPoint offset);

    QPoint spacing() const;

    void setSpacing(QPoint spacing);

    bool spacingActiveHorizontal() const;

    void setSpacingActiveHorizontal(bool active);

    bool spacingActiveVertical() const;

    void setSpacingActiveVertical(bool active);

    int subdivision() const;

    void setSubdivision(int subdivision);

    qreal angleLeft() const;

    void setAngleLeft(qreal angleLeft);

    qreal angleRight() const;

    void setAngleRight(qreal angleRight);

    bool angleLeftActive() const;

    void setAngleLeftActive(bool active);

    bool angleRightActive() const;

    void setAngleRightActive(bool active);

    int cellSpacing() const;

    void setCellSpacing(int cellSpacing);

    int cellSize() const;

    void setCellSize(int cellSize);

    QString type() const;

    void setType(const QString &gridType);

    bool offsetAspectLocked() const;

    void setOffsetAspectLocked(bool offsetAspectLocked);

    bool spacingAspectLocked() const;

    void setSpacingAspectLocked(bool spacingAspectLocked);

    bool angleAspectLocked() const;

    void setAngleAspectLocked(bool angleAspectLocked);

    QString lineTypeMain() const;

    void setLineTypeMain(const QString &lineType);

    QString lineTypeSubdivision() const;

    void setLineTypeSubdivision(const QString &lineType);

    QString lineTypeVertical() const;

    void setLineTypeVertical(const QString &lineType);

    QColor colorMain() const;

    void setColorMain(QColor colorMain);

    QColor colorSubdivision() const;

    void setColorSubdivision(QColor colorSubdivision);

    QColor colorVertical() const;

    void setColorVertical(QColor colorVertical);

    bool fromXml(const QString &xmlContent) const;

    QString toXml() const;

private:
    friend class Document;

    KisGridConfig gridConfig() const;

private:
    struct Private;
    Private *d;

};

#endif
