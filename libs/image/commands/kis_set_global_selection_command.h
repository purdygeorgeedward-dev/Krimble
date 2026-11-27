/*
 *  SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_SET_GLOBAL_SELECTION_COMMAND_H
#define KIS_SET_GLOBAL_SELECTION_COMMAND_H

#include <kritaimage_export.h>
#include <kundo2command.h>
#include "kis_types.h"
#include "kis_command_utils.h"

class KRITAIMAGE_EXPORT KisSetGlobalSelectionCommand : public KisCommandUtils::AggregateCommand
{

public:
    KisSetGlobalSelectionCommand(KisImageWSP image, KisSelectionSP selection);

    void populateChildCommands() override;

private:
    KisImageWSP m_image;
    KisSelectionSP m_newSelection;
    KisSelectionSP m_oldSelection;
};

class KRITAIMAGE_EXPORT KisSetEmptyGlobalSelectionCommand : public KisSetGlobalSelectionCommand
{
public:
    KisSetEmptyGlobalSelectionCommand(KisImageWSP image);
};

#endif
