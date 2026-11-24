/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KISBRUSHCOLORWELLS_H
#define KISBRUSHCOLORWELLS_H

#include <QObject>

class KisViewManager;

class KisBrushColorWells : public QObject
{
    Q_OBJECT
public:
    explicit KisBrushColorWells(KisViewManager *viewManager);
};

#endif
