/*
    SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
    SPDX-FileCopyrightText: 1999 Simon Hausmann <hausmann@kde.org>
    SPDX-FileCopyrightText: 2000 Nicolas Hadacek <haadcek@kde.org>
    SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
    SPDX-FileCopyrightText: 2000 Michael Koch <koch@kde.org>
    SPDX-FileCopyrightText: 2001 Holger Freyther <freyther@kde.org>
    SPDX-FileCopyrightText: 2002 Ellis Whitehead <ellis@kde.org>
    SPDX-FileCopyrightText: 2002 Joseph Wenninger <jowenn@kde.org>
    SPDX-FileCopyrightText: 2003 Andras Mantia <amantia@kde.org>
    SPDX-FileCopyrightText: 2005-2006 Hamish Rodda <rodda@kde.org>
    SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

/*
 * The functions in this file was refactored out of `krecentfilesaction.cpp`.
 *
 * `krecentfilesaction.cpp` was forked from KConfigWidgets. Due to historical
 * reasons, it was licensed under LGPL 2.0 only (not LGPL 2.1 or later). Until
 * the original file has been relicensed, keep these code snippets in a
 * separate file.
 *
 * The relicensing issue is tracked at https://invent.kde.org/teams/licensing/issues/-/issues/34
 */

void KisRecentFilesManager::loadEntries(const KConfigGroup &_config)
{
    m_d->m_entries.clear();

    KConfigGroup cg = _config;
    if (cg.name().isEmpty()) {
        cg = KConfigGroup(cg.config(), "RecentFiles");
    }

    m_d->m_maxItems = cg.readEntry("maxRecentFileItems", 100);

    for (int i = 0; i < m_d->m_maxItems; ++i) {
        const QString key = QString("File%1").arg(i+1);
        QString value;
#ifdef Q_OS_ANDROID
        value = cg.readEntry(key, QString());
#else
        value = cg.readPathEntry(key, QString());
#endif
        if (value.isEmpty()) {
            continue;
        }
        QUrl url = QUrl::fromUserInput(value);

        if (url.isLocalFile()) {
            QString localFilePath = url.toLocalFile();
            QFileInfo fileInfo = QFileInfo(localFilePath);

            if (!fileInfo.exists()) {
                continue;
            }

            value = fileInfo.absoluteFilePath();
            url = QUrl::fromLocalFile(value);
        }

        if (m_d->containsUrl(url)) {
            continue;
        }

#ifdef Q_OS_WIN
        if (url.isLocalFile()) {
            value = QDir::toNativeSeparators(value);
        }
#endif

        const QString nameKey = QString("Name%1").arg(i+1);
        const QString nameValue = cg.readEntry(nameKey, url.fileName());
        m_d->m_entries.append(KisRecentFilesEntry {
            url,
            nameValue,
        });
    }

    Q_EMIT listRenewed();
}

void KisRecentFilesManager::saveEntries(const KConfigGroup &_cg)
{
    KConfigGroup cg = _cg;
    if (cg.name().isEmpty()) {
        cg = KConfigGroup(cg.config(), "RecentFiles");
    }

    cg.deleteGroup();

    cg.writeEntry("maxRecentFileItems", m_d->m_maxItems);

    for (int i = 0; i < m_d->m_entries.count(); ++i) {
        const QString key = QString("File%1").arg(i+1);
        QString value;
#ifdef Q_OS_ANDROID
        value = m_d->m_entries[i].m_url.toDisplayString();
        cg.writeEntry(key, value);
#else
        value = m_d->m_entries[i].m_url.toDisplayString(QUrl::PreferLocalFile);
        cg.writePathEntry(key, value);
#endif
        const QString nameKey = QString("Name%1").arg(i+1);
        const QString nameValue = m_d->m_entries[i].m_displayName;
        cg.writeEntry(nameKey, nameValue);
    }
}

void KisRecentFilesManager::add(const QUrl &url)
{
    const QString name;

    if (m_d->m_maxItems <= 0) {
        return;
    }

    if (url.isLocalFile() && url.toLocalFile().startsWith(QDir::tempPath())) {
        return;
    }
    const QString tmpName = name.isEmpty() ? url.fileName() : name;
    const QString pathOrUrl(url.toDisplayString(QUrl::PreferLocalFile));

#ifdef Q_OS_WIN
    const QString file = url.isLocalFile() ? QDir::toNativeSeparators(pathOrUrl) : pathOrUrl;
#else
    const QString file = pathOrUrl;
#endif

    {
        int removeIndex = m_d->indexOfUrl(url);
        if (removeIndex >= 0) {
            m_d->m_entries.removeAt(removeIndex);
            Q_EMIT fileRemoved(url);
        }
    }

    if (m_d->m_entries.count() >= m_d->m_maxItems) {
        m_d->m_entries.removeFirst();
    }

    m_d->m_entries.append(KisRecentFilesEntry {
        url,
        tmpName,
    });
    Q_EMIT fileAdded(url);
    m_d->requestSaveOnNextTick();
}
