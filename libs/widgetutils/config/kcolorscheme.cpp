/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "kcolorscheme.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <kcolorutils.h>

#include <QColor>
#include <QBrush>
#include <QWidget>

class StateEffects
{
public:
    explicit StateEffects(QPalette::ColorGroup state, const KSharedConfigPtr &);
    ~StateEffects() {}

    QBrush brush(const QBrush &background) const;
    QBrush brush(const QBrush &foreground, const QBrush &background) const;

private:
    enum Effects {
        Intensity = 0,
        Color = 1,
        Contrast = 2,
        IntensityNoEffect = 0,
        IntensityShade = 1,
        IntensityDarken = 2,
        IntensityLighten = 3,
        ColorNoEffect = 0,
        ColorDesaturate = 1,
        ColorFade = 2,
        ColorTint = 3,
        ContrastNoEffect = 0,
        ContrastFade = 1,
        ContrastTint = 2
    };

    int _effects[3];
    double _amount[3];
    QColor _color;
};

StateEffects::StateEffects(QPalette::ColorGroup state, const KSharedConfigPtr &config)
    : _color(0, 0, 0, 0)
{
    QString group;
    if (state == QPalette::Disabled) {
        group = QLatin1String("ColorEffects:Disabled");
    } else if (state == QPalette::Inactive) {
        group = QLatin1String("ColorEffects:Inactive");
    }

    _effects[0] = 0;
    _effects[1] = 0;
    _effects[2] = 0;

    if (! group.isEmpty()) {
        KConfigGroup cfg(config, group);
        const bool enabledByDefault = (state == QPalette::Disabled);
        if (cfg.readEntry("Enable", enabledByDefault)) {
            _effects[Intensity] = cfg.readEntry("IntensityEffect",
                                                (int)(state == QPalette::Disabled ?  IntensityDarken : IntensityNoEffect));
            _effects[Color]     = cfg.readEntry("ColorEffect",
                                                (int)(state == QPalette::Disabled ?  ColorNoEffect : ColorDesaturate));
            _effects[Contrast]  = cfg.readEntry("ContrastEffect",
                                                (int)(state == QPalette::Disabled ?  ContrastFade : ContrastTint));
            _amount[Intensity]  = cfg.readEntry("IntensityAmount", state == QPalette::Disabled ? 0.10 :  0.0);
            _amount[Color]      = cfg.readEntry("ColorAmount", state == QPalette::Disabled ?  0.0 : -0.9);
            _amount[Contrast]   = cfg.readEntry("ContrastAmount", state == QPalette::Disabled ? 0.65 :  0.25);
            if (_effects[Color] > ColorNoEffect) {
                _color = cfg.readEntry("Color", state == QPalette::Disabled ?  QColor(56, 56, 56) : QColor(112, 111, 110));
            }
        }
    }
}

QBrush StateEffects::brush(const QBrush &background) const
{
    QColor color = background.color();
    switch (_effects[Intensity]) {
    case IntensityShade:
        color = KColorUtils::shade(color, _amount[Intensity]);
        break;
    case IntensityDarken:
        color = KColorUtils::darken(color, _amount[Intensity]);
        break;
    case IntensityLighten:
        color = KColorUtils::lighten(color, _amount[Intensity]);
        break;
    }
    switch (_effects[Color]) {
    case ColorDesaturate:
        color = KColorUtils::darken(color, 0.0, 1.0 - _amount[Color]);
        break;
    case ColorFade:
        color = KColorUtils::mix(color, _color, _amount[Color]);
        break;
    case ColorTint:
        color = KColorUtils::tint(color, _color, _amount[Color]);
        break;
    }
    return QBrush(color);
}

QBrush StateEffects::brush(const QBrush &foreground, const QBrush &background) const
{
    QColor color = foreground.color();
    QColor bg = background.color();
    switch (_effects[Contrast]) {
    case ContrastFade:
        color = KColorUtils::mix(color, bg, _amount[Contrast]);
        break;
    case ContrastTint:
        color = KColorUtils::tint(color, bg, _amount[Contrast]);
        break;
    }
    return brush(color);
}

