/*
 *  SPDX-FileCopyrightText: 2018 Jouni Pentikäinen <joupent@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISWINDOWLAYOUTMANAGER_H
#define KISWINDOWLAYOUTMANAGER_H

#include <QObject>
#include <QUuid>
#include <QVector>
#include <QSize>

#include <KisWindowLayoutResource.h>

class QScreen;
class KisDocument;

class KisWindowLayoutManager : public QObject
{
    Q_OBJECT

public:
    struct Display
    {
        QSize resolution;

        bool matches(QScreen* screen) const;
    };

    struct DisplayLayout
    {
        QString name;

        QVector<Display> displays;
        QString preferredWindowLayout;

        bool matches(QList<QScreen*> screens) const;
    };

    explicit KisWindowLayoutManager();
    ~KisWindowLayoutManager();

    static KisWindowLayoutManager *instance();

    bool primaryWorkspaceFollowsFocus() const;
    void setPrimaryWorkspaceFollowsFocus(bool enabled, QUuid primaryWindow);
    QUuid primaryWindowId() const;

    bool isShowImageInAllWindowsEnabled() const;
    void setShowImageInAllWindowsEnabled(bool showInAll);

    void activeDocumentChanged(KisDocument *document);

    void setLastUsedLayout(KisWindowLayoutResource *layout);

private Q_SLOTS:
    void slotFocusChanged(QWidget*, QWidget*);
    void slotScreensChanged();

private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
