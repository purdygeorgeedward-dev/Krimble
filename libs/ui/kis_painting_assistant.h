/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *  SPDX-FileCopyrightText: 2017 Scott Petrovic <scottpetrovic@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_PAINTING_ASSISTANT_H_
#define _KIS_PAINTING_ASSISTANT_H_

#include <QString>
#include <QPointF>
#include <QRect>
#include <QFile>
#include <QObject>
#include <QColor>
#include <QXmlStreamWriter>
#include <QMap>

#include <kritaui_export.h>
#include <kis_shared.h>
#include <kis_types.h>

class QPainter;
class QRect;
class QRectF;
class KoStore;
class KisCoordinatesConverter;
class KisCanvas2;
class QDomDocument;
class QDomElement;
class KoColorDisplayRendererInterface;

#include <kis_shared_ptr.h>
#include <KoGenericRegistry.h>

class KisPaintingAssistantHandle;
typedef KisSharedPtr<KisPaintingAssistantHandle> KisPaintingAssistantHandleSP;
class KisPaintingAssistant;
class QPainterPath;

enum HandleType {
    NORMAL,
    SIDE,
    CORNER,
    VANISHING_POINT,
    ANCHOR
};

class KRITAUI_EXPORT KisPaintingAssistantHandle : public QPointF, public KisShared
{
    friend class KisPaintingAssistant;

public:
    KisPaintingAssistantHandle(double x, double y);
    explicit KisPaintingAssistantHandle(QPointF p);
    KisPaintingAssistantHandle(const KisPaintingAssistantHandle&);
    ~KisPaintingAssistantHandle();
    void mergeWith(KisPaintingAssistantHandleSP);
    void uncache();
    KisPaintingAssistantHandle& operator=(const QPointF&);
    void setType(char type);
    char handleType() const;

    KisPaintingAssistant* chiefAssistant() const;

private:
    void registerAssistant(KisPaintingAssistant*);
    void unregisterAssistant(KisPaintingAssistant*);
    bool containsAssistant(KisPaintingAssistant*) const;

private:
    struct Private;
    Private* const d;
};

class KRITAUI_EXPORT KisPaintingAssistant
{
public:
    KisPaintingAssistant(const QString& id, const QString& name);
    virtual ~KisPaintingAssistant();
    virtual KisPaintingAssistantSP clone(QMap<KisPaintingAssistantHandleSP, KisPaintingAssistantHandleSP> &handleMap) const = 0;
    const QString& id() const;
    const QString& name() const;
    bool isSnappingActive() const;
    void setSnappingActive(bool set);
    void copySharedData(KisPaintingAssistantSP assistant);

    virtual QPointF adjustPosition(const QPointF& point, const QPointF& strokeBegin, bool snapToAny, qreal moveThresholdPt) = 0;
    virtual void adjustLine(QPointF& point, QPointF& strokeBegin) = 0;
    virtual void endStroke();
    virtual void setAdjustedBrushPosition(const QPointF position);
    virtual void setFollowBrushPosition(bool follow);
    virtual QPointF getDefaultEditorPosition() const = 0;
    virtual QPointF getEditorPosition() const;
    virtual int numHandles() const = 0;

    virtual bool canBeLocal() const;
    bool isLocal() const;
    void setLocal(bool value);

    bool isLocked();
    void setLocked(bool value);
    bool isDuplicating();
    void setDuplicating(bool value);

    QPointF editorWidgetOffset();
    void setEditorWidgetOffset(QPointF offset);

    void replaceHandle(KisPaintingAssistantHandleSP _handle, KisPaintingAssistantHandleSP _with);
    void addHandle(KisPaintingAssistantHandleSP handle, HandleType type);

    QPointF viewportConstrainedEditorPosition(const KisCoordinatesConverter* converter, const QSize editorSize);

    QColor effectiveAssistantColor() const;
    bool useCustomColor();
    void setUseCustomColor(bool useCustomColor);
    void setAssistantCustomColor(QColor color);
    QColor assistantCustomColor();
    void setAssistantGlobalColorCache(const QColor &color);

    virtual void drawAssistant(QPainter& gc, const QRectF& updateRect, const KisCoordinatesConverter *converter, const KoColorDisplayRendererInterface *displayRenderInterface, bool cached, KisCanvas2 *canvas=0, bool assistantVisible=true, bool previewVisible=true);
    void uncache();
    const QList<KisPaintingAssistantHandleSP>& handles() const;
    QList<KisPaintingAssistantHandleSP> handles();
    const QList<KisPaintingAssistantHandleSP>& sideHandles() const;
    QList<KisPaintingAssistantHandleSP> sideHandles();

