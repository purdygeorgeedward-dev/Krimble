/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2023 Ivan Santa María <ghevan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KisMacosSecurityScopeBookmarkManager_h
#define KisMacosSecurityScopeBookmarkManager_h

#include <QObject>

class QString;
class QUrl;

class KisMacosSecurityBookmarkManager : public QObject
{
    Q_OBJECT

public:
    enum SecurityBookmarkType {
        File = 0,
        Directory
    };
    Q_ENUM(SecurityBookmarkType)

    static KisMacosSecurityBookmarkManager *instance();

    explicit KisMacosSecurityBookmarkManager();

    ~KisMacosSecurityBookmarkManager();
    bool parentDirHasPermissions(const QString &path);

    void createBookmarkFromPath(const QString &path, const QString &refpath,SecurityBookmarkType type = SecurityBookmarkType::File);

    void startAccessingSecurityScopedResources();

    void stopAccessingSecurityScopedResources();

    bool requestAccessToDir(const QString &path);

    void loadSecurityScopedResources();

    bool isSandboxed();

public Q_SLOTS:
    void addBookmarkAndCheckParentDir(const QUrl &url);

    void slotCreateBookmark(const QString &path);

private:
    Q_DISABLE_COPY(KisMacosSecurityBookmarkManager)

    void loadKeysFromArray(SecurityBookmarkType);

    QString securityBookmarkTypeToString(const SecurityBookmarkType);

    QUrl decodeBookmarkToURL(QString encodedPath);

    class Private;
    const QScopedPointer<Private> m_d;
};

#endif
