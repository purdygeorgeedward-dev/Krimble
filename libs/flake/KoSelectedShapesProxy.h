/*
 *  SPDX-FileCopyrightText: 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KOSELECTEDSHAPESPROXY_H
#define KOSELECTEDSHAPESPROXY_H

#include <QObject>
#include "kritaflake_export.h"

class KoSelection;
class KoShapeLayer;

class KRITAFLAKE_EXPORT KoSelectedShapesProxy : public QObject
{
    Q_OBJECT
public:
    explicit KoSelectedShapesProxy(QObject *parent = 0);

    virtual KoSelection *selection() = 0;

    bool isRequestingToBeEdited();
    void setRequestingToBeEdited(bool value);

Q_SIGNALS:

    void selectionChanged();

    void selectionContentChanged();

    void currentLayerChanged(const KoShapeLayer *layer);

private:
    bool m_isRequestingEditing = false;
};

#endif
