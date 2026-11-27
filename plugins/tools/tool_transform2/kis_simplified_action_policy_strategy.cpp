/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_simplified_action_policy_strategy.h"

#include "KoPointerEvent.h"
#include <KoSnapGuide.h>
#include "kis_coordinates_converter.h"

struct KisSimplifiedActionPolicyStrategy::Private
{
    Private(const KisCoordinatesConverter *_converter, KoSnapGuide *_snapGuide)
        : converter(_converter),
          snapGuide(_snapGuide),
          sampleFromNodeModifierActive(false),
          changeSizeModifierActive(false),
          anySamplerModifierActive(false) {}

    const KisCoordinatesConverter *converter;
    KoSnapGuide *snapGuide;

    bool sampleFromNodeModifierActive;
    bool changeSizeModifierActive;
    bool anySamplerModifierActive;
    Qt::KeyboardModifiers modifiersAtStart;
    QPointF dragOffset;
    QPointF lastImagePos;
};

KisSimplifiedActionPolicyStrategy::KisSimplifiedActionPolicyStrategy(const KisCoordinatesConverter *_converter, KoSnapGuide *_snapGuide)
    : m_d(new Private(_converter, _snapGuide))
{
}

KisSimplifiedActionPolicyStrategy::~KisSimplifiedActionPolicyStrategy()
{
}

QPointF KisSimplifiedActionPolicyStrategy::handleSnapPoint(const QPointF &imagePos)
{
    return imagePos;
}

QPointF KisSimplifiedActionPolicyStrategy::snapDocPoint(const QPointF &point, Qt::KeyboardModifiers modifiers) const
{
    QPointF pos = point;

    if (m_d->snapGuide) {
        Qt::KeyboardModifiers modifiersForSnapping = modifiers;

        if (shiftModifierIsUsed()) {
            modifiersForSnapping.setFlag(Qt::ShiftModifier, false);
        }

        pos = m_d->snapGuide->snap(point, m_d->dragOffset, modifiersForSnapping);
    }

    return pos;
}

bool KisSimplifiedActionPolicyStrategy::beginPrimaryAction(KoPointerEvent *event)
{
    const QPointF rawImagePoint = m_d->converter->documentToImage(event->point);
    const QPointF snappedImagePoint = handleSnapPoint(rawImagePoint);

    if (m_d->snapGuide && rawImagePoint != snappedImagePoint) {
        QPointF imageOffset = snappedImagePoint - rawImagePoint;
        m_d->dragOffset = m_d->converter->imageToDocument(imageOffset);
    }

    const QPointF pos = snapDocPoint(event->point, event->modifiers());

    QPointF imagePos = m_d->converter->documentToImage(pos);
    m_d->lastImagePos = imagePos;

    m_d->modifiersAtStart = event->modifiers();

    return beginPrimaryAction(imagePos);
}

void KisSimplifiedActionPolicyStrategy::continuePrimaryAction(KoPointerEvent *event)
{
    const bool shiftIsActive = event->modifiers() & Qt::ShiftModifier;
    const bool altIsActive = event->modifiers() & Qt::AltModifier;

    const QPointF pos = snapDocPoint(event->point, event->modifiers());
    QPointF imagePos = m_d->converter->documentToImage(pos);
    m_d->lastImagePos = imagePos;

    continuePrimaryAction(imagePos, shiftIsActive, altIsActive, m_d->modifiersAtStart == event->modifiers());
}

void KisSimplifiedActionPolicyStrategy::continuePrimaryAction(const QPointF &imagePos, bool shiftModifierActive, bool altModifierActive, bool hasOriginalModifiersSet)
{
    Q_UNUSED(hasOriginalModifiersSet);
    continuePrimaryAction(imagePos, shiftModifierActive, altModifierActive);
}

void KisSimplifiedActionPolicyStrategy::hoverActionCommon(KoPointerEvent *event)
{
    QPointF imagePos = m_d->converter->documentToImage(event->point);
    m_d->lastImagePos = imagePos;

    hoverActionCommon(imagePos);
}

