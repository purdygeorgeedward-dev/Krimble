/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_SHAPE_CONTROLLER
#define KIS_SHAPE_CONTROLLER

#include <QMap>

#include "kis_dummies_facade_base.h"
#include <KoShapeControllerBase.h>

class KisNodeDummy;
class KoShapeLayer;

class KisCanvas2;
class KisDocument;
class KisNameServer;
class KUndo2Stack;

class KRITAUI_EXPORT KisShapeController : public KisDummiesFacadeBase, public KoShapeControllerBase
{

    Q_OBJECT

public:

    KisShapeController(KisNameServer *nameServer, KUndo2Stack *undoStack, QObject *parent = 0);
    ~KisShapeController() override;

    bool hasDummyForNode(KisNodeSP node) const override;
    KisNodeDummy* dummyForNode(KisNodeSP layer) const override;

    KisNodeDummy* rootDummy() const override;
    int dummiesCount() const override;

    KoShapeLayer* shapeForNode(KisNodeSP layer) const;
    void setInitialShapeForCanvas(KisCanvas2 *canvas);

    using KisDummiesFacadeBase::setImage;
    void setImage(KisImageWSP image, KisNodeSP activeNode) override;

private:
    void addNodeImpl(KisNodeSP node, KisNodeSP parent, KisNodeSP aboveThis) override;
    void removeNodeImpl(KisNodeSP node) override;

private Q_SLOTS:
    void slotUpdateDocumentResolution();
    void slotUpdateDocumentSize();

Q_SIGNALS:
    void selectionChanged();
    void selectionContentChanged();
    void currentLayerChanged(const KoShapeLayer*);

public:
    KoShapeContainer *createParentForShapes(const QList<KoShape *> shapes, bool forceNewLayer, KUndo2Command *parentCommand) override;

    QRectF documentRectInPixels() const override;
    qreal pixelsPerInch() const override;

private:
    struct Private;
    Private * const m_d;
};

#endif
