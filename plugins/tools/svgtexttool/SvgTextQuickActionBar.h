/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef SVGTEXTQUICKACTIONBAR_H
#define SVGTEXTQUICKACTIONBAR_H

#include <QWidget>

class SvgTextQuickActionBar : public QWidget
{
    Q_OBJECT
public:
    explicit SvgTextQuickActionBar(QWidget *parent = nullptr);
    ~SvgTextQuickActionBar() override = default;

    void setPasteEnabled(bool enabled);

Q_SIGNALS:
    void sigCut();
    void sigCopy();
    void sigPaste();
    void sigSelectAll();

private:
    QAction *m_pasteAction;
};

#endif
