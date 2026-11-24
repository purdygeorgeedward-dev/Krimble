/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_BASE_NODE_H
#define _KIS_BASE_NODE_H

#include <QObject>
#include <QIcon>
#include <QUuid>
#include <QString>

#include <KoID.h>

#include "kis_shared.h"
#include "kis_paint_device.h"
#include "kis_processing_visitor.h"

class KoProperties;
class KoColorSpace;
class KoCompositeOp;
class KisNodeVisitor;
class KisUndoAdapter;
class KisKeyframeChannel;

#include "kritaimage_export.h"

class KRITAIMAGE_EXPORT KisBaseNode : public QObject, public KisShared
{

    Q_OBJECT

public:
    struct Property
    {
        QString id;

        QString name;

        bool isMutable {false};

        QIcon onIcon;
        QIcon offIcon;

        QVariant state;

        bool canHaveStasis {false};

        bool isInStasis {false};

        bool stateInStasis {false};

        bool operator==(const Property &rhs) const {
            return rhs.name == name && rhs.state == state && isInStasis == rhs.isInStasis;
        }

        Property(): isMutable( false ), isInStasis(false) { }

        Property( const KoID &n, const QIcon &on, const QIcon &off, bool isOn )
                : id(n.id()), name( n.name() ), isMutable( true ), onIcon( on ), offIcon( off ), state( isOn ),
                  canHaveStasis( false ), isInStasis(false) { }

        Property( const KoID &n, const QIcon &on, const QIcon &off, bool isOn,
                  bool _isInStasis, bool _stateInStasis = false )
                : id(n.id()), name(n.name()), isMutable( true ), onIcon( on ), offIcon( off ), state( isOn ),
                  canHaveStasis( true ), isInStasis( _isInStasis ), stateInStasis( _stateInStasis ) { }

        Property( const KoID &n, const QString &s )
                : id(n.id()), name(n.name()), isMutable( false ), state( s ), isInStasis(false) { }
    };

    typedef QList<Property> PropertyList;

public:

    KisBaseNode(KisImageWSP image);

    KisBaseNode(const KisBaseNode & rhs);

    ~KisBaseNode() override;

    virtual KisPaintDeviceSP paintDevice() const = 0;

    virtual KisPaintDeviceSP original() const = 0;

    virtual KisPaintDeviceSP projection() const = 0;

    virtual KisPaintDeviceSP colorSampleSourceDevice() const;

    virtual const KoColorSpace *colorSpace() const = 0;

    quint8 opacity() const;

    void setOpacity(quint8 val);

    quint8 percentOpacity() const;

    void setPercentOpacity(quint8 val);

    virtual const KoCompositeOp *compositeOp() const = 0;
    const QString& compositeOpId() const;

    void setCompositeOpId(const QString& compositeOpId);

    QUuid uuid() const;

    void setUuid(const QUuid& id);

    QString name() const {
        return objectName();
    }

    void setName(const QString& name) {
        setObjectName(name);
        baseNodeChangedCallback();
    }

    virtual QIcon icon() const {
        return QIcon();
    }

    virtual PropertyList sectionModelProperties() const;

    virtual void setSectionModelProperties(const PropertyList &properties);

    const KoProperties & nodeProperties() const;

    void setNodeProperty(const QString & name, const QVariant & value);

    void mergeNodeProperties(const KoProperties & properties);

    bool check(const KoProperties & properties) const;

    virtual bool accept(KisNodeVisitor &) {
        return false;
    }

    virtual void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) {
        Q_UNUSED(visitor);
        Q_UNUSED(undoAdapter);
    }

    virtual QImage createThumbnail(qint32 w, qint32 h, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio, KisThumbnailBoundsMode boundsMode = KisThumbnailBoundsMode::Precise);

    QImage createPreferredThumbnail(qint32 w, qint32 h, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio);

    KisThumbnailBoundsMode preferredThumbnailBoundsMode() const;

    void setPreferredThumbnailBoundsMode(KisThumbnailBoundsMode value) const;

    virtual int thumbnailSeqNo() const;

    virtual QImage createThumbnailForFrame(qint32 w, qint32 h, int time, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio, KisThumbnailBoundsMode boundsMode = KisThumbnailBoundsMode::Precise);
    QImage createPreferredThumbnailForFrame(qint32 w, qint32 h, int time, Qt::AspectRatioMode aspectRatioMode = Qt::IgnoreAspectRatio);

    virtual void updateSettings() {
    }

    virtual bool visible(bool recursive = false) const;

    virtual void setVisible(bool visible, bool loading = false);

    bool userLocked() const;

    bool belongsToIsolatedGroup() const;

    bool isIsolatedRoot() const;

    virtual void setUserLocked(bool l);

    bool isEditable(bool checkVisibility = true) const;

    bool hasEditablePaintDevice() const;

    virtual qint32 x() const {
        return 0;
    }

    virtual void setX(qint32) {
    }

    virtual qint32 y() const {
        return 0;
    }

    virtual void setY(qint32) {
    }

    virtual QRect extent() const {
        return QRect();
    }

    virtual QRect exactBounds() const {
        return QRect();
    }

    void setCollapsed(bool collapsed);

    bool collapsed() const;

    void setColorLabelIndex(int index);

    int colorLabelIndex() const;

    bool supportsLodMoves() const;

    virtual bool supportsLodPainting() const;

    QMap<QString, KisKeyframeChannel*> keyframeChannels() const;

    KisKeyframeChannel *getKeyframeChannel(const QString &id, bool create);
    KisKeyframeChannel *getKeyframeChannel(const QString &id) const;

    bool isPinnedToTimeline() const;

    void setPinnedToTimeline(bool pinned);

    bool isAnimated() const;
    void enableAnimation();

    virtual void setImage(KisImageWSP image);
    KisImageWSP image() const;

    virtual bool isFakeNode() const;

protected:

    void setSupportsLodMoves(bool value);

    virtual KisBaseNodeSP parentCallback() const {
        return KisBaseNodeSP();
    }

    virtual void notifyParentVisibilityChanged(bool value) {
        Q_UNUSED(value);
    }

    virtual void baseNodeChangedCallback() {
    }

    virtual void baseNodeCollapsedChangedCallback() {
    }

    virtual void baseNodeInvalidateAllFramesCallback() {
    }

    virtual void addKeyframeChannel(KisKeyframeChannel* channel);

    virtual KisKeyframeChannel * requestKeyframeChannel(const QString &id);

public:
    virtual bool supportsKeyframeChannel(const QString &id);

Q_SIGNALS:
    void keyframeChannelAdded(KisKeyframeChannel *channel);
    void opacityChanged(quint8 value);

private:

    struct Private;
    Private * const m_d;

};

KRITAIMAGE_EXPORT QDebug operator<<(QDebug dbg, const KisBaseNode::Property &prop);

Q_DECLARE_METATYPE( KisBaseNode::PropertyList )

#endif
