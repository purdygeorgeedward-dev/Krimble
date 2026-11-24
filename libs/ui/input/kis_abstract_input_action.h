/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_ABSTRACT_INPUT_ACTION_H
#define KIS_ABSTRACT_INPUT_ACTION_H

#include <QHash>
#include <QPoint>
#include "kritaui_export.h"

#include "KisInputActionGroup.h"

class QPointF;
class QEvent;
class KisInputManager;

class KRITAUI_EXPORT KisAbstractInputAction
{
public:
    explicit KisAbstractInputAction(const QString &id);
    virtual ~KisAbstractInputAction();

    virtual void activate(int shortcut);

    virtual void deactivate(int shortcut);

    virtual void begin(int shortcut, QEvent *event);
    virtual void end(QEvent *event);
    virtual void inputEvent(QEvent* event);

    virtual bool supportsHiResInputEvents(int shortcut) const;

    virtual KisInputActionGroup inputActionGroup(int shortcut) const;

    virtual QHash<QString, int> shortcutIndexes() const;

    virtual QString id() const;

    virtual QString name() const;

    virtual QString description() const;

    virtual int priority() const;

    virtual bool canIgnoreModifiers() const;

    virtual bool isShortcutRequired(int shortcut) const;

    virtual bool isAvailable() const;

protected:
    KisInputManager *inputManager() const;
    void setName(const QString &name);
    void setDescription(const QString &description);
    void setShortcutIndexes(const QHash<QString, int> &indexes);

    virtual void cursorMoved(const QPointF &lastPos, const QPointF &pos);
    virtual void cursorMovedAbsolute(const QPointF &startPos, const QPointF &pos);

    QPoint eventPos(const QEvent *event);

    QPointF eventPosF(const QEvent *event);

private:
    friend class KisInputManager;
    static void setInputManager(KisInputManager *manager);

    class Private;
    Private * const d;
};

#endif
