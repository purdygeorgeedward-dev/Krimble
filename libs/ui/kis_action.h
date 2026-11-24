/*
 *  SPDX-FileCopyrightText: 2013 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_ACTION_H
#define KIS_ACTION_H

#include <QWidgetAction>
#include <kritaui_export.h>
#include <kis_debug.h>
#include <QIcon>
class KisActionManager;

class KRITAUI_EXPORT KisAction : public QWidgetAction
{
    Q_OBJECT
public:

    enum ActivationFlag {
        NONE                        = 0x0000,
        ACTIVE_IMAGE                = 0x0001,
        MULTIPLE_IMAGES             = 0x0002,
        CURRENT_IMAGE_MODIFIED      = 0x0004,
        ACTIVE_NODE                 = 0x0008,
        ACTIVE_DEVICE               = 0x0010,
        ACTIVE_LAYER                = 0x0020,
        ACTIVE_TRANSPARENCY_MASK    = 0x0040,
        ACTIVE_SHAPE_LAYER          = 0x0080,
        PIXELS_SELECTED             = 0x0100,
        SHAPES_SELECTED             = 0x0200,
        ANY_SELECTION_WITH_PIXELS   = 0x0400,
        PIXELS_IN_CLIPBOARD         = 0x0800,
        SHAPES_IN_CLIPBOARD         = 0x1000,
        NEVER_ACTIVATE              = 0x2000,
        LAYERS_IN_CLIPBOARD         = 0x4000,
        IMAGE_HAS_ANIMATION         = 0x8000,
        SHAPE_SELECTION_WITH_SHAPES = 0x10000,
        PIXEL_SELECTION_WITH_PIXELS = 0x20000,
        IMAGE_CAN_RESELECT          = 0x40000,
        IMAGE_IS_WRITABLE           = 0x80000,
    };
    Q_DECLARE_FLAGS(ActivationFlags, ActivationFlag)

    enum ActivationCondition {
        NO_CONDITION = 0,
        ACTIVE_NODE_EDITABLE = 0x1,
        ACTIVE_NODE_EDITABLE_PAINT_DEVICE = 0x2,
        SELECTION_EDITABLE = 0x4,
        OPENGL_ENABLED = 0x8,
    };
    Q_DECLARE_FLAGS(ActivationConditions, ActivationCondition)

    explicit KisAction(QObject* parent = 0);
    KisAction(const QString& text, QObject* parent = 0);
    KisAction(const QIcon& icon, const QString& text, QObject* parent = 0);
    ~KisAction() override;

    void setDefaultShortcut(const QKeySequence & shortcut);
    QKeySequence defaultShortcut() const;

    void setActivationFlags(ActivationFlags flags);
    ActivationFlags activationFlags();

    void setActivationConditions(ActivationConditions conditions);
    ActivationConditions activationConditions();

    void setExcludedNodeTypes(const QStringList &nodeTypes);
    const QStringList& excludedNodeTypes() const;

    virtual void setActionEnabled(bool enabled);

    void setOperationID(const QString& id);

    QString operationID() const;

Q_SIGNALS:
    void sigEnableSlaves(bool value);

private Q_SLOTS:
    void slotTriggered();
    void slotChanged();

private:

    friend class KisActionManager;

     void setActionManager(KisActionManager* actionManager);

    class Private;
    Private* const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KisAction::ActivationFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(KisAction::ActivationConditions)

#endif
