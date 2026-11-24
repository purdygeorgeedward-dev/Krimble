/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 1999, 2000 Kurt Granroth <granroth@kde.org>
   SPDX-FileCopyrightText: 2001, 2002 Ellis Whitehead <ellis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef KSTANDARDACTION_H
#define KSTANDARDACTION_H

#include <kritawidgetutils_export.h>
#include <kstandardshortcut.h>
#include <QList>
#include <QStringList>

class QWidget;
class QObject;
class QAction;
class KRecentFilesAction;
class KDualAction;
class KToggleAction;
class KToggleFullScreenAction;

namespace KStandardAction
{
enum StandardAction {
    ActionNone,

    New, Open, OpenRecent, Save, SaveAs, Revert, Close,
    Print, PrintPreview, Mail, Quit,

    Undo, Redo, Cut, Copy, Paste, SelectAll, Deselect, Find, FindNext, FindPrev,
    Replace,

    ActualSize, FitToPage, FitToWidth, FitToHeight, ZoomIn, ZoomOut,
    Zoom, Redisplay,

    Up, Back, Forward, Home , Prior, Next, Goto, GotoPage, GotoLine,
    FirstPage, LastPage, DocumentBack, DocumentForward,

    AddBookmark, EditBookmarks,

    Spelling,

    ShowMenubar, ShowToolbar, ShowStatusbar,
    SaveOptions, KeyBindings,
    Preferences, ConfigureToolbars, ResetConfigurations,

    Help, HelpContents, WhatsThis, ReportBug, AboutApp, AboutKDE,
    TipofDay,

    ConfigureNotifications,
    FullScreen,
    Clear,
    PasteText,
    SwitchApplicationLanguage
};

KRITAWIDGETUTILS_EXPORT QAction *create(StandardAction id, const QObject *recvr, const char *slot,
                                      QObject *parent);

KRITAWIDGETUTILS_EXPORT const char *name(StandardAction id);

#ifndef KDE_NO_DEPRECATED
inline KRITAWIDGETUTILS_DEPRECATED const char *stdName(StandardAction act_enum)
{
    return name(act_enum);
}
#endif

KRITAWIDGETUTILS_EXPORT QStringList stdNames();

KRITAWIDGETUTILS_EXPORT QList<StandardAction> actionIds();

KRITAWIDGETUTILS_EXPORT KStandardShortcut::StandardShortcut shortcutForActionId(StandardAction id);

KRITAWIDGETUTILS_EXPORT QAction *openNew(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *open(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT KRecentFilesAction *openRecent(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *save(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *saveAs(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *revert(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *close(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *print(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *printPreview(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *mail(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *quit(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *undo(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *redo(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *cut(QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *copy(QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *paste(QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *clear(QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *selectAll(QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *cut(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *copy(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *paste(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *pasteText(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *clear(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *selectAll(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *deselect(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *find(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *findNext(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *findPrev(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *replace(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *actualSize(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *fitToPage(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *fitToWidth(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *fitToHeight(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *zoomIn(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *zoomOut(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *zoom(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *redisplay(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *up(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *back(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *forward(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *home(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *prior(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *next(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *goTo(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *gotoPage(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *gotoLine(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *firstPage(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *lastPage(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *documentBack(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *documentForward(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *addBookmark(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *editBookmarks(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *spelling(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT KToggleAction *showMenubar(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT KToggleAction *showStatusbar(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT KToggleFullScreenAction *fullScreen(const QObject *recvr, const char *slot, QWidget *window, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *saveOptions(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *keyBindings(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *preferences(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *configureToolbars(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *configureNotifications(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *help(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *helpContents(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *whatsThis(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *tipOfDay(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *reportBug(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *aboutApp(const QObject *recvr, const char *slot, QObject *parent);

KRITAWIDGETUTILS_EXPORT QAction *aboutKDE(const QObject *recvr, const char *slot, QObject *parent);
}

#endif