struct SetDefaultColors {
    int NormalBackground[3];
    int AlternateBackground[3];
    int NormalText[3];
    int InactiveText[3];
    int ActiveText[3];
    int LinkText[3];
    int VisitedText[3];
    int NegativeText[3];
    int NeutralText[3];
    int PositiveText[3];
};

struct DecoDefaultColors {
    int Hover[3];
    int Focus[3];
};

static const SetDefaultColors defaultViewColors = {
    { 255, 255, 255 },
    { 248, 247, 246 },
    {  31,  28,  27 },
    { 137, 136, 135 },
    { 146,  76, 157 },
    {   0,  87, 174 },
    { 100,  74, 155 },
    { 191,   3,   3 },
    { 176, 128,   0 },
    {   0, 110,  40 }
};

static const SetDefaultColors defaultWindowColors = {
    { 214, 210, 208 },
    { 218, 217, 216 },
    {  34,  31,  30 },
    { 137, 136, 135 },
    { 146,  76, 157 },
    {   0,  87, 174 },
    { 100,  74, 155 },
    { 191,   3,   3 },
    { 176, 128,   0 },
    { 0,   110,  40 }
};

static const SetDefaultColors defaultButtonColors = {
    { 223, 220, 217 },
    { 224, 223, 222 },
    {  34,  31,  30 },
    { 137, 136, 135 },
    { 146,  76, 157 },
    {   0,  87, 174 },
    { 100,  74, 155 },
    { 191,   3,   3 },
    { 176, 128,   0 },
    {   0, 110,  40 }
};

static const SetDefaultColors defaultSelectionColors = {
    {  67, 172, 232 },
    {  62, 138, 204 },
    { 255, 255, 255 },
    { 199, 226, 248 },
    { 108,  36, 119 },
    {   0,  49, 110 },
    {  69,  40, 134 },
    { 156,  14,  14 },
    { 255, 221,   0 },
    { 128, 255, 128 }
};

static const SetDefaultColors defaultTooltipColors = {
    {  24,  21,  19 },
    { 196, 224, 255 },
    { 231, 253, 255 },
    { 137, 136, 135 },
    { 255, 128, 224 },
    {  88, 172, 255 },
    { 150, 111, 232 },
    { 191,   3,   3 },
    { 176, 128,   0 },
    {   0, 110,  40 }
};

static const DecoDefaultColors defaultDecorationColors = {
    { 110, 214, 255 },
    {  58, 167, 221 },
};

class KColorSchemePrivate : public QSharedData
{
public:
    explicit KColorSchemePrivate(const KSharedConfigPtr &, QPalette::ColorGroup, const char *, SetDefaultColors);
    explicit KColorSchemePrivate(const KSharedConfigPtr &, QPalette::ColorGroup, const char *, SetDefaultColors, const QBrush &);
    ~KColorSchemePrivate() {}

    QBrush background(KColorScheme::BackgroundRole) const;
    QBrush foreground(KColorScheme::ForegroundRole) const;
    QBrush decoration(KColorScheme::DecorationRole) const;
    qreal contrast() const;
private:
    struct {
        QBrush fg[8], bg[8], deco[2];
    } _brushes;
    qreal _contrast;

    void init(const KSharedConfigPtr &, QPalette::ColorGroup, const char *, SetDefaultColors);
};

#define DEFAULT(c) QColor( c[0], c[1], c[2] )
#define  SET_DEFAULT(a) DEFAULT( defaults.a )
#define DECO_DEFAULT(a) DEFAULT( defaultDecorationColors.a )

KColorSchemePrivate::KColorSchemePrivate(const KSharedConfigPtr &config,
        QPalette::ColorGroup state,
        const char *group,
        SetDefaultColors defaults)
{
    KConfigGroup cfg(config, group);
    _contrast = KColorScheme::contrastF(config);

    _brushes.bg[0] = cfg.readEntry("BackgroundNormal", SET_DEFAULT(NormalBackground));
    _brushes.bg[1] = cfg.readEntry("BackgroundAlternate", SET_DEFAULT(AlternateBackground));

    init(config, state, group, defaults);
}

