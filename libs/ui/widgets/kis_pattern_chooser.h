/*
 *  SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_PATTERN_CHOOSER_H_
#define KIS_PATTERN_CHOOSER_H_

#include <QFrame>

#include <KoResource.h>

#include <kritaui_export.h>

class KSqueezedTextLabel;
class KisResourceItemChooser;

class KRITAUI_EXPORT KisPatternChooser : public QFrame
{

    Q_OBJECT

public:
    KisPatternChooser(QWidget *parent = 0);
    ~KisPatternChooser() override;

    KoResourceSP currentResource(bool includeHidden = false);
    void setGrayscalePreview(bool grayscale);
    void setPreviewOrientation(Qt::Orientation orientation);

Q_SIGNALS:

    void resourceSelected(KoResourceSP resource);
    void updateItemSize();

public Q_SLOTS:

    void setCurrentPattern(KoResourceSP resource);
    void setCurrentItem(int row);

private Q_SLOTS:

    void update(KoResourceSP resource);

private:

    KSqueezedTextLabel *m_lblName;
    KisResourceItemChooser *m_itemChooser;
};

#endif

