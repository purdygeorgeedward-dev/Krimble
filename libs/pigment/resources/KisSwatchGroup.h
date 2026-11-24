/*  This file is part of the KDE project
 *
 *  SPDX-FileCopyrightText: 2005..2022 Halla Rempt <halla@valdyas.org>
 *  SPDX-FileCopyrightText: 2016 L. E. Segovia <amy@amyspark.me>
 *  SPDX-FileCopyrightText: 2018 Michael Zhou <simerixh@gmail.com>
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KISSWATCHGROUP_H
#define KISSWATCHGROUP_H

#include "KisSwatch.h"

#include "kritapigment_export.h"

#include <QSharedPointer>
#include <QList>
#include <QMap>

#include <KisPropagateConstWrapper.h>

class KRITAPIGMENT_EXPORT KisSwatchGroup
{
public :
    struct SwatchInfo {
        QString group;
        KisSwatch swatch;
        int row;
        int column;
    };

public:

    static int DEFAULT_COLUMN_COUNT;
    static int DEFAULT_ROW_COUNT;

    ~KisSwatchGroup();
    KisSwatchGroup(const KisSwatchGroup &rhs);
    KisSwatchGroup &operator=(const KisSwatchGroup &rhs);

    void setName(const QString &name);
    QString name() const;

    void setRowCount(int newRowCount);
    int rowCount() const;

    int colorCount() const;
    int slotCount() const;

    QList<SwatchInfo> infoList() const;

    bool checkSwatchExists(int column, int row) const;

    KisSwatch getSwatch(int column, int row) const;

private:

    friend class KoColorSet;
    friend struct AddSwatchCommand;
    friend struct RemoveGroupCommand;
    friend struct RemoveSwatchCommand;
    friend struct AddGroupCommand;
    friend struct ClearCommand;
    friend struct SetColumnCountCommand;
    friend class TestKisSwatchGroup;
    friend class TestKoColorSet;

    KisSwatchGroup();

    friend class KisPaletteEditor;

    void setSwatch(const KisSwatch &e, int column, int row);

    bool removeSwatch(int column, int row);

    QPair<int, int> addSwatch(const KisSwatch &e);

    void clear();

    void setColumnCount(int columnCount);
    int columnCount() const;

    struct Private;
    std::experimental::propagate_const<std::unique_ptr<Private>> d;
};

inline QDebug operator<<(QDebug dbg, const KisSwatchGroup group)
{
    dbg.nospace() << "[Group] Name: " << group.name();
    return dbg.space();
}

typedef QSharedPointer<KisSwatchGroup> KisSwatchGroupSP;

#endif
