/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISSURFACECOLORMANAGERINTERFACE_H
#define KISSURFACECOLORMANAGERINTERFACE_H

#include "KisSurfaceColorimetry.h"

#include <QObject>
#include <QFuture>

class QWindow;
class QScreen;

class KRITASURFACECOLORMANAGEMENTAPI_EXPORT KisSurfaceColorManagerInterface : public QObject
{
    Q_OBJECT
public:
    KisSurfaceColorManagerInterface(QWindow *window, QObject *parent = nullptr);
    virtual ~KisSurfaceColorManagerInterface();

    virtual bool isReady() const = 0;

    virtual bool supportsSurfaceDescription(const KisSurfaceColorimetry::SurfaceDescription &desc) = 0;

    virtual bool supportsRenderIntent(const KisSurfaceColorimetry::RenderIntent &intent) = 0;

    virtual QFuture<bool> setSurfaceDescription(const KisSurfaceColorimetry::SurfaceDescription &desc, KisSurfaceColorimetry::RenderIntent intent) = 0;

    virtual void unsetSurfaceDescription() = 0;

    virtual std::optional<KisSurfaceColorimetry::SurfaceDescription> surfaceDescription() const = 0;

    virtual std::optional<KisSurfaceColorimetry::RenderIntent> renderingIntent() const = 0;

    virtual std::optional<KisSurfaceColorimetry::SurfaceDescription> preferredSurfaceDescription() const = 0;

Q_SIGNALS:
    void sigReadyChanged(bool value);
    void sigPreferredSurfaceDescriptionChanged(const KisSurfaceColorimetry::SurfaceDescription &desc);

protected:
    QWindow *m_window;
};

#endif