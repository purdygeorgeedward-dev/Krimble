/*
 *  SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_PREFERENCE_SET_REGISTRY_H_
#define KIS_PREFERENCE_SET_REGISTRY_H_

#include <QObject>
#include <QWidget>
#include <QString>

#include "KoGenericRegistry.h"

#include "kis_types.h"

#include <kritaui_export.h>

class QIcon;

class KRITAUI_EXPORT KisPreferenceSet : public QWidget {
Q_OBJECT
public:
    KisPreferenceSet(QWidget* parent=0) : QWidget(parent)
    {}
    virtual QString id() = 0;
    virtual QString name() = 0;
    virtual QString header() = 0;
    virtual QIcon icon() = 0;
public Q_SLOTS:
    virtual void savePreferences() const = 0;
    virtual void loadPreferences() = 0;
    virtual void loadDefaultPreferences() = 0;
};

class KRITAUI_EXPORT KisAbstractPreferenceSetFactory {
public:
    virtual ~KisAbstractPreferenceSetFactory() {}
    virtual KisPreferenceSet* createPreferenceSet() = 0;
    virtual QString id() const = 0;
};

class KRITAUI_EXPORT KisPreferenceSetRegistry : public QObject, public KoGenericRegistry<KisAbstractPreferenceSetFactory*>
{
public:
    KisPreferenceSetRegistry();
    ~KisPreferenceSetRegistry() override;
    static KisPreferenceSetRegistry * instance();

private:
    Q_DISABLE_COPY(KisPreferenceSetRegistry)
};

#endif
