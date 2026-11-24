/*
   SPDX-FileCopyrightText: 2006 Boudewijn Rempt (boud@valdyas.org)
   SPDX-FileCopyrightText: 2007, 2009, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_DOCUMENTRESOURCEMANAGER_H
#define KO_DOCUMENTRESOURCEMANAGER_H

#include <QObject>

#include "kritaflake_export.h"

class KoShape;
class KUndo2Stack;
class KoShapeController;
class KoColor;
class KoUnit;

class QVariant;
class QSizeF;

class KRITAFLAKE_EXPORT KoDocumentResourceManager : public QObject
{
    Q_OBJECT

public:

enum DocumentResource {
    UndoStack,
    OdfDocument,
    GrabSensitivity,
    MarkerCollection,
    DocumentResolution,
    DocumentRectInPixels,

    KarbonStart = 1000,
    KexiStart = 2000,
    FlowStart = 3000,
    PlanStart = 4000,
    StageStart = 5000,
    KritaStart = 6000,
    SheetsStart = 7000,
    WordsStart = 8000,
    KoPageAppStart = 9000,
    KoTextStart = 10000
};

    explicit KoDocumentResourceManager(QObject *parent = 0);
    ~KoDocumentResourceManager() override;

    void setResource(int key, const QVariant &value);

    void setResource(int key, const KoColor &color);

    void setResource(int key, KoShape *shape);

    void setResource(int key, const KoUnit &unit);

    QVariant resource(int key) const;

    bool boolResource(int key) const;

    int intResource(int key) const;

    KoColor koColorResource(int key) const;

    KoShape *koShapeResource(int key) const;

    QString stringResource(int key) const;

    QSizeF sizeResource(int key) const;

    KoUnit unitResource(int key) const;

    bool hasResource(int key) const;

    void clearResource(int key);

    void setGrabSensitivity(int grabSensitivity);
    int grabSensitivity() const;

    KUndo2Stack *undoStack() const;
    void setUndoStack(KUndo2Stack *undoStack);

    qreal documentResolution() const;
    QRectF documentRectInPixels() const;

Q_SIGNALS:
    void resourceChanged(int key, const QVariant &value);

private:
    KoDocumentResourceManager(const KoDocumentResourceManager&);
    KoDocumentResourceManager& operator=(const KoDocumentResourceManager&);

    class Private;
    Private *const d;
};

#endif
