/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISROOTSURFACETRACKERBASE_H
#define KISROOTSURFACETRACKERBASE_H

#include <kritawidgetutils_export.h>

#include <QObject>
#include <QPointer>

class QWidget;
class QWindow;
class QEvent;

class KRITAWIDGETUTILS_EXPORT KisRootSurfaceTrackerBase : public QObject
{
    Q_OBJECT
public:
    KisRootSurfaceTrackerBase(QWidget *watched, QObject *parent = nullptr);
    ~KisRootSurfaceTrackerBase();

    QWidget* trackedWidget() const;

protected:
    virtual void connectToNativeWindow(QWindow *nativeWindow) = 0;
    virtual void disconnectFromNativeWindow() = 0;
    void initialize();

private:

    bool eventFilter(QObject *watched, QEvent *event) override;

    void tryUpdateHierarchy();
    void tryReconnectToNativeWindow();

    QVector<QPointer<QObject>> getCurrentHierarchy(QWidget *wdg);

    void reconnectToHierarchy(const QVector<QPointer<QObject>> newHierarchy);

private:
    QWidget *m_watched {nullptr};
    QVector<QPointer<QObject>> m_watchedHierarchy;

    QPointer<QWidget> m_topLevelWidgetWithSurface;

    QPointer<QWindow> m_topLevelNativeWindow;
    QPointer<QObject> m_childChangedFilter;
};

#endif
