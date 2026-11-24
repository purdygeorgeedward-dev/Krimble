/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISTEXTPROPERTYMANAGER_H
#define KISTEXTPROPERTYMANAGER_H

#include <QObject>
#include "kritaui_export.h"

class KisView;
class KisCanvasResourceProvider;
class KoSvgTextPropertiesInterface;

class KRITAUI_EXPORT KisTextPropertiesManager : public QObject
{
    Q_OBJECT
public:
    KisTextPropertiesManager(QObject *parent = nullptr);
    ~KisTextPropertiesManager();

    void setCanvasResourceProvider(KisCanvasResourceProvider *provider);
    void setTextPropertiesInterface(KoSvgTextPropertiesInterface *interface);

Q_SIGNALS:
    void sigInterfaceChanged(KoSvgTextPropertiesInterface *interface);

private Q_SLOTS:
    void slotInterfaceSelectionChanged();
    void slotCharacterInterfaceSelectionChanged();
    void slotTextPropertiesChanged();
    void slotCharacterPropertiesChanged();
private:

    struct Private;
    QScopedPointer<Private> d;

};

#endif