    QByteArray saveXml( QMap<KisPaintingAssistantHandleSP, int> &handleMap);
    virtual void saveCustomXml(QXmlStreamWriter* xml);

    void loadXml(KoStore *store, QMap<int, KisPaintingAssistantHandleSP> &handleMap, QString path);
    virtual bool loadCustomXml(QXmlStreamReader* xml);

    void saveXmlList(QDomDocument& doc, QDomElement& assistantsElement, int count);
    void findPerspectiveAssistantHandleLocation();
    KisPaintingAssistantHandleSP oppHandleOne();

    const KisPaintingAssistantHandleSP topLeft() const;
    KisPaintingAssistantHandleSP topLeft();
    const KisPaintingAssistantHandleSP topRight() const;
    KisPaintingAssistantHandleSP topRight();
    const KisPaintingAssistantHandleSP bottomLeft() const;
    KisPaintingAssistantHandleSP bottomLeft();
    const KisPaintingAssistantHandleSP bottomRight() const;
    KisPaintingAssistantHandleSP bottomRight();
    const KisPaintingAssistantHandleSP topMiddle() const;
    KisPaintingAssistantHandleSP topMiddle();
    const KisPaintingAssistantHandleSP rightMiddle() const;
    KisPaintingAssistantHandleSP rightMiddle();
    const KisPaintingAssistantHandleSP leftMiddle() const;
    KisPaintingAssistantHandleSP leftMiddle();
    const KisPaintingAssistantHandleSP bottomMiddle() const;
    KisPaintingAssistantHandleSP bottomMiddle();

    KisPaintingAssistantHandleSP closestCornerHandleFromPoint(QPointF point);

    bool areTwoPointsClose(const QPointF& pointOne, const QPointF& pointTwo);

    virtual bool isAssistantComplete() const;

    virtual void transform(const QTransform &transform);

public:
    void drawPath(QPainter& painter, const QPainterPath& path, const KoColorDisplayRendererInterface *displayRenderInterface, bool drawActive=true);
    void drawPreview(QPainter& painter, const QPainterPath& path, const KoColorDisplayRendererInterface *displayRenderInterface);
    void drawError(QPainter& painter, const QPainterPath& path, const KoColorDisplayRendererInterface *displayRenderInterface);
    void drawX(QPainter& painter, const QPointF& pt, const KoColorDisplayRendererInterface *displayRenderInterface);
    static double norm2(const QPointF& p);

    void setDecorationThickness(int thickness);

protected:
    explicit KisPaintingAssistant(const KisPaintingAssistant &rhs, QMap<KisPaintingAssistantHandleSP, KisPaintingAssistantHandleSP> &handleMap);

    virtual QRect boundingRect() const;

    virtual void drawCache(QPainter& gc, const KisCoordinatesConverter *converter, const KoColorDisplayRendererInterface *displayRenderInterface, bool assistantVisible=true) = 0;

    void initHandles(QList<KisPaintingAssistantHandleSP> _handles);
    QList<KisPaintingAssistantHandleSP> m_handles;

    QPointF pixelToView(const QPoint pixelCoords) const;
    QPointF effectiveBrushPosition(const KisCoordinatesConverter *converter, KisCanvas2 *canvas) const;

    virtual KisPaintingAssistantHandleSP firstLocalHandle() const;
    virtual KisPaintingAssistantHandleSP secondLocalHandle() const;
    QRectF getLocalRect() const;

public:
    static QList<KisPaintingAssistantSP> cloneAssistantList(const QList<KisPaintingAssistantSP> &list);

protected:
    bool m_hasBeenInsideLocalRect {false};

private:
    struct Private;
    Private* const d;

};

class KRITAUI_EXPORT KisPaintingAssistantFactory
{
public:
    KisPaintingAssistantFactory();
    virtual ~KisPaintingAssistantFactory();
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual KisPaintingAssistant* createPaintingAssistant() const = 0;

};

class KRITAUI_EXPORT KisPaintingAssistantFactoryRegistry : public KoGenericRegistry<KisPaintingAssistantFactory*>
{
  public:
    KisPaintingAssistantFactoryRegistry();
    ~KisPaintingAssistantFactoryRegistry() override;

    static KisPaintingAssistantFactoryRegistry* instance();

};

#endif
