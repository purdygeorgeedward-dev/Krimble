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
    void krimbleFitIconSize();
    void krimbleSetLongPress(bool on) { m_krimbleLongPress = on; update(); }
    bool krimbleHasLongPress() const { return m_krimbleLongPress; }
    KoToolAction *krimbleToolAction() const { return m_toolAction; }
    static void krimblePaintRaisedBox(QWidget *widget, bool down);

Q_SIGNALS:
    void krimbleLongPressed();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setDataFromToolAction(QAction *action); // Generates tooltips.
private Q_SLOTS:
    void slotUpdateActionData();
private:
    KoToolAction *m_toolAction;
    bool m_krimbleLongPress {false};
    bool m_krimbleLongPressFired {false};
    QSize m_krimbleBaseIconSize;
    QSize m_krimbleSetIconSize;       // the size set for a full color picture (so a size set by the toolbox afterwards is noticed)
    bool m_krimbleColorFit {false};
    QPoint m_krimblePressPos;
    QTimer m_krimbleTimer;
};

#endif // _KO_TOOLBOXBUTTON_H_