KColorSchemePrivate::KColorSchemePrivate(const KSharedConfigPtr &config,
        QPalette::ColorGroup state,
        const char *group,
        SetDefaultColors defaults,
        const QBrush &tint)
{
    KConfigGroup cfg(config, group);
    _contrast = KColorScheme::contrastF(config);

    _brushes.bg[0] = cfg.readEntry("BackgroundNormal", SET_DEFAULT(NormalBackground));
    _brushes.bg[1] = cfg.readEntry("BackgroundAlternate", SET_DEFAULT(AlternateBackground));

    _brushes.bg[0] = KColorUtils::tint(_brushes.bg[0].color(), tint.color(), 0.4);
    _brushes.bg[1] = KColorUtils::tint(_brushes.bg[1].color(), tint.color(), 0.4);

    init(config, state, group, defaults);
}

void KColorSchemePrivate::init(const KSharedConfigPtr &config,
                               QPalette::ColorGroup state,
                               const char *group,
                               SetDefaultColors defaults)
{
    KConfigGroup cfg(config, group);

    _brushes.fg[0] = cfg.readEntry("ForegroundNormal", SET_DEFAULT(NormalText));
    _brushes.fg[1] = cfg.readEntry("ForegroundInactive", SET_DEFAULT(InactiveText));
    _brushes.fg[2] = cfg.readEntry("ForegroundActive", SET_DEFAULT(ActiveText));
    _brushes.fg[3] = cfg.readEntry("ForegroundLink", SET_DEFAULT(LinkText));
    _brushes.fg[4] = cfg.readEntry("ForegroundVisited", SET_DEFAULT(VisitedText));
    _brushes.fg[5] = cfg.readEntry("ForegroundNegative", SET_DEFAULT(NegativeText));
    _brushes.fg[6] = cfg.readEntry("ForegroundNeutral", SET_DEFAULT(NeutralText));
    _brushes.fg[7] = cfg.readEntry("ForegroundPositive", SET_DEFAULT(PositiveText));

    _brushes.deco[0] = cfg.readEntry("DecorationHover", DECO_DEFAULT(Hover));
    _brushes.deco[1] = cfg.readEntry("DecorationFocus", DECO_DEFAULT(Focus));

    if (state != QPalette::Active) {
        StateEffects effects(state, config);
        for (int i = 0; i < 8; i++) {
            _brushes.fg[i] = effects.brush(_brushes.fg[i], _brushes.bg[0]);
        }
        _brushes.deco[0] = effects.brush(_brushes.deco[0], _brushes.bg[0]);
        _brushes.deco[1] = effects.brush(_brushes.deco[1], _brushes.bg[0]);
        _brushes.bg[0] = effects.brush(_brushes.bg[0]);
        _brushes.bg[1] = effects.brush(_brushes.bg[1]);
    }

    _brushes.bg[2] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[2].color());
    _brushes.bg[3] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[3].color());
    _brushes.bg[4] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[4].color());
    _brushes.bg[5] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[5].color());
    _brushes.bg[6] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[6].color());
    _brushes.bg[7] = KColorUtils::tint(_brushes.bg[0].color(), _brushes.fg[7].color());
}

QBrush KColorSchemePrivate::background(KColorScheme::BackgroundRole role) const
{
    switch (role) {
    case KColorScheme::AlternateBackground:
        return _brushes.bg[1];
    case KColorScheme::ActiveBackground:
        return _brushes.bg[2];
    case KColorScheme::LinkBackground:
        return _brushes.bg[3];
    case KColorScheme::VisitedBackground:
        return _brushes.bg[4];
    case KColorScheme::NegativeBackground:
        return _brushes.bg[5];
    case KColorScheme::NeutralBackground:
        return _brushes.bg[6];
    case KColorScheme::PositiveBackground:
        return _brushes.bg[7];
    default:
        return _brushes.bg[0];
    }
}

QBrush KColorSchemePrivate::foreground(KColorScheme::ForegroundRole role) const
{
    switch (role) {
    case KColorScheme::InactiveText:
        return _brushes.fg[1];
    case KColorScheme::ActiveText:
        return _brushes.fg[2];
    case KColorScheme::LinkText:
        return _brushes.fg[3];
    case KColorScheme::VisitedText:
        return _brushes.fg[4];
    case KColorScheme::NegativeText:
        return _brushes.fg[5];
    case KColorScheme::NeutralText:
        return _brushes.fg[6];
    case KColorScheme::PositiveText:
        return _brushes.fg[7];
    default:
        return _brushes.fg[0];
    }
}

