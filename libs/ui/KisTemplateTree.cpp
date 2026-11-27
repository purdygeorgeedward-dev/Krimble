/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KisTemplateTree.h"

#include <QDir>
#include <QPrinter>
#include <QUrl>

#include <kdesktopfile.h>
#include <kconfig.h>
#include <kis_debug.h>

#include <KoResourcePaths.h>
#include <klocalizedstring.h>
#include <kconfiggroup.h>

#include <KisTemplate.h>
#include <KisTemplateGroup.h>
#include <KisTemplates.h>

static QString currentLocale()
{
    const QStringList languages = KLocalizedString::languages();
    if (languages.isEmpty()) {
        return QLocale().name();
    } else {
        return languages.first();
    }
}

KisTemplateTree::KisTemplateTree(const QString &templatesResourcePath,
                                 bool readTree)
    :  m_templatesResourcePath(templatesResourcePath)
    , m_defaultGroup(0)
    , m_defaultTemplate(0)
{
    if (readTree)
        readTemplateTree();
}

KisTemplateTree::~KisTemplateTree()
{
    qDeleteAll(m_groups);
}

void KisTemplateTree::readTemplateTree()
{
    readGroups();
    readTemplates();
}

void KisTemplateTree::writeTemplateTree()
{
    QString localDir = KoResourcePaths::saveLocation("templates");

    Q_FOREACH (KisTemplateGroup *group, m_groups) {

        bool touched = false;
        QList<KisTemplate*> templates = group->templates();
        QList<KisTemplate*>::iterator it = templates.begin();
        for (; it != templates.end() && !touched && !group->touched(); ++it)
            touched = (*it)->touched();

        if (group->touched() || touched) {
            if (!group->isHidden()) {
                QDir path;
                path.mkpath(localDir + group->name());
            } else {
                if (group->dirs().count() == 1 && group->dirs().contains(localDir)) {
                    QFile f(group->dirs().first());
                    f.remove();
                } else {
                    QDir path;
                    path.mkpath(localDir + group->name());
                }
            }
        }
        Q_FOREACH (KisTemplate *t, templates) {
            if (t->touched()) {
                writeTemplate(t, group, localDir);
            }
            if (t->isHidden() && t->touched()) {
                writeTemplate(t, group, localDir);
                QFile::remove(t->file());
                QFile::remove(t->picture());
            }
        }
    }
}

bool KisTemplateTree::add(KisTemplateGroup *g)
{

    KisTemplateGroup *group = find(g->name());
    if (group == 0) {
        m_groups.append(g);
        return true;
    }

    group->addDir(g->dirs().first());
    delete g;
    g = 0;
    return false;
}

KisTemplateGroup *KisTemplateTree::find(const QString &name) const
{
    QList<KisTemplateGroup*>::const_iterator it = m_groups.begin();
    KisTemplateGroup* ret = 0;

    while (it != m_groups.end()) {
        if ((*it)->name() == name) {
            ret = *it;
            break;
        }

        ++it;
    }

    return ret;
}

void KisTemplateTree::readGroups()
{

    QStringList dirs = KoResourcePaths::findDirs("templates");

    Q_FOREACH (const QString & dirName, dirs) {
        if (!dirName.contains("templates")) continue;
        QDir dir(dirName);
        if (!dir.exists())
            continue;
        QStringList templateDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        Q_FOREACH (const QString & templateDirName, templateDirs) {
            QDir templateDir(dirName + "/" + templateDirName);
            QString name = templateDirName;
            QString defaultTab;
            int sortingWeight = 1000;
            if (templateDir.exists(".directory")) {
                KDesktopFile config(templateDir.absoluteFilePath(".directory"));
                config.setLocale(currentLocale());
                KConfigGroup dg = config.desktopGroup();
                name = dg.readEntry("Name");
                defaultTab = dg.readEntry("X-KDE-DefaultTab");
                sortingWeight = dg.readEntry("X-KDE-SortingWeight", 1000);
            }
            KisTemplateGroup *g = new KisTemplateGroup(name, templateDir.absolutePath() + '/', sortingWeight);
            if (add(g)) {
                if (defaultTab == "true") {
                    m_defaultGroup = g;
                }
            }
        }
    }
}

