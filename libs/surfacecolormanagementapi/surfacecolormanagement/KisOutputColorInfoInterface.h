/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISOUTPUTCOLORINFOINTERFACE_H
#define KISOUTPUTCOLORINFOINTERFACE_H

#include "KisSurfaceColorimetry.h"

#include <QObject>
#include <QFuture>

class QWindow;
class QScreen;

class KRITASURFACECOLORMANAGEMENTAPI_EXPORT KisOutputColorInfoInterface : public QObject
{
    Q_OBJECT
public:
    KisOutputColorInfoInterface(QObject *parent = nullptr);
    virtual ~KisOutputColorInfoInterface();

    virtual bool isReady() const = 0;

    virtual std::optional<KisSurfaceColorimetry::SurfaceDescription> outputDescription(const QScreen *screen) const = 0;

Q_SIGNALS:
    void sigReadyChanged(bool isReady);

    void sigOutputDescriptionChanged(QScreen *screen, const KisSurfaceColorimetry::SurfaceDescription &desc);

protected:
    QWindow *m_window;
};

#endif