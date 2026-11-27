/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
   SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
   SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
   SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
   SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
   SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KIS_RESOURCE_ITEM_CHOOSER
#define KIS_RESOURCE_ITEM_CHOOSER

#include <QWidget>

#include <QModelIndex>
#include <QListView>

#include <KoResource.h>
#include <KisKineticScroller.h>
#include "KisPopupButton.h"
#include "ResourceListViewModes.h"

class QAbstractProxyModel;
class QAbstractItemDelegate;
class QAbstractButton;
class QToolButton;
class QSortFilterProxyModel;
class KisResourceItemListView;
class KisTagFilterResourceProxyModel;

#include "kritaresourcewidgets_export.h"

class KRITARESOURCEWIDGETS_EXPORT KisResourceItemChooser : public QWidget
{
    Q_OBJECT
public:
    enum Buttons { Button_Import, Button_Remove };
    enum class Layout {
        NotSet,
        Vertical,
        Horizontal2Rows,
        Horizontal1Row
    };

    explicit KisResourceItemChooser(const QString &resourceType, bool usePreview = false, QWidget *parent = 0);
    ~KisResourceItemChooser() override;

    void setResponsiveness(bool isResponsive);

    void setListViewMode(ListViewMode viewMode);

    void showTaggingBar(bool show);

    KisTagFilterResourceProxyModel *tagFilterModel() const;

    void showViewModeBtn(bool visible);

    KisPopupButton *viewModeButton() const;

    void showStorageBtn(bool visible);

    void setRowHeight(int rowHeight);

    void setColumnWidth(int columnWidth);

    void setItemDelegate(QAbstractItemDelegate *delegate);

    KoResourceSP currentResource(bool includeHidden = false) const;

    void setCurrentResource(KoResourceSP resource);
    void setCurrentResource(QString resourceName);

    void setCurrentItem(int row);

    void showImportExportBtns(bool show);

    void setPreviewOrientation(Qt::Orientation orientation);

    void setPreviewTiled(bool tiled);

    void setGrayscalePreview(bool grayscale);

    QSize viewSize() const;

    KisResourceItemListView *itemView() const;

    void setSynced(bool sync);

    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    void listViewModeChanged(ListViewMode newViewMode);

    void resourceSelected(KoResourceSP resource);

    void resourceClicked(KoResourceSP resource);

public Q_SLOTS:
    void slotButtonClicked(int button);
    void slotScrollerStateChanged(QScroller::State state){ KisKineticScroller::updateCursor(this, state); }
    void updateView();

private Q_SLOTS:
    void scrollBackwards();
    void scrollForwards();
    void activate(const QModelIndex &index);
    void clicked(const QModelIndex &index);
    void contextMenuRequested(const QPoint &pos);
    void baseLengthChanged(int length);
    void afterFilterChanged();
    void slotSaveSplitterState();

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateButtonState();
    void updatePreview(const QModelIndex &idx);

    void hideEverything();
    void applyVerticalLayout();
    void changeLayoutBasedOnSize();

    KoResourceSP resourceFromModelIndex(const QModelIndex &index) const;

    class Private;
    Private *const d;
};

#endif
