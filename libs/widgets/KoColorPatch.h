/**
 * SPDX-FileCopyrightText: 2006 C. Boemann (cbo@boemann.dk)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOCOLORPATCH_H
#define KOCOLORPATCH_H

#include <QFrame>

#include <KoColor.h>
#include "kritawidgets_export.h"
#include <KoColorDisplayRendererInterface.h>

class KRITAWIDGETS_EXPORT KoColorPatch : public QFrame
{
  Q_OBJECT
public:
    explicit KoColorPatch( QWidget *parent );
    ~KoColorPatch() override;

    void setColor( const KoColor &c );

    void setDisplayRenderer(const KoColorDisplayRendererInterface *displayRenderer);

    QColor getColorFromDisplayRenderer(KoColor c);

    KoColor color() const;

protected:
    void mousePressEvent(QMouseEvent *e ) override;
    void paintEvent(QPaintEvent *e) override;
    QSize sizeHint() const override;

Q_SIGNALS:

    void triggered(KoColorPatch *widget);

private:
  KoColor m_color;
  const KoColorDisplayRendererInterface *m_displayRenderer;
};

#endif
