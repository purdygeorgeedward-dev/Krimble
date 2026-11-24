/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISLOCKFRAMEGENERATIONLOCK_H
#define KISLOCKFRAMEGENERATIONLOCK_H

#include <kritaimage_export.h>
#include <KisAdaptedLock.h>

class KisImageAnimationInterface;

class KRITAIMAGE_EXPORT KisLockFrameGenerationLockAdapter
{
public:
    KisLockFrameGenerationLockAdapter(KisImageAnimationInterface *interface);

    bool try_lock();
    void lock();
    void unlock();

private:
    KisImageAnimationInterface *m_interface {nullptr};
};

KIS_DECLARE_ADAPTED_LOCK(KisLockFrameGenerationLock, KisLockFrameGenerationLockAdapter)

#endif
