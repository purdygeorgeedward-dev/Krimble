/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_NODE_H
#define LIBKIS_NODE_H

#include <QObject>

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

#include "PaintingResources.h"

class KRITALIBKIS_EXPORT Node : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Node)

public:
    static Node *createNode(KisImageSP image, KisNodeSP node, QObject *parent = 0);
    ~Node() override;
    bool operator==(const Node &other) const;
    bool operator!=(const Node &other) const;

public Q_SLOTS:

    Node *clone() const;

    bool alphaLocked() const;

    void setAlphaLocked(bool value);

    QString blendingMode() const;

    void setBlendingMode(QString value);

    QList<Channel*> channels() const;

    QList<Node*> childNodes() const;

    QList<Node*> findChildNodes(const QString &name = QString(), bool recursive = false, bool partialMatch = false, const QString &type = QString(), int colorLabelIndex = 0) const;

    bool addChildNode(Node *child, Node *above);

    bool removeChildNode(Node *child);

    void setChildNodes(QList<Node*> nodes);

    QString colorDepth() const;

    QString colorModel() const;

    QString colorProfile() const;

    bool setColorProfile(const QString &colorProfile);

    bool setColorSpace(const QString &colorModel, const QString &colorDepth, const QString &colorProfile);

    bool animated() const;

    void enableAnimation() const;

    void setPinnedToTimeline(bool pinned) const;

    bool isPinnedToTimeline() const;

    void setCollapsed(bool collapsed);

    bool collapsed() const;

    int colorLabel() const;

    void setColorLabel(int index);

    bool inheritAlpha() const;

    void setInheritAlpha(bool value);

    bool locked() const;

    void setLocked(bool value);

    bool hasExtents();

    QString name() const;

    void setName(QString name);

    int opacity() const;

    void setOpacity(int value);

    Node* parentNode() const;

    virtual QString type() const;

    QIcon icon() const;

    bool visible() const;

    bool hasKeyframeAtTime(int frameNumber);

    void setVisible(bool visible);

    QByteArray pixelData(int x, int y, int w, int h) const;

    QByteArray pixelDataAtTime(int x, int y, int w, int h, int time) const;

    QByteArray projectionPixelData(int x, int y, int w, int h) const;

    bool setPixelData(QByteArray value, int x, int y, int w, int h);

    QRect bounds() const;

    void move(int x, int y);

    QPoint position() const;

    bool remove();

    Node* duplicate();

    bool save(const QString &filename, double xRes, double yRes, const InfoObject &exportConfiguration, const QRect &exportRect = QRect());

    Node *mergeDown();

    void scaleNode(QPointF origin, int width, int height, QString strategy);

    void rotateNode(double radians);

    void cropNode(int x, int y, int w, int h);

    void shearNode(double angleX, double angleY);

    QImage thumbnail(int w, int h);

    QString layerStyleToAsl();

    bool setLayerStyleFromAsl(const QString &asl);

    int index() const;

    QUuid uniqueId() const;

    void paintLine(const QPointF pointOne,
                   const QPointF pointTwo,
                   double pressureOne = 1.0,
                   double pressureTwo = 1.0,
                   const QString strokeStyle = PaintingResources::defaultStrokeStyle);

    void paintRectangle(const QRectF &rect,
                        const QString strokeStyle = PaintingResources::defaultStrokeStyle,
                        const QString fillStyle = PaintingResources::defaultFillStyle);

    void paintPolygon(const QList<QPointF> points,
                      const QString strokeStyle = PaintingResources::defaultStrokeStyle,
                      const QString fillStyle = PaintingResources::defaultFillStyle);
    void paintEllipse(const QRectF &rect,
                      const QString strokeStyle = PaintingResources::defaultStrokeStyle,
                      const QString fillStyle = PaintingResources::defaultFillStyle);
    void paintPath(const QPainterPath &path,
                   const QString strokeStyle = PaintingResources::defaultStrokeStyle,
                   const QString fillStyle = PaintingResources::defaultFillStyle);
    QString paintAbility();

private:

    friend class Filter;
    friend class Document;
    friend class Selection;
    friend class GroupLayer;
    friend class FileLayer;
    friend class FilterLayer;
    friend class FillLayer;
    friend class VectorLayer;
    friend class FilterMask;
    friend class SelectionMask;
    friend class TransparencyMask;
    friend class TransformMask;
    friend class ColorizeMask;
    friend class CloneLayer;

    explicit Node(KisImageSP image, KisNodeSP node, QObject *parent = 0);

    KisPaintDeviceSP paintDevice() const;
    KisImageSP image() const;
    KisNodeSP node() const;

    struct Private;
    Private *const d;

};

typedef QSharedPointer<Node> NodeSP;

#endif
