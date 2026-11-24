/*
 *  SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2010 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_DEFAULT_BOUNDS_H
#define KIS_DEFAULT_BOUNDS_H

#include <QRect>
#include "kis_types.h"
#include "kis_default_bounds_base.h"

class KisDefaultBounds;
class KisSelectionDefaultBounds;
class KisSelectionEmptyBounds;
class KisWrapAroundBoundsWrapper;
typedef KisSharedPtr<KisDefaultBounds> KisDefaultBoundsSP;
typedef KisSharedPtr<KisSelectionDefaultBounds> KisSelectionDefaultBoundsSP;
typedef KisSharedPtr<KisSelectionEmptyBounds> KisSelectionEmptyBoundsSP;
typedef KisSharedPtr<KisWrapAroundBoundsWrapper> KisWrapAroundBoundsWrapperSP;

class KRITAIMAGE_EXPORT KisDefaultBounds :  public KisDefaultBoundsBase
{
public:
    KisDefaultBounds();
    KisDefaultBounds(KisImageWSP image);
    ~KisDefaultBounds() override;

    QRect bounds() const override;
    bool wrapAroundMode() const override;
    WrapAroundAxis wrapAroundModeAxis() const override;
    int currentLevelOfDetail() const override;
    int currentTime() const override;
    bool externalFrameActive() const override;
    void * sourceCookie() const override;

protected:
    friend class KisPaintDeviceTest;
    static const QRect infiniteRect;

private:
    Q_DISABLE_COPY(KisDefaultBounds)

    struct Private;
    Private * const m_d;
};

class KRITAIMAGE_EXPORT KisSelectionDefaultBoundsBase : public KisDefaultBoundsBase
{
public:
    KisSelectionDefaultBoundsBase();
    ~KisSelectionDefaultBoundsBase() override;

    QRect bounds() const override;
    QRect imageBorderRect() const override;
    bool wrapAroundMode() const override;
    WrapAroundAxis wrapAroundModeAxis() const override;
    int currentLevelOfDetail() const override;
    int currentTime() const override;
    bool externalFrameActive() const override;
    void * sourceCookie() const override;

protected:
    virtual KisPaintDeviceSP parentPaintDevice() const = 0;

private:
    Q_DISABLE_COPY(KisSelectionDefaultBoundsBase)
};

class KRITAIMAGE_EXPORT KisSelectionDefaultBounds : public KisSelectionDefaultBoundsBase
{
public:
    KisSelectionDefaultBounds(KisPaintDeviceSP parentPaintDevice);
    ~KisSelectionDefaultBounds() override;

protected:
    virtual KisPaintDeviceSP parentPaintDevice() const override;

private:
    Q_DISABLE_COPY(KisSelectionDefaultBounds)
    KisPaintDeviceWSP m_paintDevice;
};

class KRITAIMAGE_EXPORT KisMaskDefaultBounds : public KisSelectionDefaultBoundsBase
{
public:
    KisMaskDefaultBounds(KisNodeSP parentNode);
    ~KisMaskDefaultBounds() override;

protected:
    virtual KisPaintDeviceSP parentPaintDevice() const override;

private:
    Q_DISABLE_COPY(KisMaskDefaultBounds)
    KisNodeWSP m_parentNode;
};

class KRITAIMAGE_EXPORT KisSelectionEmptyBounds : public KisDefaultBounds
{
public:
    KisSelectionEmptyBounds();
    KisSelectionEmptyBounds(KisImageWSP image);
    ~KisSelectionEmptyBounds() override;
    QRect bounds() const override;
};

class KRITAIMAGE_EXPORT KisWrapAroundBoundsWrapper :  public KisDefaultBoundsBase
{
public:
    KisWrapAroundBoundsWrapper(KisDefaultBoundsBaseSP base, QRect bounds);
    ~KisWrapAroundBoundsWrapper() override;

    QRect bounds() const override;
    bool wrapAroundMode() const override;
    WrapAroundAxis wrapAroundModeAxis() const override;
    int currentLevelOfDetail() const override;
    int currentTime() const override;
    bool externalFrameActive() const override;
    void * sourceCookie() const override;

protected:
    friend class KisPaintDeviceTest;

private:
    Q_DISABLE_COPY(KisWrapAroundBoundsWrapper)

    struct Private;
    const QScopedPointer<Private> m_d;
};

#endif
