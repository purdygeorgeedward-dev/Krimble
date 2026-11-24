/*
 *  SPDX-FileCopyrightText: 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_STROKE_STRATEGY_UNDO_COMMAND_BASED_H
#define __KIS_STROKE_STRATEGY_UNDO_COMMAND_BASED_H

#include <kundo2command.h>
#include <QVector>
#include <QMutex>

#include "kis_types.h"
#include "kis_simple_stroke_strategy.h"
#include "KisRunnableBasedStrokeStrategy.h"

#include <memory>

class KisStrokeJob;
class KisSavedMacroCommand;
class KisStrokeUndoFacade;
class KisStrokesQueueMutatedJobInterface;

class KRITAIMAGE_EXPORT KisStrokeStrategyUndoCommandBased : public KisRunnableBasedStrokeStrategy
{
public:
    struct MutatedCommandInterface
    {
        virtual ~MutatedCommandInterface() {}

        void setRunnableJobsInterface(KisRunnableStrokeJobsInterface *interface) {
            m_mutatedJobsInterface = interface;
        }

        KisRunnableStrokeJobsInterface* runnableJobsInterface() const {
            return m_mutatedJobsInterface;
        }

    private:
        KisRunnableStrokeJobsInterface *m_mutatedJobsInterface;
    };

    class KRITAIMAGE_EXPORT Data : public KisStrokeJobData {
    public:
        Data(KUndo2CommandSP _command,
             bool _undo = false,
             Sequentiality _sequentiality = SEQUENTIAL,
             Exclusivity _exclusivity = NORMAL,
             bool _shouldGoToHistory = true)
            : KisStrokeJobData(_sequentiality, _exclusivity),
              command(_command),
              undo(_undo),
              shouldGoToHistory(_shouldGoToHistory)
        {
        }

        Data(KUndo2Command *_command,
             bool _undo = false,
             Sequentiality _sequentiality = SEQUENTIAL,
             Exclusivity _exclusivity = NORMAL,
             bool _shouldGoToHistory = true)
            : KisStrokeJobData(_sequentiality, _exclusivity),
              command(_command),
              undo(_undo),
              shouldGoToHistory(_shouldGoToHistory)
        {
        }

        ~Data() override;

        KUndo2CommandSP command;
        bool undo;
        bool shouldGoToHistory = true;
    };

public:
    KisStrokeStrategyUndoCommandBased(const KUndo2MagicString &name,
                                      bool undo,
                                      KisStrokeUndoFacade *undoFacade,
                                      KUndo2CommandSP initCommand = KUndo2CommandSP(0),
                                      KUndo2CommandSP finishCommand = KUndo2CommandSP(0));

    using KisSimpleStrokeStrategy::setExclusive;

    void initStrokeCallback() override;
    void finishStrokeCallback() override;
    void cancelStrokeCallback() override;
    void doStrokeCallback(KisStrokeJobData *data) override;

    void setCommandExtraData(KUndo2CommandExtraData *data);

    void setMacroId(int value);

    using KisStrokeStrategy::setSupportsWrapAroundMode;

    void setUsedWhileUndoRedo(bool value);

protected:
    void runAndSaveCommand(KUndo2CommandSP command,
                           KisStrokeJobData::Sequentiality sequentiality,
                           KisStrokeJobData::Exclusivity exclusivity);
    void notifyCommandDone(KUndo2CommandSP command,
                           KisStrokeJobData::Sequentiality sequentiality,
                           KisStrokeJobData::Exclusivity exclusivity);

    KisStrokeStrategyUndoCommandBased(const KisStrokeStrategyUndoCommandBased &rhs);

    virtual void postProcessToplevelCommand(KUndo2Command *command);

    KisStrokeUndoFacade* undoFacade() const;

protected:
    void executeCommand(KUndo2CommandSP command, bool undo);
    void cancelStrokeCallbackImpl(QVector<KisStrokeJobData*> &mutatedJobs);
private:
    bool m_undo;
    KUndo2CommandSP m_initCommand;
    KUndo2CommandSP m_finishCommand;
    KisStrokeUndoFacade *m_undoFacade {nullptr};

    std::unique_ptr<KUndo2CommandExtraData> m_commandExtraData;
    int m_macroId;

    QMutex m_mutex;
    KisSavedMacroCommand *m_macroCommand {nullptr};
};

#endif