QBrush KColorSchemePrivate::decoration(KColorScheme::DecorationRole role) const
{
    switch (role) {
    case KColorScheme::FocusColor:
        return _brushes.deco[1];
    default:
        return _brushes.deco[0];
    }
}

qreal KColorSchemePrivate::contrast() const
{
    return _contrast;
}

KColorScheme::KColorScheme(const KColorScheme &other) : d(other.d)
{
}

KColorScheme &KColorScheme::operator=(const KColorScheme &other)
{
    d = other.d;
    return *this;
}

KColorScheme::~KColorScheme()
{
}

KColorScheme::KColorScheme(QPalette::ColorGroup state, ColorSet set, KSharedConfigPtr config)
{
    if (!config) {
        config = KSharedConfig::openConfig();
    }

    switch (set) {
    case Window:
        d = new KColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors);
        break;
    case Button:
        d = new KColorSchemePrivate(config, state, "Colors:Button", defaultButtonColors);
        break;
    case Selection: {
        KConfigGroup group(config, "ColorEffects:Inactive");
        bool inactiveSelectionEffect = group.readEntry("ChangeSelectionColor", group.readEntry("Enable", true));
        if (state == QPalette::Active || (state == QPalette::Inactive && !inactiveSelectionEffect)) {
            d = new KColorSchemePrivate(config, state, "Colors:Selection", defaultSelectionColors);
        } else if (state == QPalette::Inactive)
            d = new KColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors,
                                        KColorScheme(QPalette::Active, Selection, config).background());
        else {
            d = new KColorSchemePrivate(config, state, "Colors:Window", defaultWindowColors);
        }
    } break;
    case Tooltip:
        d = new KColorSchemePrivate(config, state, "Colors:Tooltip", defaultTooltipColors);
        break;
    default:
        d = new KColorSchemePrivate(config, state, "Colors:View", defaultViewColors);
    }
}

int KColorScheme::contrast()
{
    KConfigGroup g(KSharedConfig::openConfig(), "KDE");
    return g.readEntry("contrast", 7);
}

qreal KColorScheme::contrastF(const KSharedConfigPtr &config)
{
    if (config) {
        KConfigGroup g(config, "KDE");
        return 0.1 * g.readEntry("contrast", 7);
    }
    return 0.1 * (qreal)contrast();
}

QBrush KColorScheme::background(BackgroundRole role) const
{
    return d->background(role);
}

QBrush KColorScheme::foreground(ForegroundRole role) const
{
    return d->foreground(role);
}

QBrush KColorScheme::decoration(DecorationRole role) const
{
    return d->decoration(role);
}

QColor KColorScheme::shade(ShadeRole role) const
{
    return shade(background().color(), role, d->contrast());
}

QColor KColorScheme::shade(const QColor &color, ShadeRole role)
{
    return shade(color, role, KColorScheme::contrastF());
}

