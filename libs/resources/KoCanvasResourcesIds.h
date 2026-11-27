/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOCANVASRESOURCESIDS_H
#define KOCANVASRESOURCESIDS_H

namespace KoCanvasResource
{
enum CanvasResourceId {
    ForegroundColor = 0,
    BackgroundColor,
    PageSize,
    Unit,
    CurrentPage,
    ActiveStyleType,
    ActiveRange,
    ShowTextShapeOutlines,
    ShowFormattingCharacters,
    ShowTableBorders,
    ShowSectionBounds,
    ShowInlineObjectVisualization,
    ApplicationSpeciality,
    KritaStart = 6000,
    HdrExposure = KritaStart + 1,
    CurrentPattern,
    CurrentGamutMask,
    GamutMaskActive,
    CurrentGradient,
    CurrentDisplayProfile,
    CurrentKritaNode,
    CurrentPaintOpPreset,
    CurrentPaintOpPresetCache,
    CurrentPaintOpPresetName,
    CurrentGeneratorConfiguration,
    CurrentCompositeOp,
    CurrentEffectiveCompositeOp,
    LodAvailability,
    LodSizeThreshold,
    LodSizeThresholdSupported,
    EffectiveLodAvailability,
    EraserMode,
    MirrorHorizontal,
    MirrorVertical,
    MirrorHorizontalLock,
    MirrorVerticalLock,
    MirrorVerticalHideDecorations,
    MirrorHorizontalHideDecorations,
    Opacity,
    Flow,
    Size,
    Fade,
    Scatter,
    PatternSize,
    HdrGamma,
    GlobalAlphaLock,
    DisablePressure,
    PreviousPaintOpPreset,
    EffectiveZoom,
    EffectivePhysicalZoom,
    BrushRotation,
    HandleRadius,
    DecorationThickness,
    UsingOtherColor,
    ColorHistoryModel,
    SvgTextPropertyData,
    SvgCharacterTextPropertyData,
};

}

#endif
