/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISBLOCKBACKGROUNDFRAMEGENERATIONLOCK_H
#define KISBLOCKBACKGROUNDFRAMEGENERATIONLOCK_H

#include <kritaimage_export.h>
#include <KisAdaptedLock.h>

class KisImageAnimationInterface;

class KRITAIMAGE_EXPORT KisBlockBackgroundFrameGenerationLockAdapter
{
public:
    KisBlockBackgroundFrameGenerationLockAdapter(KisImageAnimationInterface *interface);

    void lock();
    void unlock();

private:
    KisImageAnimationInterface *m_interface;
};

KIS_DECLARE_ADAPTED_LOCK(KisBlockBackgroundFrameGenerationLock, KisBlockBackgroundFrameGenerationLockAdapter)

#endif
