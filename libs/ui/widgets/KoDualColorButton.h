/* This file is part of the KDE libraries

   SPDX-FileCopyrightText: 1999 Daniel M. Duley <mosfet@kde.org>
   SPDX-FileCopyrightText: 2006 Tobias Koenig <tokoe@kde.org>
   SPDX-FileCopyrightText: 2026 Arkady Flury <arkady.flury@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KODUALCOLORBUTTON_H
#define KODUALCOLORBUTTON_H

#include <kritaui_export.h>
#include <KoColorDisplayRendererInterface.h>

#include <QWidget>

class KoColor;
class KoColorSpace;
class KisCanvasResourceProvider;
class KRITAUI_EXPORT KoDualColorButton : public QWidget
{
    Q_OBJECT
    Q_ENUMS( Selection )
    Q_PROPERTY( KoColor foregroundColor READ foregroundColor WRITE setForegroundColor )
    Q_PROPERTY( KoColor backgroundColor READ backgroundColor WRITE setBackgroundColor )
    Q_PROPERTY( bool popDialog READ popDialog WRITE setPopDialog )

  public:
    enum Selection {
      Foreground,
      Background
    };
    QRect foregroundRect = QRect( 0, 0, 14, 14);
    QRect backgroundRect = QRect( 14, 14, 28, 28);
    QPixmap resetArrowPixmap;
    QPixmap resetPixmap;
    QPixmap arrowBitmap;

    bool resetColours = false;

    KoDualColorButton(KisCanvasResourceProvider *canvasResourceProvider,
                      const KoColorDisplayRendererInterface *displayRenderer,
                      QWidget *parent = 0, QWidget* dialogParent = 0);

    ~KoDualColorButton() override;

    KoColor foregroundColor() const;

    KoColor backgroundColor() const;

    bool popDialog() const;

    QSize sizeHint() const override;

  public Q_SLOTS:
    void updateArrows();

    void ResetColours();
    void swapColours();

    void setColorDialogState(Selection state);

    virtual void paint_icons(QPainter &painter);

    void setForegroundColor( const KoColor &color );

    void setBackgroundColor( const KoColor &color );

    void foregroundSelect(bool usePlatformDialog);
    void backgroundSelect(bool usePlatformDialog);

    void slotSetForegroundColorFromDialog (const KoColor color);
    void slotSetBackgroundColorFromDialog (const KoColor color);
    void slotColorDialogClosed();

    void setDisplayRenderer(const KoColorDisplayRendererInterface *displayRenderer = KoDumbColorDisplayRenderer::instance());

    void updateColorSpace();

    QColor getColorFromDisplayRenderer(KoColor c);

    void setPopDialog( bool popDialog );

    void openForegroundDialog();

    void openBackgroundDialog();
  Q_SIGNALS:
    void foregroundColorChanged( const KoColor &color );

    void backgroundColorChanged( const KoColor &color );

  protected:

    void paintEvent( QPaintEvent *event ) override;
    virtual void mousePressEvent( QMouseEvent *event ) override;
    virtual void mouseMoveEvent( QMouseEvent *event ) override;
    virtual void mouseReleaseEvent( QMouseEvent *event ) override;
    void dragEnterEvent( QDragEnterEvent *event ) override;
    void dropEvent( QDropEvent *event ) override;
    void changeEvent(QEvent *event) override;
    bool event(QEvent *event) override;

  private:
    class Private;
    Private *const d;

};

#endif

