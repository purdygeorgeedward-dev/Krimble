/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOCREATEPATHTOOL_H
#define KOCREATEPATHTOOL_H

#include "kritabasicflakes_export.h"

#include <KoFlakeTypes.h>
#include <KoToolBase.h>

#include <QList>

class KoPathShape;
class KoShapeStroke;

class KoCreatePathToolPrivate;

#define KoCreatePathTool_ID "CreatePathTool"

class KRITABASICFLAKES_EXPORT KoCreatePathTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KoCreatePathTool(KoCanvasBase * canvas);
    ~KoCreatePathTool() override;

    QRectF decorationsRect() const override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;

    bool pathStarted() const;

    bool tryMergeInPathShape(KoPathShape *pathShape);

    void setEnableClosePathShortcut(bool value);

public Q_SLOTS:
    void activate(const QSet<KoShape*> &shapes) override;
    void deactivate() override;
    void canvasResourceChanged(int key, const QVariant & res) override;

Q_SIGNALS:
    void sigUpdateAutoSmoothCurvesGUI(bool value);

protected:
    virtual void addPathShape(KoPathShape* pathShape);

    virtual void beginShape() {}
    virtual void endShape() {}

protected:
    virtual void paintPath(KoPathShape& pathShape, QPainter &painter, const KoViewConverter &converter);

    void handleMouseMove(const KoPointerEvent *event, bool considerDrag);
    void endPath();
    void endPathWithoutLastPoint();
    void cancelPath();
    void removeLastPoint();

    bool addPathShapeImpl(KoPathShape* pathShape, bool tryMergeOnly);

    QList<QPointer<QWidget> > createOptionWidgets() override;

private:
    Q_DECLARE_PRIVATE(KoCreatePathTool)
    Q_PRIVATE_SLOT(d_func(), void angleDeltaChanged(qreal))
    Q_PRIVATE_SLOT(d_func(), void angleSnapChanged(int))
    Q_PRIVATE_SLOT(d_func(), void autoSmoothCurvesChanged(bool))
};
#endif
