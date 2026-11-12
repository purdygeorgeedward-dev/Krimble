/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KO_TOOLBOXBUTTON_H_
#define _KO_TOOLBOXBUTTON_H_

#include <QToolButton>
#include <QTimer>
#include <QPoint>

class KoToolAction;

class KoToolBoxButton : public QToolButton
{
    Q_OBJECT
public:
    explicit KoToolBoxButton(KoToolAction *toolAction, QWidget * parent);
    void attachAction(QAction *action);
    void setHighlightColor();
    void krimbleRefreshIcon();
    // Krimble 2026-11-12 (task 38): George: "I want long press to open a flyout of other buttons. Then a tap selects one or misses and cancels selection." A button of a tool group (Marquee, Lasso, Wand) opens a flyout of the other tools of its group on a long press.
    void krimbleSetLongPress(bool on) { m_krimbleLongPress = on; update(); }
    bool krimbleHasLongPress() const { return m_krimbleLongPress; }
    KoToolAction *krimbleToolAction() const { return m_toolAction; }

Q_SIGNALS:
    void krimbleLongPressed();
   // Krimble 2026-10-30: the icon again after the icon style was changed

protected:
    void mousePressEvent(QMouseEvent *event) override;     // Krimble 2026-11-12 (task 38): George: "I want long press to open a flyout of other buttons. Then a tap selects one or misses and cancels selection."
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *event) override;   // Krimble 2026-11-07: the selected tool drawn like the mockup (deep blue, bright cyan outline, soft glow)

private:
    void setDataFromToolAction(QAction *action); // Generates tooltips.
private Q_SLOTS:
    void slotUpdateActionData();
private:
    KoToolAction *m_toolAction;
    bool m_krimbleLongPress {false};        // Krimble 2026-11-12 (task 38): George: "I want long press to open a flyout of other buttons. Then a tap selects one or misses and cancels selection."
    bool m_krimbleLongPressFired {false};
    QPoint m_krimblePressPos;
    QTimer m_krimbleTimer;
};

#endif // _KO_TOOLBOXBUTTON_H_