void KisTemplateTree::readTemplates()
{
    QString dontShow = "imperial";
    if ( QLocale().measurementSystem() == QLocale::ImperialSystem) {
        dontShow = "metric";
    }

    Q_FOREACH (KisTemplateGroup* group, m_groups) {
        QStringList dirs = group->dirs();
        for (QStringList::ConstIterator it = dirs.constBegin(); it != dirs.constEnd(); ++it) {
            QDir d(*it);
            if (!d.exists())
                continue;
            QStringList files = d.entryList(QDir::Files | QDir::Readable, QDir::Name);
            for (int i = 0; i < files.count(); ++i) {
                QString filePath = *it + files[i];
                QString icon;
                QString text;
                QString description;
                QString hidden_str;
                QString fileName;
                bool hidden = false;
                bool defaultTemplate = false;
                QString templatePath;
                QString measureSystem;
                if (KDesktopFile::isDesktopFile(filePath)) {
                    KConfig _config(filePath, KConfig::SimpleConfig);
                    _config.setLocale(currentLocale());
                    KConfigGroup config(&_config, "Desktop Entry");
                    if (config.readEntry("Type") == "Link") {
                        text = config.readEntry("Name");
                        fileName = filePath;
                        description = config.readEntry("Comment");
                        icon = config.readEntry("Icon");
                        if (icon[0] != '/' &&
                                QFile::exists(*it + icon))
                            icon = *it + icon;
                        hidden = config.readEntry("X-KDE-Hidden", false);
                        defaultTemplate = config.readEntry("X-KDE-DefaultTemplate", false);
                        measureSystem = config.readEntry("X-KDE-MeasureSystem").toLower();

                        if (measureSystem == dontShow)
                            continue;

                        templatePath = config.readPathEntry("URL", QString());
                        if (templatePath[0] != '/') {
                            if (templatePath.left(6) == "file:/")
                                templatePath = templatePath.right(templatePath.length() - 6);
                            templatePath = *it + templatePath;
                        }
                    } else
                        continue;
                }
                else if (files[i].right(4) != ".png")
                    continue;
                else {
                    icon = filePath;
                    QFileInfo fi(filePath);
                    text = fi.completeBaseName();
                    templatePath = filePath;
                }
                KisTemplate *t = new KisTemplate(text, description, templatePath, icon, fileName,
                                               measureSystem, hidden);
                group->add(t, false, false);
                if (defaultTemplate)
                    m_defaultTemplate = t;
            }
        }
    }
}

void KisTemplateTree::writeTemplate(KisTemplate *t, KisTemplateGroup *group,
                                   const QString &localDir)
{
    QString fileName;
    if (t->isHidden()) {
        fileName = t->fileName();
        if (QFile::remove(fileName) || !QFile::exists(fileName)) {
            QFile::remove(t->name());
            QFile::remove(t->picture());
            return;
        }
    }
    QString const path = localDir + group->name() + '/';
    QString const name = KisTemplates::trimmed(t->name());
    fileName = path + name + ".desktop";
    if (t->isHidden() && QFile::exists(fileName))
        return;
    QString fill;
    while (QFile(fileName).exists()) {
        fill += '_';
        fileName = path + fill + name + ".desktop";
    }

    KConfig _config(fileName, KConfig::SimpleConfig);
    _config.setLocale(currentLocale());
    KConfigGroup config(&_config, "Desktop Entry");
    config.writeEntry("Type", "Link");
    config.writePathEntry("URL", t->file());
    config.writeEntry("Name", t->name());
    config.writeEntry("Icon", t->picture());
    config.writeEntry("X-KDE-Hidden", t->isHidden());
}
