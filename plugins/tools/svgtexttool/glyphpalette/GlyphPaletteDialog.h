/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef GLYPHPALETTEDIALOG_H
#define GLYPHPALETTEDIALOG_H

#include <KoDialog.h>
#include <KisQQuickWidget.h>

#include <KoSvgTextProperties.h>
#include <KoFontGlyphModel.h>
#include <KoSvgTextShape.h>
#include <QStandardItemModel>

#include "GlyphPaletteProxyModel.h"
#include "GlyphPaletteAltPopup.h"

class GlyphPaletteDialog: public KoDialog
{
    Q_OBJECT
public:
    GlyphPaletteDialog(QWidget *parent = nullptr);
    ~GlyphPaletteDialog();

    void setGlyphModelFromProperties(const QPair<KoSvgTextProperties, KoSvgTextProperties> &properties, const QString &text);

Q_SIGNALS:
    void signalInsertRichText(KoSvgTextShape *text, bool replace);
public Q_SLOTS:

    void slotInsertRichText(const int charRow, const int glyphRow = -1, const bool replace = false, const bool useCharMap = false);
    void slotShowPopupPalette(const int charRow, const int x = 0, const int y = 0, const int cellWidth = 100, const int cellHeight = 100);
    void slotHidePopupPalette();

private:
    KisQQuickWidget *m_quickWidget {0};
    GlyphPaletteAltPopup *m_altPopup {0};
    QPair<KoSvgTextProperties, KoSvgTextProperties> m_lastUsedProperties;
    KoFontGlyphModel *m_model;
    GlyphPaletteProxyModel *m_charMapModel;
};

#endif
