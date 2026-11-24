/*
 *  SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KIS_MIMEDATA_H
#define KIS_MIMEDATA_H

#include <QMimeData>
#include <QRect>

#include <kis_types.h>
#include <kritaui_export.h>

class KisShapeController;
class KisNodeDummy;
class KisNodeInsertionAdapter;
class KisNodeGraphListener;
class KisProcessingApplicator;
class KisDisplayConfig;

class KRITAUI_EXPORT KisMimeData : public QMimeData
{
    Q_OBJECT
public:
    KisMimeData(QList<KisNodeSP> nodes, KisImageSP image, bool forceCopy = false);

    QList<KisNodeSP> nodes() const;

    void deepCopyNodes();

    QStringList formats() const override;

    static bool isNodeMimeDataFromSameImage(const QMimeData *data, KisImageSP image);

    static KisNodeList loadNodesFast(
        const QMimeData *data,
        KisImageSP image,
        KisShapeController *shapeController,
        bool &copyNode);

    static KisNodeList loadNodesFastAndRecenter(const QPoint &preferredCenter,
            const QMimeData *data,
            KisImageSP image,
            KisShapeController *shapeController,
            bool &copyNode);

    static KisDisplayConfig displayConfigForMimePastes();

private:
    static KisNodeList loadNonNativeNodes(const QMimeData *data,
                                          KisImageWSP image);

    static KisNodeList tryLoadInternalNodes(const QMimeData *data,
                                                 KisImageSP image,
                                                 KisShapeController *shapeController,
                                                 bool &copyNode);

public:
    static QMimeData* mimeForLayers(const KisNodeList &nodes, KisImageSP image, bool forceCopy = false);
    static QMimeData* mimeForLayersDeepCopy(const KisNodeList &nodes, KisImageSP image, bool forceCopy);
    static bool insertMimeLayers(const QMimeData *data,
                                 KisImageSP image,
                                 KisShapeController *shapeController,
                                 KisNodeDummy *parentDummy,
                                 KisNodeDummy *aboveThisDummy,
                                 bool copyNode,
                                 KisNodeInsertionAdapter *nodeInsertionAdapter,
                                 bool changeOffset = false,
                                 QPointF offset = QPointF(),
                                 KisProcessingApplicator *applicator = nullptr);

protected:
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QVariant retrieveData(const QString &mimetype, QVariant::Type preferredType) const override;
#else
    QVariant retrieveData(const QString &mimetype, QMetaType preferredType) const override;
#endif

private:
    static void initializeExternalNode(KisNodeSP *nodes,
                                       KisImageSP srcImage, KisImageSP dstImage,
                                       KisShapeController *shapeController);

private:

    QList<KisNodeSP> m_nodes;
    bool m_forceCopy;
    KisImageSP m_image;
    QRect m_copiedBounds;
};

#endif
