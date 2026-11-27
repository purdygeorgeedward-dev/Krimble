/*
 *  SPDX-FileCopyrightText: 2012 Sven Langkamp <sven.langkamp@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_LAYERCOMPOSITION_H
#define _KIS_LAYERCOMPOSITION_H

#include "kritaimage_export.h"

#include <QMap>
#include <QUuid>
#include <QDomDocument>
#include <QDomElement>

#include "kis_image.h"

class KRITAIMAGE_EXPORT KisLayerComposition
{
public:
    KisLayerComposition(KisImageWSP image, const QString& name);
    ~KisLayerComposition();

    KisLayerComposition(const KisLayerComposition &rhs, KisImageWSP otherImage = 0);

    void setName(const QString& name);

    QString name();

    void store();

    void apply();

    void setExportEnabled(bool enabled);

    bool isExportEnabled();

    void setVisible(QUuid id, bool visible);

    void setCollapsed(QUuid id, bool collapsed);

    void save(QDomDocument& doc, QDomElement& element);

private:
    KisImageWSP m_image;
    QString m_name;
    QMap<QUuid, bool> m_visibilityMap;
    QMap<QUuid, bool> m_collapsedMap;
    bool m_exportEnabled;

    friend class KisCompositionVisitor;
};

#endif
