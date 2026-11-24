/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2023 Ivan Santa María <ghevan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KisMacosEntitlements_h
#define KisMacosEntitlements_h

#include <QObject>

class KisMacosEntitlements : public QObject
{
    Q_OBJECT

public:

    enum Entitlements {
        Sandbox = 0,
        BookmarkScopeApp,
        BookmarkScopeDocument,
        Other = 99
    };

    KisMacosEntitlements();

    ~KisMacosEntitlements();

    void loadAvailableEntitlements(void);

    bool hasEntitlement(Entitlements);

    bool sandbox();

private:

    class Private;
    const QScopedPointer<Private> m_d;
};

#endif
