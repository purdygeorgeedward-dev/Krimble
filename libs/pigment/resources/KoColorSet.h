/*  This file is part of the KDE project
 *  SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2016 L. E. Segovia <amy@amyspark.me>
 *  SPDX-FileCopyrightText: 2022 Halla Rempt <halla@valdyas.org>
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef KOCOLORSET
#define KOCOLORSET

#include <QObject>
#include <QColor>
#include <QScopedPointer>
#include <QSharedPointer>

#include <KoResource.h>
#include "KoColor.h"
#include "KisSwatch.h"
#include "KisSwatchGroup.h"

class KUndo2Stack;

class KRITAPIGMENT_EXPORT KoColorSet : public QObject, public KoResource
{
    Q_OBJECT

public:
    static const QString GLOBAL_GROUP_NAME;
    static const QString KPL_VERSION_ATTR;
    static const QString KPL_GROUP_ROW_COUNT_ATTR;
    static const QString KPL_PALETTE_COLUMN_COUNT_ATTR;
    static const QString KPL_PALETTE_NAME_ATTR;
    static const QString KPL_PALETTE_COMMENT_ATTR;
    static const QString KPL_PALETTE_FILENAME_ATTR;
    static const QString KPL_PALETTE_READONLY_ATTR;
    static const QString KPL_COLOR_MODEL_ID_ATTR;
    static const QString KPL_COLOR_DEPTH_ID_ATTR;
    static const QString KPL_GROUP_NAME_ATTR;
    static const QString KPL_SWATCH_ROW_ATTR;
    static const QString KPL_SWATCH_COL_ATTR;
    static const QString KPL_SWATCH_NAME_ATTR;
    static const QString KPL_SWATCH_SPOT_ATTR;
    static const QString KPL_SWATCH_ID_ATTR;
    static const QString KPL_SWATCH_BITDEPTH_ATTR;
    static const QString KPL_PALETTE_PROFILE_TAG;
    static const QString KPL_SWATCH_POS_TAG;
    static const QString KPL_SWATCH_TAG;
    static const QString KPL_GROUP_TAG;
    static const QString KPL_PALETTE_TAG;

public:
    enum PaletteType {
        UNKNOWN = 0,
        GPL,
        RIFF_PAL,
        ACT,
        PSP_PAL,
        ACO,
        XML,
        KPL,
        SBZ,
        ASE,
        ACB,
        CSS
    };

    explicit KoColorSet(const QString &filename = QString());

    KoColorSet(const KoColorSet& rhs);

    ~KoColorSet() override;

    KoColorSet &operator=(const KoColorSet &rhs) = delete;

    KoResourceSP clone() const override;

    bool loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface) override;
    bool saveToDevice(QIODevice* dev) const override;
    QString defaultFileExtension() const override;
    QPair<QString, QString> resourceType() const override
    {
        return QPair<QString, QString>(ResourceType::Palettes, "");
    }

    KUndo2Stack *undoStack() const;

    void updateThumbnail() override;

    void setLocked(bool lock);
    bool isLocked() const;

    void setColumnCount(int columns);
    int columnCount() const;

    void setComment(QString comment);
    QString comment();

    int rowCount() const;

    int rowCountWithTitles() const;

    quint32 colorCount() const;
    quint32 slotCount() const;

    PaletteType paletteType() const;
    void setPaletteType(PaletteType paletteType);

    bool fromByteArray(QByteArray &data, KisResourcesInterfaceSP resourcesInterface);

    void addSwatch(const KisSwatch &swatch, const QString &groupName = GLOBAL_GROUP_NAME, int column = -1, int row = -1);

    void removeSwatch(int column, int row, KisSwatchGroupSP group);

    KisSwatchGroupSP getGroup(const QString &name) const;

    KisSwatchGroupSP getGroup(int row) const;

    KisSwatchGroupSP getGlobalGroup() const;

    void changeGroupName(const QString &oldGroupName, const QString &newGroupName);

    void addGroup(const QString &groupName, int columnCount = KisSwatchGroup::DEFAULT_COLUMN_COUNT, int rowCount = KisSwatchGroup::DEFAULT_ROW_COUNT);

    void moveGroup(const QString &groupName, const QString &groupNameInsertBefore = GLOBAL_GROUP_NAME);

    void removeGroup(const QString &groupName, bool keepColors = true);

    void clear();

    KisSwatchGroup::SwatchInfo getClosestSwatchInfo(KoColor compare, bool useGivenColorSpace = true) const;

    KisSwatch getColorGlobal(quint32 column, quint32 row) const;

    KisSwatch getSwatchFromGroup(quint32 column, quint32 row, QString groupName = KoColorSet::GLOBAL_GROUP_NAME) const;

    QStringList swatchGroupNames() const;

    bool isGroupTitleRow(int row) const;

    int startRowForGroup(const QString &groupName) const;

    int rowNumberInGroup(int rowNumber) const;

Q_SIGNALS:

    void modified();
    void layoutAboutToChange();
    void layoutChanged();
    void entryChanged(int column, int row);

private Q_SLOTS:

    void canUndoChanged(bool canUndo);
    void canRedoChanged(bool canRedo);

private:

    void setModified(bool);
    void notifySwatchChanged(const QString& groupName, int column, int row);

    friend struct AddSwatchCommand;
    friend struct RemoveSwatchCommand;
    friend struct ChangeGroupNameCommand;
    friend struct AddGroupCommand;
    friend struct RemoveGroupCommand;
    friend struct ClearCommand;
    friend struct SetColumnCountCommand;
    friend struct SetCommentCommand;
    friend struct SetPaletteTypeCommand;
    friend struct MoveGroupCommand;
    friend class TestKoColorSet;
    friend class TestKisPaletteModel;

    class Private;
    const QScopedPointer<Private> d;

};

typedef QSharedPointer<KoColorSet> KoColorSetSP;

#endif
