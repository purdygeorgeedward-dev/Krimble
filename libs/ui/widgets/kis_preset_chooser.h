/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>
 *  SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_PRESET_CHOOSER_H_
#define KIS_PRESET_CHOOSER_H_

#include <QWidget>
#include <QPointer>

#include <KoResource.h>
#include <KoID.h>
#include "kis_signal_auto_connection.h"
#include "ResourceListViewModes.h"

class KoAbstractResourceServerAdapter;
class KisPresetDelegate;
class KisResourceItemChooser;

#include <kritaui_export.h>

class KRITAUI_EXPORT KisPresetChooser : public QWidget
{
    Q_OBJECT

public:

    KisPresetChooser(QWidget *parent = 0);
    ~KisPresetChooser() override;

    enum ViewMode {
        THUMBNAIL,
        DETAIL
    };

    void setViewMode(ViewMode mode);
    void setViewModeToThumbnail();
    void setViewModeToDetail();

    void setCurrentResource(KoResourceSP resource);

    KoResourceSP currentResource() const;

    void showTaggingBar(bool show);

    KisResourceItemChooser *itemChooser();

    void setPresetFilter(const QString& paintOpId);

    int iconSize();

Q_SIGNALS:
    void resourceSelected(KoResourceSP resource);
    void resourceClicked(KoResourceSP resource);

public Q_SLOTS:
    void updateViewSettings();

    void setIconSize(int newSize);

    void saveIconSize();

    void showHideBrushNames(ListViewMode newViewMode);

private Q_SLOTS:
    void notifyConfigChanged();
    void slotResourceWasSelected(KoResourceSP resource);
    void slotCurrentPresetChanged();

private:
    KisResourceItemChooser *m_chooser {0};
    KisPresetDelegate* m_delegate {0};
    ViewMode m_mode;

    class PaintOpFilterModel;
    QPointer<PaintOpFilterModel> m_paintOpFilterModel;

    KisSignalAutoConnectionsStore m_currentPresetConnections;
};

#endif

