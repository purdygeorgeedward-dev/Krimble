/*
 *  SPDX-FileCopyrightText: 2019 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef LCMSRGBP2020PQCOLORSPACE_H
#define LCMSRGBP2020PQCOLORSPACE_H

#include <colorspaces/rgb_u8/RgbU8ColorSpace.h>
#include <colorspaces/rgb_u16/RgbU16ColorSpace.h>

#ifdef HAVE_OPENEXR
#include <colorspaces/rgb_f16/RgbF16ColorSpace.h>
#endif

#include <colorspaces/rgb_f32/RgbF32ColorSpace.h>

#include "KoColorConversionTransformationFactory.h"

#include <LcmsRGBP2020PQColorSpaceTransformation.h>

template <class T>
struct ColorSpaceFromFactory {
};

template<>
struct ColorSpaceFromFactory<RgbU8ColorSpaceFactory> {
  typedef RgbU8ColorSpace type;
};

template<>
struct ColorSpaceFromFactory<RgbU16ColorSpaceFactory> {
  typedef RgbU16ColorSpace type;
};

#ifdef HAVE_OPENEXR
template<>
struct ColorSpaceFromFactory<RgbF16ColorSpaceFactory> {
  typedef RgbF16ColorSpace type;
};
#endif

template<>
struct ColorSpaceFromFactory<RgbF32ColorSpaceFactory> {
  typedef RgbF32ColorSpace type;
};

template<class T> struct NextTrait { using type = void; };
template<> struct NextTrait<KoBgrU8Traits> { using type = KoBgrU16Traits; };

#ifdef HAVE_OPENEXR
template<> struct NextTrait<KoBgrU16Traits> { using type = KoRgbF16Traits; };
template<> struct NextTrait<KoRgbF16Traits> { using type = KoRgbF32Traits; };
#else
template<> struct NextTrait<KoBgrU16Traits> { using type = KoRgbF32Traits; };
#endif

template<typename ParentColorSpace, typename CurrentTraits>
void addInternalConversion(QList<KoColorConversionTransformationFactory*> &list, const QString &targetProfileName, CurrentTraits*)
{
    list << new LcmsScaleRGBP2020PQTransformationFactory<ParentColorSpace, CurrentTraits>(targetProfileName);

    using NextTraits = typename NextTrait<CurrentTraits>::type;
    addInternalConversion<ParentColorSpace>(list, targetProfileName, static_cast<NextTraits*>(0));
}

template<typename ParentColorSpace>
void addInternalConversion(QList<KoColorConversionTransformationFactory*> &list, const QString &targetProfileName, typename ParentColorSpace::ColorSpaceTraits*)
{

    using CurrentTraits = typename ParentColorSpace::ColorSpaceTraits;
    using NextTraits = typename NextTrait<CurrentTraits>::type;
    addInternalConversion<ParentColorSpace>(list, targetProfileName, static_cast<NextTraits*>(0));
}

template<typename ParentColorSpace>
void addInternalConversion(QList<KoColorConversionTransformationFactory*> &,  const QString &, void*)
{
}

template <class BaseColorSpaceFactory>
class LcmsRGBP2020PQColorSpaceFactoryWrapper : public BaseColorSpaceFactory
{
    typedef typename ColorSpaceFromFactory<BaseColorSpaceFactory>::type RelatedColorSpaceType;
public:
    LcmsRGBP2020PQColorSpaceFactoryWrapper(const QString &targetProfileName, const QString &linearProfileName)
        : m_targetProfileName(targetProfileName), m_linearProfileName(linearProfileName) {

    }
public:
    bool isHdr() const override {
        return this->colorDepthId() != Integer8BitsColorDepthID;
    }

    QList<KoColorConversionTransformationFactory *> colorConversionLinks() const override
    {
        QList<KoColorConversionTransformationFactory *> list;

        list << new LcmsFromRGBP2020PQTransformationFactory<RelatedColorSpaceType, KoRgbF32Traits>(m_targetProfileName, m_linearProfileName);
        list << new LcmsToRGBP2020PQTransformationFactory<RelatedColorSpaceType, KoRgbF32Traits>(m_targetProfileName, m_linearProfileName);
#ifdef HAVE_OPENEXR
        list << new LcmsFromRGBP2020PQTransformationFactory<RelatedColorSpaceType, KoRgbF16Traits>(m_targetProfileName, m_linearProfileName);
        list << new LcmsToRGBP2020PQTransformationFactory<RelatedColorSpaceType, KoRgbF16Traits>(m_targetProfileName, m_linearProfileName);
#endif

        addInternalConversion<RelatedColorSpaceType>(list, m_targetProfileName, static_cast<KoBgrU8Traits*>(0));

        return list;
    }

private:
    KoColorSpace *createColorSpace(const KoColorProfile *p) const override
    {
        return new RelatedColorSpaceType(this->name(), p->clone());
    }

    QString m_targetProfileName;
    QString m_linearProfileName;
};

#endif
