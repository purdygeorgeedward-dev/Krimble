/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISDYNAMICSENSORIDS_H
#define KISDYNAMICSENSORIDS_H

#include <KoID.h>

const KoID FuzzyPerDabId("fuzzy", ki18nc("Context: dynamic sensors", "Fuzzy Dab"));
const KoID FuzzyPerStrokeId("fuzzystroke", ki18nc("Context: dynamic sensors", "Fuzzy Stroke"));
const KoID SpeedId("speed", ki18nc("Context: dynamic sensors", "Speed"));
const KoID FadeId("fade", ki18nc("Context: dynamic sensors", "Fade"));
const KoID DistanceId("distance", ki18nc("Context: dynamic sensors", "Distance"));
const KoID TimeId("time", ki18nc("Context: dynamic sensors", "Time"));
const KoID DrawingAngleId("drawingangle", ki18nc("Context: dynamic sensors", "Drawing angle"));
const KoID RotationId("rotation", ki18nc("Context: dynamic sensors", "Rotation"));
const KoID PressureId("pressure", ki18nc("Context: dynamic sensors", "Pressure"));
const KoID PressureInId("pressurein", ki18nc("Context: dynamic sensors", "PressureIn"));
const KoID XTiltId("xtilt", ki18nc("Context: dynamic sensors", "X-Tilt"));
const KoID YTiltId("ytilt", ki18nc("Context: dynamic sensors", "Y-Tilt"));

const KoID TiltDirectionId("ascension", ki18nc("Context: dynamic sensors", "Tilt direction"));
const KoID TiltElevationId("declination", ki18nc("Context: dynamic sensors", "Tilt elevation"));

const KoID PerspectiveId("perspective", ki18nc("Context: dynamic sensors", "Perspective"));
const KoID TangentialPressureId("tangentialpressure", ki18nc("Context: dynamic sensors", "Tangential pressure"));
const KoID SensorsListId("sensorslist", "SHOULD NOT APPEAR IN THE UI !");

#endif
