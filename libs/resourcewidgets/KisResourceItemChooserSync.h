/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2014 Sven Langkamp <sven.langkamp@gmail.com>
   SPDX-FileCopyrightText: 2019 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KISRESOURCEITEMCHOOSERSYNC_H
#define KISRESOURCEITEMCHOOSERSYNC_H

#include <QObject>
#include <QScopedPointer>

#include "kritaresourcewidgets_export.h"

class KRITARESOURCEWIDGETS_EXPORT KisResourceItemChooserSync : public QObject
{
    Q_OBJECT
public:
    KisResourceItemChooserSync();
    ~KisResourceItemChooserSync() override;
    static KisResourceItemChooserSync* instance();

    int baseLength();

    void setBaseLength(int length);

Q_SIGNALS:
    void baseLengthChanged(int length);

private:

    KisResourceItemChooserSync(const KisResourceItemChooserSync&);
    KisResourceItemChooserSync operator=(const KisResourceItemChooserSync&);

private:
    struct Private;
    const QScopedPointer<Private> d;
};

#endif
