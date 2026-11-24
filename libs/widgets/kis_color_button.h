/*
 *  SPDX-FileCopyrightText: 2016 Wolthera van Hovell tot Westerflier <griffinvalley@gmail.com>
 *  This file is forked from the KF5 KColorButton
    SPDX-FileCopyrightText: 1997 Martin Jones (mjones@kde.org)

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KisColorButton_H
#define KisColorButton_H
#include <kritawidgets_export.h>

#include <KoColor.h>
#include <QPushButton>

class KisColorButtonPrivate;
class KRITAWIDGETS_EXPORT KisColorButton : public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(KoColor color READ color WRITE setColor NOTIFY changed USER true DESIGNABLE false)
    Q_PROPERTY(KoColor defaultColor READ defaultColor WRITE setDefaultColor DESIGNABLE false)
    Q_PROPERTY(bool alphaChannelEnabled READ isAlphaChannelEnabled WRITE setAlphaChannelEnabled)

public:
    explicit KisColorButton(QWidget *parent = 0);

    explicit KisColorButton(const KoColor &c, QWidget *parent = 0);

    KisColorButton(const KoColor &c, const KoColor &defaultColor, QWidget *parent = 0);

    ~KisColorButton() override;

    KoColor color() const;

    void setAlphaChannelEnabled(bool alpha);

    bool isAlphaChannelEnabled() const;

    void setPaletteViewEnabled( bool enable);

    bool paletteViewEnabled() const;
    KoColor defaultColor() const;

    void setDefaultColor(const KoColor &c);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setColor(const KoColor &c);
Q_SIGNALS:
    void changed(const KoColor &newColor);

protected:
    void paintEvent(QPaintEvent *pe) override;
    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    class KisColorButtonPrivate;
    KisColorButtonPrivate *const d;

    Q_PRIVATE_SLOT(d, void _k_chooseColor())
};

#endif
