/* This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
    SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KOZOOMACTION_H
#define KOZOOMACTION_H

#include <kselectaction.h>
#include <KoZoomMode.h>

#include "kritawidgets_export.h"

class KoZoomState;

class KRITAWIDGETS_EXPORT KoZoomAction : public KSelectAction
{
    Q_OBJECT
public:

    KoZoomAction(const QString& text, QObject *parent);
    ~KoZoomAction() override;

    QWidget* createWidget(QWidget* parent) override;

public Q_SLOTS:

    void slotZoomStateChanged(const KoZoomState &zoomState);

    void setUsePrintResolutionMode(bool value);

protected Q_SLOTS:

    void slotTextZoomChanged(const QString &value);
    void slotZoomLevelChangedIndex(int index);
    void sliderValueChanged(int value);
    void slotUpdateGuiAfterZoom();

Q_SIGNALS:

    void zoomChanged( KoZoomMode::Mode mode, qreal zoom );

    void sigUsePrintResolutionModeChanged(bool value);

    void sigInternalUpdateZoomLevelsComboState(const QStringList &values, int index, const QString &activeText);
    void sigInternalUpdateZoomLevelsSliderState(int size, int index);
    void sigInternalUpdateUsePrintResolutionMode(bool value);

protected:
    void regenerateItems();
    void syncSliderWithZoom();

private:
    Q_DISABLE_COPY( KoZoomAction )

    class Private;
    Private * const d;
};

#endif
