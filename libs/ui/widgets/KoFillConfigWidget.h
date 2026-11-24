/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2012 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FILLCONFIGWIDGET_H
#define FILLCONFIGWIDGET_H

#include "kritaui_export.h"

#include <QWidget>
#include <QSharedPointer>
#include <KoFlake.h>
#include <KoFlakeTypes.h>
#include <SvgMeshGradient.h>

class KoShapeFillWrapper;
class KoCanvasBase;
class KoShapeBackground;
class KoShape;
class KoColor;

class KRITAUI_EXPORT KoFillConfigWidget : public QWidget
{
    Q_OBJECT
    enum StyleButton {
        None = 0,
        Solid,
        Gradient,
        Pattern,
        MeshGradient
    };

public:

    explicit KoFillConfigWidget(KoCanvasBase *canvas, KoFlake::FillVariant fillVariant, bool trackShapeSelection, QWidget *parent);
    ~KoFillConfigWidget() override;

    void setNoSelectionTrackingMode(bool value);

    QList<KoShape*> currentShapes();

    int selectedFillIndex();

    KoShapeStrokeSP createShapeStroke();

    void activate();
    void deactivate();

    void forceUpdateOnSelectionChanged();

    void setSelectedMeshGradientHandle(const SvgMeshPosition &position);

private Q_SLOTS:
    void styleButtonPressed(int buttonId);

    void noColorSelected();
    void shapeChanged();

    void colorChanged(std::pair<QColor, KoFlake::FillVariant> resource);

    void patternChanged(QSharedPointer<KoShapeBackground> background);

    void slotUpdateFillTitle();

    void slotCanvasResourceChanged(int key, const QVariant &value);

    void slotSavePredefinedGradientClicked();

    void activeGradientChanged();
    void gradientResourceChanged();

    void slotGradientTypeChanged();
    void slotGradientRepeatChanged();

    void slotProposeCurrentColorToResourceManager();
    void slotRecoverColorInResourceManager();

    void slotMeshGradientChanged();
    void slotMeshGradientShadingChanged(int index);
    void slotMeshHandleColorChanged(const KoColor &c);

Q_SIGNALS:
    void sigFillChanged();

    void sigInternalRequestColorToResourceManager();
    void sigInternalRecoverColorInResourceManager();
    void sigMeshGradientResetted();

private:
    void updateGradientUi(const QGradient *gradient);
    void setNewGradientBackgroundToShape();
    void updateGradientSaveButtonAvailability();
    void loadCurrentFillFromResourceServer();

    void createNewMeshGradientBackground();
    void createNewDefaultMeshGradientBackground();
    void setNewMeshGradientBackgroundToShape();
    void updateMeshGradientUI();

    void updateWidgetComponentVisibility();

    void updateUiFromFillType(KoShape *shape);

    class Private;
    Private * const d {nullptr};
};

#endif
