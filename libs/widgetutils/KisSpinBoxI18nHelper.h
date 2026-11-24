/*
 * SPDX-FileCopyrightText: 2021-2023 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_SPIN_BOX_I18N_HELPER_H
#define KIS_SPIN_BOX_I18N_HELPER_H

#include <functional>

#include <QStringView>

#include "kritawidgetutils_export.h"

class QDoubleSpinBox;
class QSpinBox;
class QString;

class KisSelectionPropertySliderBase;

namespace KisSpinBoxI18nHelper
{
    KRITAWIDGETUTILS_EXPORT void install(QSpinBox *spinBox, std::function<QString(int)> messageFn);

    KRITAWIDGETUTILS_EXPORT bool update(QSpinBox *spinBox);

    KRITAWIDGETUTILS_EXPORT void setText(QSpinBox *spinBox, QStringView textTemplate);

    KRITAWIDGETUTILS_EXPORT void setText(QDoubleSpinBox *spinBox, QStringView textTemplate);

    void setText(KisSelectionPropertySliderBase *spinBox, const QStringView textTemplate) = delete;

}

#endif
