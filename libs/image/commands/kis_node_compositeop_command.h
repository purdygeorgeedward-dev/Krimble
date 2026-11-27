/*
 *  SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *  SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_NODE_COMPOSITEOP_COMMAND_H
#define KIS_NODE_COMPOSITEOP_COMMAND_H

#include "kis_node_command.h"
#include "commands_new/KisAsynchronouslyMergeableCommandInterface.h"
#include <boost/optional.hpp>

class KRITAIMAGE_EXPORT KisNodeCompositeOpCommand : public KisNodeCommand, public KisAsynchronouslyMergeableCommandInterface
{

public:
    KisNodeCompositeOpCommand(KisNodeSP node, const QString& newCompositeOp);

    void redo() override;
    void undo() override;

    int id() const override;
    bool mergeWith(const KUndo2Command *command) override;
    bool canMergeWith(const KUndo2Command *command) const override;

    bool canAnnihilateWith(const KUndo2Command *command) const override;

private:
    void setCompositeOpImpl(const QString &compositeOp);

private:
    boost::optional<QString> m_oldCompositeOp;
    QString m_newCompositeOp;
};

#endif
