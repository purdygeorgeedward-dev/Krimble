/*
 *  SPDX-FileCopyrightText: 2020 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOACTIVECANVASRESOURCEDEPENDENCY_H
#define KOACTIVECANVASRESOURCEDEPENDENCY_H

#include <QScopedPointer>
#include <QSharedPointer>
#include "kritaflake_export.h"

class KRITAFLAKE_EXPORT KoActiveCanvasResourceDependency
{
public:
    KoActiveCanvasResourceDependency(int sourceKey, int targetKey);
    virtual ~KoActiveCanvasResourceDependency();

    int sourceKey() const;

    int targetKey() const;

    virtual bool shouldUpdateSource(QVariant &source, const QVariant &target) = 0;

private:
    struct Private;
    const QScopedPointer<Private> m_d;
};

typedef QSharedPointer<KoActiveCanvasResourceDependency> KoActiveCanvasResourceDependencySP;

#endif
