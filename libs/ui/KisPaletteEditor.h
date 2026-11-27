/*
 *  SPDX-FileCopyrightText: 2018 Michael Zhou <simeirxh@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISPALETTEMANAGER_H
#define KISPALETTEMANAGER_H

#include <QObject>
#include <QScopedPointer>
#include <KisSwatch.h>

#include <kritaui_export.h>

class KoColorSet;
class KisPaletteModel;
class KisViewManager;
class KisSwatchGroup;
class KisViewManager;

class KRITAUI_EXPORT KisPaletteEditor : public QObject
{
    Q_OBJECT
public:
    struct PaletteInfo;

public:
    explicit KisPaletteEditor(QObject *parent = 0);
    ~KisPaletteEditor();

    void setPaletteModel(KisPaletteModel *model);
    void setView(KisViewManager *view);

    KoColorSetSP addPalette();
    KoColorSetSP importPalette();
    void removePalette(KoColorSetSP );

    int rowCountOfGroup(const QString &originalName) const;

    QString oldNameFromNewName(const QString &newName) const;

    void rename(const QString &newName);

    void changeColumnCount(int);

    QString addGroup();

    bool removeGroup(const QString &name);

    QString renameGroup(const QString &oldName);

    void changeGroupRowCount(const QString &name, int newRowCount);

    void setStorageLocation(QString location);

    void setEntry(const KoColor &color, const QModelIndex &index);

    void removeEntry(const QModelIndex &index);

    void modifyEntry(const QModelIndex &index);

    void addEntry(const KoColor &color);

    bool isModified() const;

    void startEditing();
    void endEditing(bool applyChanges = true);
    void clearStagedChanges();

    void saveNewPaletteVersion();

private Q_SLOTS:
    void slotGroupNameChanged(const QString &newName);
    void slotPaletteChanged();
    void slotSetDocumentModified();

private:
    QString newGroupName() const;
    bool duplicateExistsGroupName(const QString &name) const;
    bool duplicateExistsOriginalGroupName(const QString &name) const;
    QString filenameFromPath(const QString &path) const;

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

#endif
