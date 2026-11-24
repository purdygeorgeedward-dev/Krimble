/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LIBKIS_PALETTE_H
#define LIBKIS_PALETTE_H

#include <QObject>

#include "kritalibkis_export.h"
#include "libkis.h"
#include "Resource.h"
#include "KoColorSet.h"
#include <Swatch.h>

class ManagedColor;

class KRITALIBKIS_EXPORT Palette : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Palette)

public:
    explicit Palette(Resource *resource, QObject *parent = 0);
    ~Palette() override;

    bool operator==(const Palette &other) const;
    bool operator!=(const Palette &other) const;

public Q_SLOTS:

    int numberOfEntries() const;

    int columnCount();
    void setColumnCount(int columns);

    int rowCount();

    int rowCountGroup(QString name);

    void setRowCountGroup(int rows, QString name);

    QString comment();

    void setComment(QString comment);

    QStringList groupNames() const;

    void addGroup(QString name);

    void removeGroup(QString name, bool keepColors = true);

    int colorsCountTotal();

     int colorsCountGroup(QString name);

    int slotCount();

    int slotCountGroup(QString name);

    Q_DECL_DEPRECATED Swatch *colorSetEntryByIndex(int index);

    Swatch *entryByIndex(int index);

    Q_DECL_DEPRECATED Swatch *colorSetEntryFromGroup(int index, const QString &groupName);

    Swatch *entryByIndexFromGroup(int index, const QString &groupName);

    void addEntry(Swatch entry, QString groupName = QString());

     void removeEntry(int index);

    void removeEntryFromGroup(int index, const QString &groupName);

    Q_DECL_DEPRECATED void changeGroupName(QString oldGroupName, QString newGroupName);

    void renameGroup(QString oldGroupName, QString newGroupName);

    void moveGroup(const QString &groupName, const QString &groupNameInsertBefore = QString());

    bool save();

private:
    friend class PaletteView;
    struct Private;
    Private *const d;

    KoColorSetSP colorSet();

};

#endif
