/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006, 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPATHTOOLSELECTION_H
#define KOPATHTOOLSELECTION_H

#include <KoToolSelection.h>
#include <KoPathShape.h>

class KoPathTool;
class KoPathPoint;
class KoPathPointData;
class KoViewConverter;
class KoColorDisplayRendererInterface;
class QPainter;

class KRITAFLAKE_EXPORT KoPathToolSelection : public KoToolSelection, public KoPathShape::PointSelectionChangeListener
{
    Q_OBJECT

public:
    explicit KoPathToolSelection(KoPathTool *tool);

    ~KoPathToolSelection() override;

    void paint(QPainter &painter, const KoViewConverter &converter, qreal handleRadius, KoColorDisplayRendererInterface *renderInterface);

    void add(KoPathPoint *point, bool clear);

    void remove(KoPathPoint *point);

    void clear();

    void selectPoints(const QRectF &rect, bool clearSelection);

    void selectAll();

    int objectCount() const;

    int size() const;

    bool contains(KoPathPoint *point);

    const QSet<KoPathPoint *> &selectedPoints() const;

    QList<KoPathPointData> selectedPointsData() const;

    QList<KoPathPointData> selectedSegmentsData() const;

    QList<KoPathShape*> selectedShapes() const;

    void setSelectedShapes(const QList<KoPathShape*> shapes);

    void update();

    bool hasSelection() override;

    void recommendPointSelectionChange(KoPathShape *shape, const QList<KoPathPointIndex> &newSelection) override;
    void notifyPathPointsChanged(KoPathShape *shape) override;
    void notifyShapeChanged(KoShape::ChangeType type, KoShape *shape) override;

Q_SIGNALS:
    void selectionChanged();

private:
    typedef QMap<KoPathShape *, QSet<KoPathPoint *> > PathShapePointMap;

    QSet<KoPathPoint *> m_selectedPoints;
    PathShapePointMap m_shapePointMap;
    KoPathTool *m_tool;
    QList<KoPathShape*> m_selectedShapes;
};

#endif
