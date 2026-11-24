/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_PROCESSING_INFORMATION_H_
#define _KIS_PROCESSING_INFORMATION_H_

#include "kis_types.h"

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisConstProcessingInformation
{
public:
    KisConstProcessingInformation(const KisPaintDeviceSP device, const QPoint& topLeft, const KisSelectionSP selection);
    KisConstProcessingInformation(const KisConstProcessingInformation& _rhs);
    KisConstProcessingInformation& operator=(const KisConstProcessingInformation& _rhs);
    ~KisConstProcessingInformation();
    const KisPaintDeviceSP paintDevice() const;

    const KisSelectionSP selection() const;
    const QPoint& topLeft() const;
private:
    struct Private;
    Private* const d;
};

class KRITAIMAGE_EXPORT KisProcessingInformation : public KisConstProcessingInformation
{
public:
    KisProcessingInformation(KisPaintDeviceSP device, const QPoint& topLeft, const KisSelectionSP selection);
    KisProcessingInformation(const KisProcessingInformation& _rhs);
    KisProcessingInformation& operator=(const KisProcessingInformation& _rhs);
    ~KisProcessingInformation();
    KisPaintDeviceSP paintDevice();
private:
    struct Private;
    Private* const d;
};

#endif
