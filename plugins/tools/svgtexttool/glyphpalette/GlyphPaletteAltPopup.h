/*
 *  SPDX-FileCopyrightText: 2025 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef GLYPHPALETTEALTPOPUP_H
#define GLYPHPALETTEALTPOPUP_H

#include <QFrame>
#include <KisQQuickWidget.h>
#include <QAbstractItemModel>

class GlyphPaletteAltPopup : public QFrame
{
    Q_OBJECT
public:
    GlyphPaletteAltPopup(QWidget *parent = nullptr);
    ~GlyphPaletteAltPopup();

    void setRootIndex(const int index);
    void setCellSize(const int width, const int height);
    void setModel(QAbstractItemModel *model);

    void setMarkup(const QStringList &families, const int size, const int weight, const int width, const QFont::Style style, const QVariantMap &axes, const QString &language);

public Q_SLOTS:

    void slotInsertRichText(const int charRow, const int glyphRow = -1, const bool replace = false, const bool useCharMap = false);
Q_SIGNALS:
    void sigInsertRichText(int charRow, int glyphRow, bool replace, bool useCharMap);
private:
    KisQQuickWidget *m_quickWidget {0};
    QAbstractItemModel *m_charMapModel {0};
};

#endif
