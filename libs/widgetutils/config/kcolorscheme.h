/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCOLORSCHEME_H
#define KCOLORSCHEME_H

#include <kritawidgetutils_export.h>
#include <ksharedconfig.h>

#include <QExplicitlySharedDataPointer>

#include <QPalette>

class QColor;
class QBrush;

class KColorSchemePrivate;

class KRITAWIDGETUTILS_EXPORT KColorScheme
{
public:

    enum ColorSet {
        View,
        Window,
        Button,
        Selection,
        Tooltip
    };

    enum BackgroundRole {
        NormalBackground = 0,
        AlternateBackground = 1,
        ActiveBackground = 2,
        LinkBackground = 3,
        VisitedBackground = 4,
        NegativeBackground = 5,
        NeutralBackground = 6,
        PositiveBackground = 7
    };

    enum ForegroundRole {
        NormalText = 0,
        InactiveText = 1,
        ActiveText = 2,
        LinkText = 3,
        VisitedText = 4,
        NegativeText = 5,
        NeutralText = 6,
        PositiveText = 7
    };

    enum DecorationRole {
        FocusColor,
        HoverColor
    };

    enum ShadeRole {
        LightShade,
        MidlightShade,
        MidShade,
        DarkShade,
        ShadowShade
    };

    KColorScheme(const KColorScheme &);

    virtual ~KColorScheme();

    KColorScheme &operator=(const KColorScheme &);

    explicit KColorScheme(QPalette::ColorGroup, ColorSet = View, KSharedConfigPtr = KSharedConfigPtr());

    QBrush background(BackgroundRole = NormalBackground) const;

    QBrush foreground(ForegroundRole = NormalText) const;

    QBrush decoration(DecorationRole) const;

    QColor shade(ShadeRole) const;

    static int contrast();

    static qreal contrastF(const KSharedConfigPtr &config = KSharedConfigPtr());

    static QColor shade(const QColor &, ShadeRole);

    static QColor shade(const QColor &, ShadeRole,
                        qreal contrast, qreal chromaAdjust = 0.0);

    static void adjustBackground(QPalette &,
                                 BackgroundRole newRole = NormalBackground,
                                 QPalette::ColorRole color = QPalette::Base,
                                 ColorSet set = View,
                                 KSharedConfigPtr = KSharedConfigPtr());

    static void adjustForeground(QPalette &,
                                 ForegroundRole newRole = NormalText,
                                 QPalette::ColorRole color = QPalette::Text,
                                 ColorSet set = View,
                                 KSharedConfigPtr = KSharedConfigPtr());

    static QPalette createApplicationPalette(const KSharedConfigPtr &config);

private:
    QExplicitlySharedDataPointer<KColorSchemePrivate> d;
};

class KRITAWIDGETUTILS_EXPORT KStatefulBrush
{
public:
    explicit KStatefulBrush();

    explicit KStatefulBrush(KColorScheme::ColorSet,
                            KColorScheme::ForegroundRole,
                            KSharedConfigPtr = KSharedConfigPtr());

    explicit KStatefulBrush(KColorScheme::ColorSet,
                            KColorScheme::BackgroundRole,
                            KSharedConfigPtr = KSharedConfigPtr());

    explicit KStatefulBrush(KColorScheme::ColorSet,
                            KColorScheme::DecorationRole,
                            KSharedConfigPtr = KSharedConfigPtr());

    explicit KStatefulBrush(const QBrush &, KSharedConfigPtr = KSharedConfigPtr());

    explicit KStatefulBrush(const QBrush &, const QBrush &background,
                            KSharedConfigPtr = KSharedConfigPtr());

    KStatefulBrush(const KStatefulBrush &);

    ~KStatefulBrush();

    KStatefulBrush &operator=(const KStatefulBrush &);

    QBrush brush(QPalette::ColorGroup) const;

    QBrush brush(const QPalette &) const;

    QBrush brush(const QWidget *) const;

private:
    class KStatefulBrushPrivate *d;
};

Q_DECLARE_METATYPE(KStatefulBrush)

#endif
