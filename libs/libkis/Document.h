/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_DOCUMENT_H
#define LIBKIS_DOCUMENT_H

#include <QFileInfo>

#include "kritalibkis_export.h"
#include "libkis.h"

#include "GroupLayer.h"
#include "CloneLayer.h"
#include "FileLayer.h"
#include "FilterLayer.h"
#include "FillLayer.h"
#include "VectorLayer.h"
#include "FilterMask.h"
#include "SelectionMask.h"
#include "TransparencyMask.h"
#include "TransformMask.h"
#include "ColorizeMask.h"

#include "GuidesConfig.h"
#include "GridConfig.h"

class KisDocument;

class KRITALIBKIS_EXPORT Document : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Document)

public:
    explicit Document(KisDocument *document, bool ownsDocument, QObject *parent = 0);
    ~Document() override;

    bool operator==(const Document &other) const;
    bool operator!=(const Document &other) const;

public Q_SLOTS:
    Q_DECL_DEPRECATED QList<qreal> horizontalGuides() const;
    Q_DECL_DEPRECATED QList<qreal> verticalGuides() const;

    Q_DECL_DEPRECATED bool guidesVisible() const;
    Q_DECL_DEPRECATED bool guidesLocked() const;

    Document *clone() const;

    bool batchmode() const;

    void setBatchmode(bool value);

    Node* activeNode() const;

    void setActiveNode(Node* value);

    QList<Node*> topLevelNodes() const;

    Node *nodeByName(const QString &name) const;

    Node *nodeByUniqueID(const QUuid &id) const;

    QString colorDepth() const;

    QString colorModel() const;

    QString colorProfile() const;

    bool setColorProfile(const QString &colorProfile);

    bool setColorSpace(const QString &colorModel, const QString &colorDepth, const QString &colorProfile);

    QColor backgroundColor();

    bool setBackgroundColor(const QColor &color);

    QString documentInfo() const;

    void setDocumentInfo(const QString &document);

    QString fileName() const;

    void setFileName(QString value);

    int height() const;

    void setHeight(int value);

    QString name() const;

    void setName(QString value);

    int resolution() const;
    void setResolution(int value);

    Node* rootNode() const;

    Selection* selection() const;

    void setSelection(Selection* value);

    int width() const;

    void setWidth(int value);

    int xOffset() const;

    void setXOffset(int x);

    int yOffset() const;

    void setYOffset(int y);

    double xRes() const;

    void setXRes(double xRes) const;

    double yRes() const;

    void setYRes(double yRes) const;

    QByteArray pixelData(int x, int y, int w, int h) const;

    bool close();

    void crop(int x, int y, int w, int h);

    bool exportImage(const QString &filename, const InfoObject &exportConfiguration);

    void flatten();

    void resizeImage(int x, int y, int w, int h);

   void scaleImage(int w, int h, int xres, int yres, QString strategy);

   void rotateImage(double radians);

   void shearImage(double angleX, double angleY);

    bool save();

    bool saveAs(const QString &filename);

    Node* createNode(const QString &name, const QString &nodeType);
    GroupLayer* createGroupLayer(const QString &name);
    FileLayer* createFileLayer(const QString &name, const QString fileName, const QString scalingMethod, const QString scalingFilter = "Bicubic");

    FilterLayer* createFilterLayer(const QString &name, Filter &filter, Selection &selection);

    FillLayer* createFillLayer(const QString &name, const QString generatorName, InfoObject &configuration, Selection &selection);

    CloneLayer* createCloneLayer(const QString &name, const Node* source);

    VectorLayer* createVectorLayer(const QString &name);

    FilterMask* createFilterMask(const QString &name, Filter &filter, Selection &selection);

    FilterMask* createFilterMask(const QString &name, Filter &filter, const Node* selection_source);

    SelectionMask* createSelectionMask(const QString &name);

    TransparencyMask* createTransparencyMask(const QString &name);

    TransformMask* createTransformMask(const QString &name);

    ColorizeMask* createColorizeMask(const QString &name);

    QImage projection(int x = 0, int y = 0, int w = 0, int h = 0) const;

    QImage thumbnail(int w, int h) const;

    void lock();

    void unlock();

    void waitForDone();

    bool tryBarrierLock();

    void refreshProjection();

    Q_DECL_DEPRECATED void setHorizontalGuides(const QList<qreal> &lines);
    Q_DECL_DEPRECATED void setVerticalGuides(const QList<qreal> &lines);

    Q_DECL_DEPRECATED void setGuidesVisible(bool visible);

    Q_DECL_DEPRECATED void setGuidesLocked(bool locked);

    bool modified() const;

    void setModified(bool modified);

    QRect bounds() const;

    bool importAnimation(const QList<QString> &files, int firstFrame, int step);

    int framesPerSecond();

    void setFramesPerSecond(int fps);

    void setFullClipRangeStartTime(int startTime);

    int fullClipRangeStartTime();

    void setFullClipRangeEndTime(int endTime);

    int fullClipRangeEndTime();

    int animationLength();

    void setPlayBackRange(int start, int stop);

    int playBackStartTime();

    int playBackEndTime();

    int currentTime();

    void setCurrentTime(int time);

    QStringList annotationTypes() const;

    QString annotationDescription(const QString &type) const;

    QByteArray annotation(const QString &type);

    void setAnnotation(const QString &type, const QString &description, const QByteArray &annotation);

    void removeAnnotation(const QString &type);

    void setAutosave(bool active);

    bool autosave();

    GuidesConfig *guidesConfig();

    void setGuidesConfig(GuidesConfig *guidesConfig);

    GridConfig *gridConfig();

    void setGridConfig(GridConfig *gridConfig);

    qreal audioLevel() const;

    void setAudioLevel(const qreal level);

    QList<QString> audioTracks() const;

    bool setAudioTracks(const QList<QString> files) const;

private:

    friend class Krita;
    friend class Window;
    friend class Filter;
    friend class View;
    friend class VectorLayer;
    friend class Shape;
    QPointer<KisDocument> document() const;
    void setOwnsDocument(bool ownsDocument);

private:
    struct Private;
    Private *const d;

};

#endif
