/*
 *  SPDX-FileCopyrightText: 2015 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_COMMAND_UTILS_H
#define __KIS_COMMAND_UTILS_H

#include "kundo2command.h"
#include "kis_undo_stores.h"
#include "kritacommand_export.h"
#include <functional>
#include <memory>

namespace KisCommandUtils
{

    struct KRITACOMMAND_EXPORT AggregateCommand : public KUndo2Command {
        AggregateCommand(KUndo2Command *parent = 0);
        AggregateCommand(const KUndo2MagicString &text,
                         KUndo2Command *parent = 0);

        void redo() override;
        void undo() override;

    protected:
        virtual void populateChildCommands() = 0;
        void addCommand(KUndo2Command *cmd);

    private:
        bool m_firstRedo;
        KisSurrogateUndoStore m_store;
    };

    struct KRITACOMMAND_EXPORT LambdaCommand : public AggregateCommand {
        LambdaCommand(std::function<KUndo2Command*()> createCommandFunc);
        LambdaCommand(const KUndo2MagicString &text,
                      std::function<KUndo2Command*()> createCommandFunc);
        LambdaCommand(const KUndo2MagicString &text,
                      KUndo2Command *parent,
                      std::function<KUndo2Command*()> createCommandFunc);
        LambdaCommand(KUndo2Command *parent,
                      std::function<KUndo2Command*()> createCommandFunc);

    protected:
        void populateChildCommands() override;

    private:
        std::function<KUndo2Command*()> m_createCommandFunc;
    };

    struct KRITACOMMAND_EXPORT SkipFirstRedoWrapper : public KUndo2Command {
        SkipFirstRedoWrapper(KUndo2Command *child = 0, KUndo2Command *parent = 0);
        void redo() override;
        void undo() override;

    private:
        bool m_firstRedo;
        QScopedPointer<KUndo2Command> m_child;
    };

    struct KRITACOMMAND_EXPORT SkipFirstRedoBase : public KUndo2Command {
        SkipFirstRedoBase(bool skipFirstRedo, KUndo2Command *parent = 0);
        SkipFirstRedoBase(bool skipFirstRedo, const KUndo2MagicString &text, KUndo2Command *parent = 0);

        void redo() override final;
        void undo() override final;

        void setSkipOneRedo(bool value);

    protected:
        virtual void redoImpl() = 0;
        virtual void undoImpl() = 0;

    private:
        bool m_firstRedo;
    };

    struct KRITACOMMAND_EXPORT FlipFlopCommand : public KUndo2Command {
        enum State {
            INITIALIZING,
            FINALIZING
        };

        FlipFlopCommand(State initialState, KUndo2Command *parent = 0);
        FlipFlopCommand(bool finalizing = false, KUndo2Command *parent = 0);

        void redo() override;
        void undo() override;

    protected:
        virtual void partA();
        virtual void partB();

        State getState() const { return m_currentState; }
        bool isFirstRedo() const { return m_firstRedo; }

    private:
        State m_currentState;
        bool m_firstRedo {true};
    };

    struct KRITACOMMAND_EXPORT CompositeCommand : public KUndo2Command {
        CompositeCommand(KUndo2Command *parent = 0);
        ~CompositeCommand() override;

        void addCommand(KUndo2Command *cmd);

        void redo() override;
        void undo() override;

    private:
        QVector<KUndo2Command*> m_commands;
    };

    KRITACOMMAND_EXPORT
    KUndo2Command* composeCommands(KUndo2Command *parent, KUndo2Command *cmd);

    KRITACOMMAND_EXPORT
    void redoAndMergeIntoAccumulatingCommand(KUndo2Command *cmd, std::unique_ptr<KUndo2Command> &accumulatingCommand);
}

#endif
