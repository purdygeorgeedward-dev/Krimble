/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LIBKIS_PALETTE_VIEW_H
#define LIBKIS_PALETTE_VIEW_H

#include <QObject>
#include <QScopedPointer>
#include "kritalibkis_export.h"
#include "libkis.h"
#include "Palette.h"
#include "ManagedColor.h"
#include <kis_palette_view.h>
#include <KisPaletteModel.h>

#include <Swatch.h>

class KisSwatch;

class KRITALIBKIS_EXPORT PaletteView : public QWidget
{
    Q_OBJECT
public:
    PaletteView(QWidget *parent = 0);
    ~PaletteView();
public Q_SLOTS:
    void setPalette(Palette *palette);
    bool addEntryWithDialog(ManagedColor *color);
    bool addGroupWithDialog();
    bool removeSelectedEntryWithDialog();
    void trySelectClosestColor(ManagedColor *color);
Q_SIGNALS:
    void entrySelectedForeGround(Swatch entry);
    void entrySelectedBackGround(Swatch entry);

private Q_SLOTS:

    void fgSelected(QModelIndex index);

private:

    struct Private;
    const QScopedPointer<Private> d;
};

#endif
