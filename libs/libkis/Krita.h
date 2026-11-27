/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_KRITA_H
#define LIBKIS_KRITA_H

#include <QObject>
#include <QAction>

#include "kritalibkis_export.h"
#include "libkis.h"

#include "Extension.h"
#include "Document.h"
#include "Window.h"
#include "View.h"
#include "Notifier.h"

class KRITALIBKIS_EXPORT Krita : public QObject
{
    Q_OBJECT

public:
    explicit Krita(QObject *parent = 0);
    ~Krita() override;

public Q_SLOTS:

    Document* activeDocument() const;

    void setActiveDocument(Document* value);

    bool batchmode() const;

    void setBatchmode(bool value);

    QList<QAction*> actions() const;

    QAction *action(const QString &name) const;

    QList<Document*> documents() const;

    QList<QDockWidget*> dockers() const;

    QStringList filters() const;

    Filter *filter(const QString &name) const;

    QStringList colorModels() const;

    QStringList colorDepths(const QString &colorModel) const;

    QStringList filterStrategies() const;

    QStringList profiles(const QString &colorModel, const QString &colorDepth) const;

    bool addProfile(const QString &profilePath);

    Notifier *notifier() const;

    QString version() const;

    QList<View*> views() const;

    Window *activeWindow() const;

    QList<Window *> windows() const;

    QMap<QString, Resource*> resources(QString &type) const;

    QStringList recentDocuments() const;

    Document *createDocument(int width, int height, const QString &name, const QString &colorModel, const QString &colorDepth, const QString &profile, double resolution);

    Document *openDocument(const QString &filename);

    Window *openWindow();

    void addExtension(Extension* extension);

    QList<Extension*> extensions();

    void addDockWidgetFactory(DockWidgetFactoryBase* factory );

    void writeSetting(const QString &group, const QString &name, const QString &value);

    QString readSetting(const QString &group, const QString &name, const QString &defaultValue);

    QIcon icon(QString &iconName) const;

    static Krita* instance();

    static QObject *fromVariant(const QVariant& v);

    static QString krita_i18n(const QString &text);
    static QString krita_i18nc(const QString &context, const QString &text);

    static QString getAppDataLocation();

private Q_SLOTS:

    void mainWindowIsBeingCreated(KisMainWindow *window);

private:
    struct Private;
    Private *const d;
    static Krita* s_instance;

};

Q_DECLARE_METATYPE(Notifier*);

#endif
