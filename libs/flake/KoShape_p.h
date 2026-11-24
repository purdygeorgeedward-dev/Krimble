/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOSHAPEPRIVATE_H
#define KOSHAPEPRIVATE_H

#include "KoShape.h"

#include <QPoint>
#include <QPaintDevice>
#include <QTransform>
#include <QScopedPointer>
#include <QSharedData>

#include <KoClipMask.h>

class KoShapeManager;

class KoShape::SharedData : public QSharedData
{
public:
    explicit SharedData();
    virtual ~SharedData();

    explicit SharedData(const SharedData &rhs);

public:

    mutable QSizeF size;
    QString shapeId;
    QString name;

    QTransform localMatrix;

    QScopedPointer<KoShapeUserData> userData;
    QSharedPointer<KoShapeStrokeModel> stroke;
    QSharedPointer<KoShapeBackground> fill;
    bool inheritBackground = false;
    bool inheritStroke = false;
    QScopedPointer<KoClipPath> clipPath;
    QScopedPointer<KoClipMask> clipMask;
    QMap<QString, QString> additionalAttributes;
    QMap<QByteArray, QString> additionalStyleAttributes;
    qreal transparency;
    QString hyperLink;

    int zIndex : 16;
    int visible : 1;
    int printable : 1;
    int geometryProtected : 1;
    int keepAspect : 1;
    int selectable : 1;
    int protectContent : 1;

    QVector<PaintOrder> paintOrder {Fill, Stroke, Markers};
    bool inheritPaintOrder = true;
};

class KoShape::Private
{
private:
    struct DependeesLifetimeListener : KoShape::ShapeChangeListener
    {
        DependeesLifetimeListener(QList<KoShape*> &dependees)
            : m_dependees(dependees)
        {
        }

        void notifyShapeChanged(KoShape::ChangeType type, KoShape *shape) override {
            if (type == KoShape::Deleted && m_dependees.contains(shape)) {
                m_dependees.removeOne(shape);
            }
        }

    private:
        QList<KoShape*> &m_dependees;
    };

public:
    Private() : dependeesLifetimeListener(dependees) {}
    Private(const Private &rhs) = delete;

    KoShapeContainer *parent = nullptr;
    QSet<KoShapeManager *> shapeManagers;
    QSet<KoShape *> toolDelegates;
    QList<KoShape*> dependees;
    QList<KoShape::ShapeChangeListener*> listeners;
    DependeesLifetimeListener dependeesLifetimeListener;
};

#endif