QColor KColorScheme::shade(const QColor &color, ShadeRole role, qreal contrast, qreal chromaAdjust)
{
    contrast = (1.0 > contrast ? (-1.0 < contrast ? contrast : -1.0) : 1.0);
    qreal y = KColorUtils::luma(color), yi = 1.0 - y;

    if (y < 0.006) {
        switch (role) {
        case KColorScheme::LightShade:
            return KColorUtils::shade(color, 0.05 + 0.95 * contrast, chromaAdjust);
        case KColorScheme::MidShade:
            return KColorUtils::shade(color, 0.01 + 0.20 * contrast, chromaAdjust);
        case KColorScheme::DarkShade:
            return KColorUtils::shade(color, 0.02 + 0.40 * contrast, chromaAdjust);
        default:
            return KColorUtils::shade(color, 0.03 + 0.60 * contrast, chromaAdjust);
        }
    }

    if (y > 0.93) {
        switch (role) {
        case KColorScheme::MidlightShade:
            return KColorUtils::shade(color, -0.02 - 0.20 * contrast, chromaAdjust);
        case KColorScheme::DarkShade:
            return KColorUtils::shade(color, -0.06 - 0.60 * contrast, chromaAdjust);
        case KColorScheme::ShadowShade:
            return KColorUtils::shade(color, -0.10 - 0.90 * contrast, chromaAdjust);
        default:
            return KColorUtils::shade(color, -0.04 - 0.40 * contrast, chromaAdjust);
        }
    }

    qreal lightAmount = (0.05 + y * 0.55) * (0.25 + contrast * 0.75);
    qreal darkAmount = (- y) * (0.55 + contrast * 0.35);
    switch (role) {
    case KColorScheme::LightShade:
        return KColorUtils::shade(color, lightAmount, chromaAdjust);
    case KColorScheme::MidlightShade:
        return KColorUtils::shade(color, (0.15 + 0.35 * yi) * lightAmount, chromaAdjust);
    case KColorScheme::MidShade:
        return KColorUtils::shade(color, (0.35 + 0.15 * y) * darkAmount, chromaAdjust);
    case KColorScheme::DarkShade:
        return KColorUtils::shade(color, darkAmount, chromaAdjust);
    default:
        return KColorUtils::darken(KColorUtils::shade(color, darkAmount, chromaAdjust), 0.5 + 0.3 * y);
    }
}

void KColorScheme::adjustBackground(QPalette &palette, BackgroundRole newRole, QPalette::ColorRole color,
                                    ColorSet set, KSharedConfigPtr config)
{
    palette.setBrush(QPalette::Active,   color, KColorScheme(QPalette::Active,   set, config).background(newRole));
    palette.setBrush(QPalette::Inactive, color, KColorScheme(QPalette::Inactive, set, config).background(newRole));
    palette.setBrush(QPalette::Disabled, color, KColorScheme(QPalette::Disabled, set, config).background(newRole));
}

void KColorScheme::adjustForeground(QPalette &palette, ForegroundRole newRole, QPalette::ColorRole color,
                                    ColorSet set, KSharedConfigPtr config)
{
    palette.setBrush(QPalette::Active,   color, KColorScheme(QPalette::Active,   set, config).foreground(newRole));
    palette.setBrush(QPalette::Inactive, color, KColorScheme(QPalette::Inactive, set, config).foreground(newRole));
    palette.setBrush(QPalette::Disabled, color, KColorScheme(QPalette::Disabled, set, config).foreground(newRole));
}

QPalette KColorScheme::createApplicationPalette(const KSharedConfigPtr &config)
{
    QPalette palette;

    static const QPalette::ColorGroup states[3] = { QPalette::Active, QPalette::Inactive, QPalette::Disabled };

    KColorScheme schemeTooltip(QPalette::Active, KColorScheme::Tooltip, config);

    for (int i = 0; i < 3; i++) {
        QPalette::ColorGroup state = states[i];
        KColorScheme schemeView(state, KColorScheme::View, config);
        KColorScheme schemeWindow(state, KColorScheme::Window, config);
        KColorScheme schemeButton(state, KColorScheme::Button, config);
        KColorScheme schemeSelection(state, KColorScheme::Selection, config);

        palette.setBrush(state, QPalette::WindowText, schemeWindow.foreground());
        palette.setBrush(state, QPalette::Window, schemeWindow.background());
        palette.setBrush(state, QPalette::Base, schemeView.background());
        palette.setBrush(state, QPalette::Text, schemeView.foreground());
        palette.setBrush(state, QPalette::Button, schemeButton.background());
        palette.setBrush(state, QPalette::ButtonText, schemeButton.foreground());
        palette.setBrush(state, QPalette::Highlight, schemeSelection.background());
        palette.setBrush(state, QPalette::HighlightedText, schemeSelection.foreground());
        palette.setBrush(state, QPalette::ToolTipBase, schemeTooltip.background());
        palette.setBrush(state, QPalette::ToolTipText, schemeTooltip.foreground());
        palette.setBrush(state, QPalette::PlaceholderText, schemeView.foreground(KColorScheme::InactiveText));

        palette.setColor(state, QPalette::Light, schemeWindow.shade(KColorScheme::LightShade));
        palette.setColor(state, QPalette::Midlight, schemeWindow.shade(KColorScheme::MidlightShade));
        palette.setColor(state, QPalette::Mid, schemeWindow.shade(KColorScheme::MidShade));
        palette.setColor(state, QPalette::Dark, schemeWindow.shade(KColorScheme::DarkShade));
        palette.setColor(state, QPalette::Shadow, schemeWindow.shade(KColorScheme::ShadowShade));

        palette.setBrush(state, QPalette::AlternateBase, schemeView.background(KColorScheme::AlternateBackground));
        palette.setBrush(state, QPalette::Link, schemeView.foreground(KColorScheme::LinkText));
        palette.setBrush(state, QPalette::LinkVisited, schemeView.foreground(KColorScheme::VisitedText));
    }

    return palette;
}

