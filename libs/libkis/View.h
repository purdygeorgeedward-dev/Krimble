/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_VIEW_H
#define LIBKIS_VIEW_H

#include <QObject>

#include "kritalibkis_export.h"
#include "libkis.h"

class ManagedColor;
class Resource;
class Scratchpad;
class Node;
class KisView;

class KRITALIBKIS_EXPORT View : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(View)

public:
    explicit View(KisView *view, QObject *parent = 0);
    ~View() override;

    bool operator==(const View &other) const;
    bool operator!=(const View &other) const;

public Q_SLOTS:

    Window* window() const;

    Document* document() const;

    void setDocument(Document *document);

    bool visible() const;

    void setVisible();

    Canvas* canvas() const;

    void activateResource(Resource *resource);

    ManagedColor *foregroundColor() const;
    void setForeGroundColor(ManagedColor *color);

    ManagedColor *backgroundColor() const;
    void setBackGroundColor(ManagedColor *color);

    Resource *currentBrushPreset() const;

    void setCurrentBrushPreset(Resource *resource);

    Resource *currentPattern() const;

    void setCurrentPattern(Resource *resource);

    Resource *currentGradient() const;

    void setCurrentGradient(Resource *resource);

    QString currentBlendingMode() const;

    void setCurrentBlendingMode(const QString &blendingMode);

    float HDRExposure() const;

    void setHDRExposure(float exposure);

    float HDRGamma() const;

    void setHDRGamma(float gamma);

    qreal paintingOpacity() const;

    void setPaintingOpacity(qreal opacity);

    qreal brushSize() const;

    void setBrushSize(qreal brushSize);

    qreal brushFade() const;

    void setBrushFade(qreal brushFade);

    qreal brushRotation() const;

    void setBrushRotation(qreal brushRotation);

    qreal paintingFlow() const;

    void setPaintingFlow(qreal flow);

    qreal patternSize() const;

    void setPatternSize(qreal size);

    bool eraserMode() const;

    void setEraserMode(bool value);

    bool globalAlphaLock() const;

    void setGlobalAlphaLock(bool value);

    bool disablePressure() const;

    void setDisablePressure(bool value);

    void showFloatingMessage(const QString &message, const QIcon& icon, int timeout, int priority);

    QList<Node *> selectedNodes() const;

    QTransform flakeToDocumentTransform() const;

    QTransform flakeToCanvasTransform() const;

    QTransform flakeToImageTransform() const;

Q_SIGNALS:
    void currentToolChanged(const QString &toolId);

    void currentBrushPresetChanged();

    void foregroundColorChanged();

    void backgroundColorChanged();
private:

    friend class Window;
    friend class Scratchpad;

    KisView *view();

    struct Private;
    Private *const d;

};

#endif
