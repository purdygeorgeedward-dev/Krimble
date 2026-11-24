/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KISRESOURCEITEMDELEGATE_H
#define KISRESOURCEITEMDELEGATE_H

#include <QAbstractItemDelegate>

#include "KisResourceThumbnailPainter.h"
#include "KoCheckerBoardPainter.h"
#include "kritaresourcewidgets_export.h"

class KRITARESOURCEWIDGETS_EXPORT KisResourceItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit KisResourceItemDelegate(QObject *parent = 0);
    ~KisResourceItemDelegate() override {}

    void paint( QPainter *, const QStyleOptionViewItem &, const QModelIndex & ) const override;
    void setShowText(bool);

    void setNeedIndexConversion(bool value);

    QSize sizeHint ( const QStyleOptionViewItem &, const QModelIndex & ) const override;

private:
    QModelIndex convertToGlobalModelIndexIfNeeded(const QModelIndex &localIndex) const;

private:
    KoCheckerBoardPainter m_checkerPainter;
    KisResourceThumbnailPainter m_thumbnailPainter;
    bool m_showText;
    bool m_isWidget;
};

#endif
