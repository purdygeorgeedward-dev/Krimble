/*
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2005 Bart Coppens <kde@bartcoppens.be>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "kis_imagepipe_brush.h"
#include "kis_pipebrush_parasite.h"
#include "kis_brushes_pipe.h"
#include <KisOptimizedBrushOutline.h>

class KisImageBrushesPipe : public KisBrushesPipe<KisGbrBrush>
{
public:
    KisImageBrushesPipe()
        : m_currentBrushIndex(0)
        , m_isInitialized(false)
    {
    }

protected:
    static int selectPre(KisParasite::SelectionMode mode,
                         int index, int rank,
                         const KisPaintInformation& info) {

        qreal angle;
        qreal velocity;
        qreal capSpeed = 3;

        switch (mode) {
        case KisParasite::Constant:
        case KisParasite::Incremental:
        case KisParasite::Random:
            break;
        case KisParasite::Pressure:
            index = static_cast<int>(info.pressure() * (rank - 1) + 0.5);
            break;
        case KisParasite::Angular:
            angle = info.drawingAngle() + M_PI_2 + M_PI_4;
            angle = normalizeAngle(angle);

            index = static_cast<int>(angle / (2.0 * M_PI) * rank);
            break;
        case KisParasite::TiltX:
            index = qRound(info.xTilt() / 2.0 * rank) + rank / 2;
            break;
        case KisParasite::TiltY:
            index = qRound(info.yTilt() / 2.0 * rank) + rank / 2;
            break;
        case KisParasite::Velocity:
            velocity = log(info.drawingSpeed() + 1);
            if (velocity > capSpeed) {
                velocity = capSpeed;
            }
            velocity /= capSpeed;
            velocity *= (rank - 1) + 0.5;
            index = qRound(velocity);
            break;
        default:
            warnImage << "Parasite" << mode << "is not implemented";
            index = 0;
        }

        return index;
    }

    static int selectPost(KisParasite::SelectionMode mode,
                          int index, int rank,
                          KisRandomSourceSP randomSource,
                          int seqNo) {

        switch (mode) {
        case KisParasite::Constant: break;
        case KisParasite::Incremental:
            index = (seqNo >= 0 ? seqNo : (index + 1)) % rank;
            break;
        case KisParasite::Random:
            index = randomSource->generate(0, rank-1);
            break;
        case KisParasite::Pressure:
        case KisParasite::Angular:
            break;
        case KisParasite::TiltX:
        case KisParasite::TiltY:
        case KisParasite::Velocity:
            break;
        default:
            warnImage << "Parasite" << mode << "is not implemented";
            index = 0;
        }

        return index;
    }

    int chooseNextBrush(const KisPaintInformation& info) override {
        quint32 brushIndex = 0;

        KIS_SAFE_ASSERT_RECOVER(m_isInitialized) {
            updateBrushIndexes(info.randomSource(), 0);
        }

        for (int i = 0; i < m_parasite.dim; i++) {
            int index = selectPre(m_parasite.selection[i],
                                  m_parasite.index[i],
                                  m_parasite.rank[i], info);

            brushIndex += m_parasite.brushesCount[i] * index;
        }
        brushIndex %= (quint32)m_brushes.size();
        m_currentBrushIndex = brushIndex;
        return brushIndex;
    }

    void updateBrushIndexes(KisRandomSourceSP randomSource, int seqNo) override {
        m_isInitialized = true;

        for (int i = 0; i < m_parasite.dim; i++) {
            m_parasite.index[i] = selectPost(m_parasite.selection[i],
                                             m_parasite.index[i],
                                             m_parasite.rank[i],
                                             randomSource,
                                             seqNo);
        }
    }

public:
    using KisBrushesPipe<KisGbrBrush>::addBrush;
    using KisBrushesPipe<KisGbrBrush>::sizeBrush;

    int currentBrushIndex() override {
        return m_currentBrushIndex;
    }

    void setParasite(const KisPipeBrushParasite& parasite) {
        m_parasite = parasite;
    }

    const KisPipeBrushParasite& parasite() const {
        return m_parasite;
    }

    void setAdjustmentMidPoint(quint8 value) {
        Q_FOREACH (KisGbrBrushSP brush, m_brushes) {
            brush->setAdjustmentMidPoint(value);
        }
    }

    void setBrightnessAdjustment(qreal value) {
        Q_FOREACH (KisGbrBrushSP brush, m_brushes) {
            brush->setBrightnessAdjustment(value);
        }
    }

    void setContrastAdjustment(qreal value) {
        Q_FOREACH (KisGbrBrushSP brush, m_brushes) {
            brush->setContrastAdjustment(value);
        }
    }

    void setAutoAdjustMidPoint(bool value) {
        Q_FOREACH (KisGbrBrushSP brush, m_brushes) {
            brush->setAutoAdjustMidPoint(value);
        }
    }

    void makeMaskImage(bool preserveAlpha) {
        Q_FOREACH (KisGbrBrushSP brush, m_brushes) {
            brush->makeMaskImage(preserveAlpha);
        }
    }

    bool saveToDevice(QIODevice* dev) const {
        Q_FOREACH (KisGbrBrushSP brush, m_brushes) {
            if (!brush->saveToDevice(dev)) {
                return false;
            }
        }
        return true;
    }

    void coldInitBrush() {
        Q_FOREACH (KisGbrBrushSP brush, m_brushes) {
            brush->coldInitBrush();
        }
    }

    void notifyStrokeStarted() override {
        m_isInitialized = false;
    }

private:
    KisPipeBrushParasite m_parasite;
    int m_currentBrushIndex;
    bool m_isInitialized;
};

struct KisImagePipeBrush::Private {
public:
    KisImageBrushesPipe brushesPipe;
};

KisImagePipeBrush::KisImagePipeBrush(const QString& filename)
    : KisGbrBrush(filename)
    , d(new Private())
{
}

KisImagePipeBrush::KisImagePipeBrush(const QString& name, int w, int h,
                                     QVector< QVector<KisPaintDevice*> > devices,
                                     QVector<KisParasite::SelectionMode > modes)
    : KisGbrBrush(QString())
    , d(new Private())
{
    Q_ASSERT(devices.count() == modes.count());
    Q_ASSERT(devices.count() > 0);
    Q_ASSERT(devices.count() < 2);

    setName(name);

    KisPipeBrushParasite parasite;

    parasite.dim = devices.count();
    parasite.ncells = devices.at(0).count();
    parasite.rank[0] = parasite.ncells;
    parasite.selection[0] = modes.at(0);

    parasite.setBrushesCount();

    setParasite(parasite);
    setDevices(devices, w, h);
    setBrushTipImage(d->brushesPipe.firstBrush()->brushTipImage());
}

KisImagePipeBrush::KisImagePipeBrush(const KisImagePipeBrush& rhs)
    : KisGbrBrush(rhs),
      d(new Private(*rhs.d))
{
}

KoResourceSP KisImagePipeBrush::clone() const
{
    return KoResourceSP(new KisImagePipeBrush(*this));
}

KisImagePipeBrush::~KisImagePipeBrush()
{
    delete d;
}

bool KisImagePipeBrush::loadFromDevice(QIODevice *dev, KisResourcesInterfaceSP resourcesInterface)
{
    Q_UNUSED(resourcesInterface);

    QByteArray data = dev->readAll();
    return initFromData(data);
}

bool KisImagePipeBrush::initFromData(const QByteArray &data)
{
    if (data.size() == 0) return false;

    QByteArray line1;

    qint32 i = 0;

    while (i < data.size() && data[i] != '\n') {
        line1.append(data[i]);
        i++;
    }
    setName(QString::fromUtf8(line1, line1.size()));

    i++;

    QByteArray line2;
    while (i < data.size() && data[i] != '\n') {
        line2.append(data[i]);
        i++;
    }

    QString paramline = QString::fromUtf8(line2, line2.size());
    qint32 numOfBrushes = paramline.left(paramline.indexOf(' ')).toUInt();
    QString parasiteString = paramline.mid(paramline.indexOf(' ') + 1);

    KisPipeBrushParasite parasite = KisPipeBrushParasite(parasiteString);
    parasite.sanitize();

    parasiteSelectionString = parasite.selectionMode;

    d->brushesPipe.setParasite(parasite);
    i++;

    for (int brushIndex = d->brushesPipe.sizeBrush();
            brushIndex < numOfBrushes && i < data.size(); brushIndex++) {

        KisGbrBrushSP brush = KisGbrBrushSP(new KisGbrBrush(name() + '_' + QString().setNum(brushIndex),
                                             data,
                                             i));

        d->brushesPipe.addBrush(brush);
    }

    if (numOfBrushes > 0) {
        setValid(true);
        setSpacing(d->brushesPipe.lastBrush()->spacing());
        setWidth(d->brushesPipe.firstBrush()->width());
        setHeight(d->brushesPipe.firstBrush()->height());
        setBrushTipImage(d->brushesPipe.firstBrush()->brushTipImage());
        setBrushApplication(d->brushesPipe.firstBrush()->brushApplication());
        setBrushType(d->brushesPipe.isImageType() ? PIPE_IMAGE : PIPE_MASK);
        setHasColorAndTransparency(d->brushesPipe.hasColorAndTransparency());
    }

    return true;
}

bool KisImagePipeBrush::saveToDevice(QIODevice* dev) const
{
    QByteArray utf8Name = name().toUtf8();
    char const* name = utf8Name.data();
    int len = qstrlen(name);

    if (d->brushesPipe.parasite().dim >= KisPipeBrushParasite::MaxDim) {
        warnImage << "Save to file for pipe brushes with dim != not yet supported!";
        return false;
    }

    if (dev->write(name, len) == -1)
        return false;

    if (!dev->putChar('\n'))
        return false;

    if (!d->brushesPipe.parasite().saveToDevice(dev))
        return false;

    if (!dev->putChar('\n'))
        return false;

    return d->brushesPipe.saveToDevice(dev);
}

void KisImagePipeBrush::notifyStrokeStarted()
{
    d->brushesPipe.notifyStrokeStarted();
}

void KisImagePipeBrush::prepareForSeqNo(const KisPaintInformation &info, int seqNo)
{
    d->brushesPipe.prepareForSeqNo(info, seqNo);
}

void KisImagePipeBrush::generateMaskAndApplyMaskOrCreateDab(KisFixedPaintDeviceSP dst, KisBrush::ColoringInformation* coloringInformation,
        KisDabShape const& shape,
        const KisPaintInformation& info,
        double subPixelX , double subPixelY,
        qreal softnessFactor, qreal lightnessStrength) const
{
    d->brushesPipe.generateMaskAndApplyMaskOrCreateDab(dst, coloringInformation, shape, info, subPixelX, subPixelY, softnessFactor, lightnessStrength);
}

void KisImagePipeBrush::notifyBrushIsGoingToBeClonedForStroke()
{
    d->brushesPipe.notifyBrushIsGoingToBeClonedForStroke();
}

QVector<KisGbrBrushSP> KisImagePipeBrush::brushes() const
{
    return d->brushesPipe.brushes();
}

KisFixedPaintDeviceSP KisImagePipeBrush::paintDevice(
    const KoColorSpace * colorSpace,
    KisDabShape const& shape,
    const KisPaintInformation& info,
    double subPixelX, double subPixelY) const
{
    return d->brushesPipe.paintDevice(colorSpace, shape, info, subPixelX, subPixelY);
}

QString KisImagePipeBrush::parasiteSelection()
{
    return parasiteSelectionString;
}

void KisImagePipeBrush::makeMaskImage(bool preserveAlpha)
{
    KisGbrBrush::makeMaskImage(preserveAlpha);
    d->brushesPipe.makeMaskImage(preserveAlpha);
    setBrushType(PIPE_MASK);
}

void KisImagePipeBrush::setAdjustmentMidPoint(quint8 value)
{
    KisGbrBrush::setAdjustmentMidPoint(value);
    d->brushesPipe.setAdjustmentMidPoint(value);
}

void KisImagePipeBrush::setBrightnessAdjustment(qreal value)
{
    KisGbrBrush::setBrightnessAdjustment(value);
    d->brushesPipe.setBrightnessAdjustment(value);
}

void KisImagePipeBrush::setContrastAdjustment(qreal value)
{
    KisGbrBrush::setContrastAdjustment(value);
    d->brushesPipe.setContrastAdjustment(value);
}

void KisImagePipeBrush::setAutoAdjustMidPoint(bool value)
{
    KisGbrBrush::setAutoAdjustMidPoint(value);
    d->brushesPipe.setAutoAdjustMidPoint(value);
}

KisOptimizedBrushOutline KisImagePipeBrush::outline(bool forcePreciseOutline) const
{
    Q_UNUSED(forcePreciseOutline);

    KisGbrBrushSP brush = d->brushesPipe.firstBrush();
    Q_ASSERT(brush);

    return brush->outline();
}

bool KisImagePipeBrush::canPaintFor(const KisPaintInformation& info)
{
    return (!d->brushesPipe.parasite().needsMovement || info.drawingDistance() >= 0.5);
}

QString KisImagePipeBrush::defaultFileExtension() const
{
    return QString(".gih");
}

quint32 KisImagePipeBrush::brushIndex() const
{
    return d->brushesPipe.currentBrushIndex();
}

qint32 KisImagePipeBrush::maskWidth(KisDabShape const& shape, double subPixelX, double subPixelY, const KisPaintInformation& info) const
{
    return d->brushesPipe.maskWidth(shape, subPixelX, subPixelY, info);
}

qint32 KisImagePipeBrush::maskHeight(KisDabShape const& shape, double subPixelX, double subPixelY, const KisPaintInformation& info) const
{
    return d->brushesPipe.maskHeight(shape, subPixelX, subPixelY, info);
}

void KisImagePipeBrush::setAngle(qreal _angle)
{
    KisGbrBrush::setAngle(_angle);
    d->brushesPipe.setAngle(_angle);
}

void KisImagePipeBrush::setScale(qreal _scale)
{
    KisGbrBrush::setScale(_scale);
    d->brushesPipe.setScale(_scale);
}

void KisImagePipeBrush::setSpacing(double _spacing)
{
    KisGbrBrush::setSpacing(_spacing);
    d->brushesPipe.setSpacing(_spacing);
}

void KisImagePipeBrush::setBrushApplication(enumBrushApplication brushApplication)
{
    KisGbrBrush::setBrushApplication(brushApplication);
    d->brushesPipe.setBrushApplication(brushApplication);
}

void KisImagePipeBrush::setGradient(KoAbstractGradientSP gradient) {
    KisGbrBrush::setGradient(gradient);
    d->brushesPipe.setGradient(gradient);
}

KisGbrBrushSP KisImagePipeBrush::testingGetCurrentBrush(const KisPaintInformation& info) const
{
    return d->brushesPipe.currentBrush(info);
}

void KisImagePipeBrush::testingSelectNextBrush(const KisPaintInformation& info) const
{
    return d->brushesPipe.testingSelectNextBrush(info);
}

const KisPipeBrushParasite& KisImagePipeBrush::parasite() const {
    return d->brushesPipe.parasite();
}

void KisImagePipeBrush::setParasite(const KisPipeBrushParasite &parasite)
{
    d->brushesPipe.setParasite(parasite);
}

void KisImagePipeBrush::setDevices(QVector<QVector<KisPaintDevice *> > devices, int w, int h)
{

    for (int i = 0; i < devices.at(0).count(); i++) {
        d->brushesPipe.addBrush(KisGbrBrushSP(new KisGbrBrush(devices.at(0).at(i), 0, 0, w, h)));
    }
}

void KisImagePipeBrush::coldInitBrush()
{
    d->brushesPipe.coldInitBrush();
}
