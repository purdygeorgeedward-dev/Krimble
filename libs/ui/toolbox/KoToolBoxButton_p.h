/*
 * SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KO_TOOLBOXBUTTON_H_
#define _KO_TOOLBOXBUTTON_H_

#include <QToolButton>

class KoToolAction;

class KoToolBoxButton : public QToolButton
{
    Q_OBJECT
public:
    explicit KoToolBoxButton(KoToolAction *toolAction, QWidget * parent);
    void attachAction(QAction *action);
    void setHighlightColor();
    void krimbleRefreshIcon();   // Krimble 2026-10-30: the icon again after the icon style was changed

protected:
    void paintEvent(QPaintEvent *event) override;   // Krimble 2026-11-07: the selected tool drawn like the mockup (deep blue, bright cyan outline, soft glow)

private:
    void setDataFromToolAction(QAction *action); // Generates tooltips.
private Q_SLOTS:
    void slotUpdateActionData();
private:
    KoToolAction *m_toolAction;
};

#endif // _KO_TOOLBOXBUTTON_H_
