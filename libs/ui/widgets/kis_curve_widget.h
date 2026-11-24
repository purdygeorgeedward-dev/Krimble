/*
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *  SPDX-FileCopyrightText: 2009 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_CURVE_WIDGET_H
#define KIS_CURVE_WIDGET_H

#include <QWidget>
#include <QColor>
#include <QPointF>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QPaintEvent>
#include <optional>

#include <kis_cubic_curve.h>

#include <kritaui_export.h>

class QSpinBox;

class KRITAUI_EXPORT KisCurveWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool pointSelected READ pointSelected NOTIFY pointSelectedChanged);

public:
    friend class CurveEditorItem;
    KisCurveWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    ~KisCurveWidget() override;

    void reset(void);

    void setPixmap(const QPixmap & pix);
    QPixmap getPixmap();

    bool pointSelected() const;

    enum PointConstrain
    {
        PointConstrain_None,
        PointConstrain_AlwaysCorner,
        PointConstrain_AlwaysSmooth,
    };

    bool setCurrentPoint(const QPointF &position, bool setAsCorner);
    bool setCurrentPointPosition(const QPointF &position);
    bool setCurrentPointAsCorner(bool setAsCorner);
    void setGlobalPointConstrain(PointConstrain constrain);
    std::optional<KisCubicCurvePoint> currentPoint() const;
    std::optional<QPointF> currentPointPosition() const;
    std::optional<bool> isCurrentPointSetAsCorner() const;
    PointConstrain globalPointConstrain() const;

Q_SIGNALS:

    void modified(void);
    void curveChanged(const KisCubicCurve &);
    void pointSelectedChanged();
    void compressorShouldEmitModified();

    void shouldSyncIOControls();
    void shouldFocusIOControls();

protected Q_SLOTS:
    void notifyModified();

    void slotCompressorShouldEmitModified();

protected:

    void keyPressEvent(QKeyEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent * e) override;
    void mouseReleaseEvent(QMouseEvent * e) override;
    void mouseMoveEvent(QMouseEvent * e) override;
    void leaveEvent(QEvent *) override;
    void resizeEvent(QResizeEvent *e) override;

public:

    KisCubicCurve curve();

    void setupInOutControls(QSpinBox *in, QSpinBox *out, int inMin, int inMax, int outMin, int outMax);
    void dropInOutControls();

    void addPointInTheMiddle();

public Q_SLOTS:
    void setCurve(KisCubicCurve inlist);

private:

    class Private;
    Private * const d {nullptr};
};

#endif
