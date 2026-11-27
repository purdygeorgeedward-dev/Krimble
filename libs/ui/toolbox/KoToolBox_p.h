/*
 * SPDX-FileCopyrightText: 2005 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2005-2008 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2009 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef _KO_TOOLBOX_H_
#define _KO_TOOLBOX_H_

#include <KoCanvasObserverBase.h>

#include <QWidget>
#include <QToolButton>
#include <QList>

#include <KoToolManager.h>

class KisViewManager;
class KoCanvasController;
class KoShapeLayer;
class KoToolBoxLayout;

class KoToolBox : public QWidget {
    Q_OBJECT
public:
    explicit KoToolBox();
    ~KoToolBox() override;

    void applyIconSize();
    void setViewManager(KisViewManager *viewManager);

public Q_SLOTS:
    void setActiveTool(KoCanvasController *canvas);

    void setButtonsVisible(const QList<QString> &codes);

    void setOrientation(Qt::Orientation orientation);

    void setCompact(bool);
    bool isCompact();

    void setFloating(bool v);

    KoToolBoxLayout *toolBoxLayout() const;

private:
    void addButton(KoToolAction *toolAction);
    void addActionButton(QToolButton *button, int priority);

private Q_SLOTS:
    void setCurrentLayer(const KoCanvasController *canvas, const KoShapeLayer* newLayer);

    void toolAdded(KoToolAction *toolAction, KoCanvasController *canvas);

    void slotContextIconSize();

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void krimbleApplyGroups();
    void krimbleShowFlyout(int groupIndex);
    void setupIconSizeMenu(QMenu *menu);
    void setupIconStyleMenu(QMenu *menu);
    void setupButtonStyleMenu(QMenu *menu);

    friend class KoToolBoxDocker;

private:
    class Private;
    Private * const d;
};

#endif
