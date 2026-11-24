/*
 *  SPDX-FileCopyrightText: 2025 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISROOTSURFACEINFOPROXY_H
#define KISROOTSURFACEINFOPROXY_H

#include <kritaui_export.h>
#include <KisRootSurfaceTrackerBase.h>

class KisSRGBSurfaceColorSpaceManager;
class KoColorProfile;
namespace KisSurfaceColorimetry {
struct SurfaceDescription;
}

class KRITAUI_EXPORT KisRootSurfaceInfoProxy : public KisRootSurfaceTrackerBase
{
    Q_OBJECT
public:
    KisRootSurfaceInfoProxy(QWidget *watched, QObject *parent = nullptr);
    ~KisRootSurfaceInfoProxy();
    const KoColorProfile* rootSurfaceProfile() const;
    bool isReady() const;

    QString colorManagementReport() const;
    QString osPreferredColorSpaceReport() const;

    std::optional<KisSurfaceColorimetry::SurfaceDescription> currentSurfaceDescription() const;

Q_SIGNALS:
    void sigRootSurfaceProfileChanged(const KoColorProfile *profile) const;

private:

    void connectToNativeWindow(QWindow *nativeWindow) override;
    void disconnectFromNativeWindow() override;
    void tryUpdateRootSurfaceProfile();

private:
    QPointer<KisSRGBSurfaceColorSpaceManager> m_topLevelSurfaceManager;
    QMetaObject::Connection m_surfaceManagerConnection;

    const KoColorProfile* m_rootSurfaceProfile {nullptr};
};

#endif