class KStatefulBrushPrivate : public QBrush
{
public:
    KStatefulBrushPrivate() : QBrush() {}
    KStatefulBrushPrivate(const QBrush &brush) : QBrush(brush) {}
};

KStatefulBrush::KStatefulBrush()
{
    d = new KStatefulBrushPrivate[3];
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::ForegroundRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[3];
    d[0] = KColorScheme(QPalette::Active,   set, config).foreground(role);
    d[1] = KColorScheme(QPalette::Disabled, set, config).foreground(role);
    d[2] = KColorScheme(QPalette::Inactive, set, config).foreground(role);
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::BackgroundRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[3];
    d[0] = KColorScheme(QPalette::Active,   set, config).background(role);
    d[1] = KColorScheme(QPalette::Disabled, set, config).background(role);
    d[2] = KColorScheme(QPalette::Inactive, set, config).background(role);
}

KStatefulBrush::KStatefulBrush(KColorScheme::ColorSet set, KColorScheme::DecorationRole role,
                               KSharedConfigPtr config)
{
    d = new KStatefulBrushPrivate[3];
    d[0] = KColorScheme(QPalette::Active,   set, config).decoration(role);
    d[1] = KColorScheme(QPalette::Disabled, set, config).decoration(role);
    d[2] = KColorScheme(QPalette::Inactive, set, config).decoration(role);
}

KStatefulBrush::KStatefulBrush(const QBrush &brush, KSharedConfigPtr config)
{
    if (!config) {
        config = KSharedConfig::openConfig();
    }
    d = new KStatefulBrushPrivate[3];
    d[0] = brush;
    d[1] = StateEffects(QPalette::Disabled, config).brush(brush);
    d[2] = StateEffects(QPalette::Inactive, config).brush(brush);
}

KStatefulBrush::KStatefulBrush(const QBrush &brush, const QBrush &background,
                               KSharedConfigPtr config)
{
    if (!config) {
        config = KSharedConfig::openConfig();
    }
    d = new KStatefulBrushPrivate[3];
    d[0] = brush;
    d[1] = StateEffects(QPalette::Disabled, config).brush(brush, background);
    d[2] = StateEffects(QPalette::Inactive, config).brush(brush, background);
}

KStatefulBrush::KStatefulBrush(const KStatefulBrush &other)
{
    d = new KStatefulBrushPrivate[3];
    d[0] = other.d[0];
    d[1] = other.d[1];
    d[2] = other.d[2];
}

KStatefulBrush::~KStatefulBrush()
{
    delete[] d;
}

KStatefulBrush &KStatefulBrush::operator=(const KStatefulBrush &other)
{
    d[0] = other.d[0];
    d[1] = other.d[1];
    d[2] = other.d[2];
    return *this;
}

QBrush KStatefulBrush::brush(QPalette::ColorGroup state) const
{
    switch (state) {
    case QPalette::Inactive:
        return d[2];
    case QPalette::Disabled:
        return d[1];
    default:
        return d[0];
    }
}

QBrush KStatefulBrush::brush(const QPalette &pal) const
{
    return brush(pal.currentColorGroup());
}

QBrush KStatefulBrush::brush(const QWidget *widget) const
{
    if (widget) {
        return brush(widget->palette());
    } else {
        return QBrush();
    }
}

