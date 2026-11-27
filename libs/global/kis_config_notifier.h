/*
 *  SPDX-FileCopyrightText: 2007 Adrian Page <adrian@pagenet.plus.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_CONFIG_NOTIFIER_H_
#define KIS_CONFIG_NOTIFIER_H_

#include <QObject>
#include <QScopedPointer>

#include "kritaglobal_export.h"

class KRITAGLOBAL_EXPORT KisConfigNotifier : public QObject
{
    Q_OBJECT
public:
    KisConfigNotifier();
    ~KisConfigNotifier() override;

    static KisConfigNotifier *instance();

    void notifyConfigChanged(void);

    void notifyDropFramesModeChanged();
    void notifyPixelGridModeChanged();
    void notifyColorHistoryModeChanged();
    void notifyTouchPaintingChanged();
    void notifyColorSamplerPreviewStyleChanged();

    void notifyColorThemeChanged(const QString &filename);
    void notifyLongPressChanged(bool enabled);

#ifdef Q_OS_ANDROID
    void notifyUsePageUpDownMouseButtonEmulationWorkaroundChanged(bool enabled);
    void notifyUseHighFunctionKeyMouseButtonEmulationWorkaroundChanged(bool enabled);
    void notifyUseIgnoreHistoricTabletEventsWorkaroundChanged(bool enabled);
#endif

Q_SIGNALS:
    void configChanged(void);
    void dropFramesModeChanged();
    void pixelGridModeChanged();
    void colorHistoryModeChanged();
    void touchPaintingChanged();
    void sigColorSamplerPreviewStyleChanged();
    void signalColorThemeChanged(const QString &filename);
    void sigLongPressChanged(bool enabled);
#ifdef Q_OS_ANDROID
    void sigUsePageUpDownMouseButtonEmulationWorkaroundChanged(bool enabled);
    void sigUseHighFunctionKeyMouseButtonEmulationWorkaroundChanged(bool enabled);
    void sigUseIgnoreHistoricTabletEventsWorkaroundChanged(bool enabled);
#endif

private:
    KisConfigNotifier(const KisConfigNotifier&);
    KisConfigNotifier operator=(const KisConfigNotifier&);

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
