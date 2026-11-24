/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *  SPDX-FileCopyrightText: 2022 Halla Rempt <halla@valdyas.org>
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_PALETTE_VIEW_H
#define __KIS_PALETTE_VIEW_H

#include <QScopedPointer>
#include <QTableView>
#include <QPixmap>

#include <KoColorSet.h>
#include "kritawidgets_export.h"

#include <KisKineticScroller.h>

class KisPaletteModel;
class QWheelEvent;
class KoColorDisplayRendererInterface;

class KRITAWIDGETS_EXPORT KisPaletteView : public QTableView
{
    Q_OBJECT
private:
    static int MINIMUM_ROW_HEIGHT;
public:
    explicit KisPaletteView(QWidget *parent = 0);
    ~KisPaletteView() override;

    void setPaletteModel(KisPaletteModel *model);
    KisPaletteModel *paletteModel() const;

public:

    void setAllowModification(bool allow);

    void setDisplayRenderer(const KoColorDisplayRendererInterface *displayRenderer);

    void setCrossedKeyword(const QString &value);

    void removeSelectedEntry();

    void selectClosestColor(const KoColor &color);

    const KoColor closestColor(const KoColor& color) const;

    bool addEntryWithDialog(KoColor color);

    bool removeEntryWithDialog(QModelIndex index);

    bool addGroupWithDialog();

    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

Q_SIGNALS:
    void sigIndexSelected(const QModelIndex &index);
    void sigColorSelected(const KoColor &);

public Q_SLOTS:
    void slotFGColorChanged(const KoColor &);

    void slotScrollerStateChanged(QScroller::State state){KisKineticScroller::updateCursor(this, state);}

private Q_SLOTS:
    void slotHorizontalHeaderResized(int, int, int newSize);
    void slotAdditionalGuiUpdate();
    void slotCurrentSelectionChanged(const QModelIndex &newCurrent);

private:
    void resizeRows(int newSize);
    void saveModification();

private:
    struct Private;
    const QScopedPointer<Private> d;
};

#endif
