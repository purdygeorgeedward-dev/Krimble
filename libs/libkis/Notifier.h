/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_NOTIFIER_H
#define LIBKIS_NOTIFIER_H

#include <QObject>

#include "kritalibkis_export.h"
#include <kis_types.h>
#include "libkis.h"
#include <KisView.h>

class KisMainWindow;
class KisDocument;

class KRITALIBKIS_EXPORT Notifier : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Notifier)

    Q_PROPERTY(bool Active READ active WRITE setActive)

public:
    explicit Notifier(QObject *parent = 0);
    ~Notifier() override;

    bool active() const;

    void setActive(bool value);

Q_SIGNALS:

    void applicationClosing();

    void imageCreated(Document *image);

    void imageSaved(const QString &filename);

    void imageClosed(const QString &filename);

    void viewCreated(View *view);

    void viewClosed(View *view);

    void windowIsBeingCreated(Window *window);

    void windowCreated();

    void configurationChanged();

private Q_SLOTS:

    void imageCreated(KisDocument *document);

    void viewCreated(KisView *view);
    void viewClosed(KisView *view);

    void windowIsBeingCreated(KisMainWindow *window);

private:
    struct Private;
    Private *const d;

};

#endif
