/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SELECTIONDECORATOR_H
#define SELECTIONDECORATOR_H

#include <KoViewConverter.h>
#include <KoFlake.h>

#include "KoShapeMeshGradientHandles.h"

#include <QPainter>

class KoSelection;
class KoCanvasResourceProvider;
class KoColorDisplayRendererInterface;

static const struct DecoratorIconPositions {
    QPoint uiOffset = QPoint(0, 40);
} decoratorIconPositions;

class SelectionDecorator
{
public:
    SelectionDecorator(KoCanvasResourceProvider *resourceManager, KoColorDisplayRendererInterface *displayInterface);
    ~SelectionDecorator() {}

    void paint(QPainter &painter, const KoViewConverter &converter);

    void setSelection(KoSelection *selection);

    void setHandleRadius(int radius);

    void setDecorationThickness(int thickness);

    void setShowFillGradientHandles(bool value);

    void setShowStrokeFillGradientHandles(bool value);

    void setShowFillMeshGradientHandles(bool value);
    void setCurrentMeshGradientHandles(const KoShapeMeshGradientHandles::Handle &selectedHandle,
                                       const KoShapeMeshGradientHandles::Handle &hoveredHandle);

    void setForceShapeOutlines(bool value);

private:
    void paintGradientHandles(KoShape *shape, KoFlake::FillVariant fillVariant, QPainter &painter, const KoViewConverter &converter);

    void paintMeshGradientHandles(KoShape *shape, KoFlake::FillVariant fillVariant, QPainter &painter, const KoViewConverter &converter);

private:
    KoFlake::AnchorPosition m_hotPosition;
    KoSelection *m_selection {nullptr};
    KoColorDisplayRendererInterface *m_rendererInterface {nullptr};
    KoShapeMeshGradientHandles::Handle m_currentHoveredMeshHandle;
    KoShapeMeshGradientHandles::Handle m_selectedMeshHandle;
    int m_handleRadius {7};
    int m_decorationThickness {1};
    bool m_showFillGradientHandles {false};
    bool m_showStrokeFillGradientHandles {false};
    bool m_showFillMeshGradientHandles {false};
    bool m_forceShapeOutlines {false};
};

#endif
