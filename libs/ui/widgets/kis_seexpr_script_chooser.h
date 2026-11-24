/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2020 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_SEEXPR_SCRIPT_CHOOSER_H_
#define KIS_SEEXPR_SCRIPT_CHOOSER_H_

#include <QFrame>
#include <kritaui_export.h>
#include <KSqueezedTextLabel>
#include <KisResourceItemChooser.h>

class KRITAUI_EXPORT KisSeExprScriptChooser : public QFrame
{

    Q_OBJECT

public:
    KisSeExprScriptChooser(QWidget *parent = 0);
    ~KisSeExprScriptChooser() override;

    KoResourceSP currentResource();
    void setCurrentScript(KoResourceSP resource);
    void setCurrentItem(int row);
    void setPreviewOrientation(Qt::Orientation orientation);

Q_SIGNALS:

    void resourceSelected(KoResourceSP resource);
    void updateItemSize();

private Q_SLOTS:

    void update(KoResourceSP resource);

private:

    KSqueezedTextLabel *m_lblName;
    KisResourceItemChooser *m_itemChooser;
};

#endif

