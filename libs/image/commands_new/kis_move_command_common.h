/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@kde.org>
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_MOVE_COMMAND_COMMON_H
#define KIS_MOVE_COMMAND_COMMON_H

#include <QPoint>
#include <klocalizedstring.h>
#include "kundo2command.h"
#include "kritaimage_export.h"
#include "kis_types.h"

template <class ObjectSP>
class KRITAIMAGE_EXPORT KisMoveCommandCommon : public KUndo2Command
{
public:
    KisMoveCommandCommon(ObjectSP object, const QPoint& oldPos, const QPoint& newPos, KUndo2Command *parent = 0)
        : KUndo2Command(kundo2_i18n("Move"), parent),
          m_oldPos(oldPos),
          m_newPos(newPos),
          m_object(object)
    {
    }

    void redo() override {
        moveTo(m_newPos);
    }

    void undo() override {
        moveTo(m_oldPos);
    }

private:
    void moveTo(const QPoint& pos) {
        m_object->setX(pos.x());
        m_object->setY(pos.y());
    }

private:
    QPoint m_oldPos;
    QPoint m_newPos;

protected:
    ObjectSP m_object;
};

#endif
