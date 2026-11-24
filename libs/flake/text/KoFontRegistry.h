/*
 *  SPDX-FileCopyrightText: 2022 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOFONTREGISTRY_H
#define KOFONTREGISTRY_H

#include <QObject>
#include <QScopedPointer>
#include <QVector>

#include <KoFontLibraryResourceUtils.h>
#include <KoFFWWSConverter.h>

#include "KoCSSFontInfo.h"
#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoFontRegistry: public QObject
{
    Q_OBJECT
public:
    KoFontRegistry(QObject *parent = nullptr);
    ~KoFontRegistry();

    static KoFontRegistry *instance();

    bool addBundledFontFile(const QString &path) { return addFontFilePathToRegistry(path); }

    std::vector<FT_FaceSP> facesForCSSValues(QVector<int> &lengths,
                                             KoCSSFontInfo info = KoCSSFontInfo(),
                                             const QString &text = "",
                                             quint32 xRes = 72,
                                             quint32 yRes = 72,
                                             bool disableFontMatching = false,
                                             const QString &language = QString());

    bool configureFaces(const std::vector<FT_FaceSP> &faces,
                        qreal size,
                        qreal fontSizeAdjust,
                        quint32 xRes,
                        quint32 yRes,
                        const QMap<QString, qreal> &axisSettings);

    QList<KoFontFamilyWWSRepresentation> collectRepresentations() const;

    std::optional<KoFontFamilyWWSRepresentation> representationByFamilyName(const QString &familyName) const;

    std::optional<QString> wwsNameByFamilyName(const QString familyName) const;

    static QFont::Style slantMode(FT_FaceSP face);

    KoSvgText::FontMetrics fontMetricsForCSSValues(KoCSSFontInfo info = KoCSSFontInfo(),
                                                   const bool isHorizontal = true,
                                                   const KoSvgText::TextRendering rendering = KoSvgText::RenderingAuto,
                                                   const QString &text = "",
                                                   quint32 xRes = 72,
                                                   quint32 yRes = 72,
                                                   bool disableFontMatching = false,
                                                   const QString &language = QString());

    static KoSvgText::FontMetrics generateFontMetrics(FT_FaceSP face, bool isHorizontal = true, QString script = QString(), const KoSvgText::TextRendering rendering = KoSvgText::RenderingAuto);

    static int32_t loadFlagsForFace(FT_Face face, bool isHorizontal = true, int32_t loadFlags = 0, const KoSvgText::TextRendering rendering = KoSvgText::RenderingAuto);

    KoCSSFontInfo getCssDataForPostScriptName (const QString postScriptName,
                                      QString *foundPostScriptName);
private Q_SLOTS:
    void updateConfig();
private:
    class Private;

    friend class TestSvgText;
    friend class SvgTextCursorTest;

    bool addFontFilePathToRegistry(const QString &path);

    bool addFontFileDirectoryToRegistry(const QString &path);

    QScopedPointer<Private> d;

    Q_DISABLE_COPY(KoFontRegistry);
};

#endif
