/*
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_COLOR_SELECTOR_BASE_H
#define KIS_COLOR_SELECTOR_BASE_H

#include <QWidget>
#include <QRgb>
#include <QPointer>
#include <kis_canvas2.h>
#include "kis_acs_types.h"
#include "kis_signal_compressor_with_param.h"

class KoColor;
class QTimer;
class KoColorSpace;
class KisCanvas2;
class KisColorPreviewPopup;
class KisDisplayColorConverter;
class KoGamutMask;

class KisColorSelectorBase : public QWidget
{
Q_OBJECT
public:
    enum Move {MoveToMousePosition, DontMove};
    explicit KisColorSelectorBase(QWidget *parent = 0);
    ~KisColorSelectorBase() override;

    void setPopupBehaviour(bool onMouseOver, bool onMouseClick);
    void setColorSpace(const KoColorSpace* colorSpace);
    virtual void setCanvas(KisCanvas2* canvas);
    virtual void unsetCanvas();
    const KoColorSpace* colorSpace() const;

    KisDisplayColorConverter* converter() const;

    void tryHideAllPopups();

public:
    void updateColor(const KoColor &color, Acs::ColorRole role, bool needsExplicitColorReset);
    void updateColorPreview(const KoColor &color);
    void showColorPreview();
    void updateBaseColorPreview(const KoColor &color);
    void updatePreviousColorPreview();

    virtual void setColor(const KoColor& color);

public Q_SLOTS:
    virtual void reset();

    virtual void updateSettings();
    virtual void showPopup(Move move=MoveToMousePosition);

public:
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    void enterEvent(QEvent *e) override;
#else
    void enterEvent(QEnterEvent *e) override;
#endif

    void leaveEvent(QEvent *e) override;

    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

protected:
    void keyPressEvent(QKeyEvent *) override;
    virtual KisColorSelectorBase* createPopup() const = 0;
    void dragEnterEvent(QDragEnterEvent *) override;
    void dropEvent(QDropEvent *) override;
    void setHidingTime(int time);
    bool isPopup() const { return m_isPopup; }
    void mouseMoveEvent(QMouseEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void requestUpdateColorAndPreview(const KoColor &color, Acs::ColorRole role);

private:
    void commitColor(const KoColor& koColor, Acs::ColorRole role);

protected Q_SLOTS:
    void hidePopup();

    virtual void canvasResourceChanged(int key, const QVariant& v);

    void updateLastUsedColorPreview(const KoColor &color);

public:
    void slotUpdateColorAndPreview(QPair<KoColor, Acs::ColorRole> color);

private:
    void lazyCreatePopup();

protected:
    QPointer<KisCanvas2> m_canvas;
    KisColorSelectorBase* m_popup;
    QWidget* m_parent;
    bool m_colorUpdateAllowed;

    bool m_colorUpdateSelf;

private:
    QTimer* m_hideTimer;
    bool m_popupOnMouseOver;
    bool m_popupOnMouseClick;
    mutable const KoColorSpace* m_colorSpace;
    bool m_isPopup;
    bool m_hideOnMouseClick;
    KisColorPreviewPopup* m_colorPreviewPopup;

    typedef KisSignalCompressorWithParam<QPair<KoColor, Acs::ColorRole>> ColorCompressorType;
    QScopedPointer<ColorCompressorType> m_updateColorCompressor;
};

#endif
