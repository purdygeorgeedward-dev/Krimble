/*
 *  SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_GRADIENT_CHOOSER_H_
#define KIS_GRADIENT_CHOOSER_H_

#include <QFrame>
#include <QScopedPointer>

#include <KoDialog.h>
#include <KoColor.h>
#include <KoResource.h>
#include <kritaui_export.h>
#include <KoCanvasResourcesInterface.h>
#include <KoAbstractGradient.h>

class KisResourceItemChooser;

class KRITAUI_EXPORT KisGradientChooser : public QFrame
{
    Q_OBJECT

public:
    enum ViewMode
    {
        ViewMode_Icon,
        ViewMode_List
    };

    enum ItemSize
    {
        ItemSize_Small,
        ItemSize_Medium,
        ItemSize_Large,
        ItemSize_Custom
    };

    KisGradientChooser(QWidget *parent = 0, const char *name = 0, bool useGlobalViewOptions = true);
    ~KisGradientChooser() override;

    void setCanvasResourcesInterface(KoCanvasResourcesInterfaceSP canvasResourcesInterface);
    KoCanvasResourcesInterfaceSP canvasResourcesInterface() const;

    KoResourceSP currentResource(bool includeHidden = false);
    void setCurrentResource(KoResourceSP resource);

    void setCurrentItem(int row);

    void loadViewSettings(const QString &prefix = QString());
    void saveViewSettings(const QString &prefix = QString());

    ViewMode viewMode() const;
    ItemSize itemSize() const;
    int itemSizeCustom() const;

    KisResourceItemChooser* resourceItemChooser() const;

    bool isNameLabelVisible() const;
    bool areEditOptionsVisible() const;

public Q_SLOTS:
    void slotUpdateIcons();

    void setViewMode(ViewMode newViewMode);
    void setItemSize(ItemSize newItemSize);
    void setItemSizeCustom(int newSize);

    void setNameLabelVisible(bool newNameLabelVisible);
    void setEditOptionsVisible(bool newEditOptionsVisible);

Q_SIGNALS:
    void resourceSelected(KoResourceSP resource);
    void resourceClicked(KoResourceSP resource);
    void gradientEdited(KoAbstractGradientSP resource);

private:
    class Private;
    QScopedPointer<Private> m_d;

    bool event(QEvent *e) override;
};

#endif

