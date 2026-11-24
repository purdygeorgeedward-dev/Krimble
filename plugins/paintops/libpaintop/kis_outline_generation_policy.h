/*
 *  SPDX-FileCopyrightText: 2013 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_OUTLINE_GENERATION_POLICY_H
#define __KIS_OUTLINE_GENERATION_POLICY_H

#include <kis_current_outline_fetcher.h>

template <class ParentClass>
class KisOutlineGenerationPolicy : public ParentClass
{
public:
    KisOutlineGenerationPolicy(KisCurrentOutlineFetcher::Options options,
                               KisResourcesInterfaceSP resourcesInterface)
        : ParentClass(resourcesInterface),
          m_outlineFetcher(options)
    {
    }

    ~KisOutlineGenerationPolicy() override
    {
    }

    const KisCurrentOutlineFetcher *outlineFetcher() const
    {
        return &m_outlineFetcher;
    }

    void onPropertyChanged() override
    {
        m_outlineFetcher.setDirty();
        ParentClass::onPropertyChanged();
    }

private:
    KisCurrentOutlineFetcher m_outlineFetcher;
};

#endif
