/*
 * SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_CANVAS_DECORATION_H_
#define _KIS_CANVAS_DECORATION_H_

#include <QObject>
#include <QPointer>

#include <kritaui_export.h>
#include <kis_image.h>
#include "KisView.h"
#include <kis_shared.h>

class KisCanvasWidgetBase;
class KisCanvas2;
class QRectF;
class QPainter;
class KisCoordinatesConverter;

class KisCanvasDecoration;
typedef KisSharedPtr<KisCanvasDecoration> KisCanvasDecorationSP;

class KRITAUI_EXPORT KisCanvasDecoration : public QObject, public KisShared
{
    Q_OBJECT
public:
    KisCanvasDecoration(const QString& id, QPointer<KisView>parent);

    ~KisCanvasDecoration() override;

    void setView(QPointer<KisView> imageView);

    const QString& id() const;

    bool visible() const;

    void paint(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter *converter,KisCanvas2* canvas);

    int priority() const;

    static bool comparePriority(KisCanvasDecorationSP decoration1, KisCanvasDecorationSP decoration2);

    virtual void notifyWindowMinimized(bool minimized);
    virtual void setCanvasWidget(KisCanvasWidgetBase* canvas) { Q_UNUSED(canvas); };

public Q_SLOTS:
    virtual void setVisible(bool v);
    void toggleVisibility();
protected:
    virtual void drawDecoration(QPainter& gc, const QRectF& updateArea, const KisCoordinatesConverter *converter,KisCanvas2* canvas) = 0;

    QPointer<KisView> view() const;

    void setPriority(int value);

    int decorationThickness() const;

private:
    struct Private;
    Private* const d;
};

#endif
