/*
 *  SPDX-FileCopyrightText: 2017 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_COLORIZEMASK_H
#define LIBKIS_COLORIZEMASK_H

#include <QObject>
#include "Node.h"
#include "ManagedColor.h"

#include <kis_types.h>

#include "kritalibkis_export.h"
#include "libkis.h"

class KRITALIBKIS_EXPORT ColorizeMask : public Node
{
    Q_OBJECT
    Q_DISABLE_COPY(ColorizeMask)

public:
    explicit ColorizeMask(KisImageSP image, QString name, QObject *parent = 0);
    explicit ColorizeMask(KisImageSP image, KisColorizeMaskSP mask, QObject *parent = 0);
    ~ColorizeMask() override;
public Q_SLOTS:

    virtual QString type() const override;

    QList<ManagedColor*> keyStrokesColors() const;

    void initializeKeyStrokeColors(QList<ManagedColor*> colors, int transparentIndex = -1);

    void removeKeyStroke(ManagedColor* color);

    int transparencyIndex() const;

    QByteArray keyStrokePixelData(ManagedColor* color, int x, int y, int w, int h) const;

    bool setKeyStrokePixelData(QByteArray value, ManagedColor* color, int x, int y, int w, int h);

    void setUseEdgeDetection(bool value);

    bool useEdgeDetection() const;

    void setEdgeDetectionSize(qreal value);

    qreal edgeDetectionSize() const;

    void setCleanUpAmount(qreal value);

    qreal cleanUpAmount() const;

    void setLimitToDeviceBounds(bool value);

    bool limitToDeviceBounds() const;

    void updateMask(bool force = false);

    void resetCache();

    bool showOutput() const;

    void setShowOutput(bool enabled);

    bool editKeyStrokes() const;

    void setEditKeyStrokes(bool enabled);

};

#endif

