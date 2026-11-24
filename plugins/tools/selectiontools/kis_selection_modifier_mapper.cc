/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2016 Michael Abrahams <miabraha@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "kis_selection.h"
#include "kis_selection_modifier_mapper.h"
#include "kis_config_notifier.h"
#include "kis_config.h"

Q_GLOBAL_STATIC(KisSelectionModifierMapper, s_instance)

#if 0
#include <bitset>
QString QMOD_BINARY(Qt::KeyboardModifiers m)
{
    return QString(std::bitset<sizeof(int) * 8>(m).to_string().c_str());
};
#endif

struct Q_DECL_HIDDEN KisSelectionModifierMapper::Private
{
    SelectionAction map(Qt::KeyboardModifiers m);
    void slotConfigChanged();
    Qt::KeyboardModifiers replaceModifiers;
    Qt::KeyboardModifiers intersectModifiers;
    Qt::KeyboardModifiers addModifiers;
    Qt::KeyboardModifiers subtractModifiers;
    Qt::KeyboardModifiers symmetricdifferenceModifiers;
};

KisSelectionModifierMapper::KisSelectionModifierMapper()
    : m_d(new Private)
{
    connect(KisConfigNotifier::instance(),
            SIGNAL(configChanged()),
            SLOT(slotConfigChanged()));
    slotConfigChanged();
}

KisSelectionModifierMapper::~KisSelectionModifierMapper()
{
}

KisSelectionModifierMapper *KisSelectionModifierMapper::instance()
{
    return s_instance;
}

void KisSelectionModifierMapper::slotConfigChanged()
{
    m_d->slotConfigChanged();
}

void KisSelectionModifierMapper::Private::slotConfigChanged()
{
    KisConfig cfg(true);
    if (!cfg.switchSelectionCtrlAlt()) {
        replaceModifiers   = Qt::ControlModifier;
        intersectModifiers = (Qt::KeyboardModifiers)(Qt::AltModifier | Qt::ShiftModifier);
        subtractModifiers  = Qt::AltModifier;
        symmetricdifferenceModifiers = (Qt::KeyboardModifiers)(Qt::ControlModifier | Qt::AltModifier);
    } else {
        replaceModifiers   = Qt::AltModifier;
        intersectModifiers = (Qt::KeyboardModifiers)(Qt::ControlModifier | Qt::ShiftModifier);
        subtractModifiers  = Qt::ControlModifier;
        symmetricdifferenceModifiers = (Qt::KeyboardModifiers)(Qt::AltModifier | Qt::ControlModifier);
    }

    addModifiers = Qt::ShiftModifier;
}

SelectionAction KisSelectionModifierMapper::map(Qt::KeyboardModifiers m)
{
    return s_instance->m_d->map(m);
}

SelectionAction KisSelectionModifierMapper::Private::map(Qt::KeyboardModifiers m)
{
    SelectionAction newAction = SELECTION_DEFAULT;
    if (m == replaceModifiers) {
        newAction = SELECTION_REPLACE;
    } else if (m == intersectModifiers) {
        newAction = SELECTION_INTERSECT;
    } else if (m == addModifiers) {
        newAction = SELECTION_ADD;
    } else if (m == subtractModifiers) {
        newAction = SELECTION_SUBTRACT;
    } else if (m == symmetricdifferenceModifiers) {
        newAction = SELECTION_SYMMETRICDIFFERENCE;
    }

    return newAction;
}