bool KisSimplifiedActionPolicyStrategy::endPrimaryAction(KoPointerEvent *event)
{
    const QPointF pos = snapDocPoint(event->point, event->modifiers());
    QPointF imagePos = m_d->converter->documentToImage(pos);
    m_d->lastImagePos = imagePos;

    return endPrimaryAction();
}

void KisSimplifiedActionPolicyStrategy::activatePrimaryAction()
{
    setTransformFunction(m_d->lastImagePos, m_d->anySamplerModifierActive && !m_d->sampleFromNodeModifierActive, m_d->changeSizeModifierActive, m_d->sampleFromNodeModifierActive);
}

void KisSimplifiedActionPolicyStrategy::activateAlternateAction(KisTool::AlternateAction action)
{
    if (action == KisTool::ChangeSize) {
        m_d->changeSizeModifierActive = true;
    } else if (action == KisTool::SampleFgNode || action == KisTool::SampleBgNode) {
        m_d->anySamplerModifierActive = true;
        m_d->sampleFromNodeModifierActive = true;
    } else if (action == KisTool::SampleFgImage || action == KisTool::SampleBgImage) {
        m_d->anySamplerModifierActive = true;
        m_d->sampleFromNodeModifierActive = false;
    }

    setTransformFunction(m_d->lastImagePos, m_d->anySamplerModifierActive && !m_d->sampleFromNodeModifierActive, m_d->changeSizeModifierActive, m_d->sampleFromNodeModifierActive);
}

void KisSimplifiedActionPolicyStrategy::deactivateAlternateAction(KisTool::AlternateAction action)
{
    if (action == KisTool::ChangeSize) {
        m_d->changeSizeModifierActive = false;
    } else if (action == KisTool::SampleFgNode || action == KisTool::SampleBgNode ||
               action == KisTool::SampleFgImage || action == KisTool::SampleBgImage) {

        m_d->anySamplerModifierActive = false;
        m_d->sampleFromNodeModifierActive = false;
    }
}

bool KisSimplifiedActionPolicyStrategy::beginAlternateAction(KoPointerEvent *event, KisTool::AlternateAction action)
{
    Q_UNUSED(action);

    if (!m_d->changeSizeModifierActive && !m_d->anySamplerModifierActive) return false;

    const QPointF pos = snapDocPoint(event->point, event->modifiers());
    QPointF imagePos = m_d->converter->documentToImage(pos);
    m_d->lastImagePos = imagePos;

    m_d->modifiersAtStart = event->modifiers();

    return beginPrimaryAction(imagePos);
}

void KisSimplifiedActionPolicyStrategy::continueAlternateAction(KoPointerEvent *event, KisTool::AlternateAction action)
{
    Q_UNUSED(action);

    if (!m_d->changeSizeModifierActive && !m_d->anySamplerModifierActive) return;
    const bool altIsActive = event->modifiers() & Qt::AltModifier;

    const QPointF pos = snapDocPoint(event->point, event->modifiers());
    QPointF imagePos = m_d->converter->documentToImage(pos);
    m_d->lastImagePos = imagePos;

    continuePrimaryAction(imagePos, m_d->changeSizeModifierActive, altIsActive, m_d->modifiersAtStart == event->modifiers());
}

bool KisSimplifiedActionPolicyStrategy::endAlternateAction(KoPointerEvent *event, KisTool::AlternateAction action)
{
    Q_UNUSED(action);

    if (!m_d->changeSizeModifierActive && !m_d->anySamplerModifierActive) return false;

    const QPointF pos = snapDocPoint(event->point, event->modifiers());
    QPointF imagePos = m_d->converter->documentToImage(pos);
    m_d->lastImagePos = imagePos;

    return endPrimaryAction();
}

bool KisSimplifiedActionPolicyStrategy::shiftModifierIsUsed() const
{
    return false;
}

void KisSimplifiedActionPolicyStrategy::hoverActionCommon(const QPointF &pt)
{
    setTransformFunction(pt, m_d->anySamplerModifierActive && !m_d->sampleFromNodeModifierActive, m_d->changeSizeModifierActive, m_d->sampleFromNodeModifierActive);
}

