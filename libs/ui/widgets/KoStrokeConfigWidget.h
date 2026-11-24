/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * SPDX-FileCopyrightText: 2002 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2005, 2011 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2005-2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef STROKECONFIGWIDGET_H
#define STROKECONFIGWIDGET_H

#include "kritaui_export.h"

#include <QWidget>
#include <QLabel>
#include <KoFlakeTypes.h>
#include <KoFlake.h>

class KoUnit;
class KoShapeStrokeModel;
class KoShapeStroke;
class KoMarker;
class KoCanvasBase;
class KoShapeStroke;
class KisSpinBoxUnitManager;

class KRITAUI_EXPORT KoStrokeConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KoStrokeConfigWidget(KoCanvasBase *canvas, QWidget *parent);
    ~KoStrokeConfigWidget() override;

    void setNoSelectionTrackingMode(bool value);

    Qt::PenStyle lineStyle() const;
    QVector<qreal> lineDashes() const;
    qreal lineWidth() const;
    QColor color() const;
    qreal miterLimit() const;
    KoMarker *startMarker() const;
    KoMarker *endMarker() const;
    Qt::PenCapStyle capStyle() const;
    Qt::PenJoinStyle joinStyle() const;
    QLabel *thicknessLabel;
    QLabel *strokeStyleLabel;
    QFrame* separatorLine;

    KoShapeStrokeSP createShapeStroke();

    void setActive(bool active);

    void updateStyleControlsAvailability(bool enabled);
    void setUnitManagers(KisSpinBoxUnitManager* managerLineWidth, KisSpinBoxUnitManager* managerMitterLimit);

    void activate();
    void deactivate();

private Q_SLOTS:

    void updateMarkers(const QList<KoMarker*> &markers);

    void canvasResourceChanged(int key, const QVariant &value);

    void selectionChanged();

    void applyDashStyleChanges();
    void applyLineWidthChanges();
    void applyJoinCapChanges();

    void applyMarkerChanges(int rawPosition);

Q_SIGNALS:
    void sigStrokeChanged();

private:
    void setUnit(const KoUnit &unit, KoShape *representativeShape);
    void blockChildSignals(bool block);
    void loadCurrentStrokeFillFromResourceServer();

private:
    class Private;
    Private * const d;
};

#endif
