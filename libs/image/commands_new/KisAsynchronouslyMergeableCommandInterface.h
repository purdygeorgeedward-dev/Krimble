/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISASYNCHRONOUSLYMERGEABLECOMMANDINTERFACE_H
#define KISASYNCHRONOUSLYMERGEABLECOMMANDINTERFACE_H

class KUndo2Command;

class KisAsynchronouslyMergeableCommandInterface
{
public:
    virtual ~KisAsynchronouslyMergeableCommandInterface();

    virtual bool canMergeWith(const KUndo2Command *command) const = 0;
};

#endif
