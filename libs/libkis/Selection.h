/*
 *  SPDX-FileCopyrightText: 2016 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef LIBKIS_SELECTION_H
#define LIBKIS_SELECTION_H

#include <QObject>

#include "kritalibkis_export.h"
#include "libkis.h"
#include <kis_types.h>

class KRITALIBKIS_EXPORT Selection : public QObject
{
    Q_OBJECT

public:

    Selection(KisSelectionSP selection, QObject *parent = 0);

    explicit Selection(QObject *parent = 0);
    ~Selection() override;

    bool operator==(const Selection &other) const;
    bool operator!=(const Selection &other) const;

public Q_SLOTS:

    Selection *duplicate() const;

    int width() const;

    int height() const;

    int x() const;

    int y() const;

    void move(int x, int y);

    void clear();

    void contract(int value);

    void copy(Node *node);

    void cut(Node *node);

    void paste(Node *destination, int x, int y);

    void erode();

    void dilate();

    void border(int xRadius, int yRadius);

    void feather(int radius);

    void grow(int xradius, int yradius);

    void shrink(int xRadius, int yRadius, bool edgeLock);

    void smooth();

    void invert();

    void resize(int w, int h);

    void select(int x, int y, int w, int h, int value);

    void selectAll(Node *node, int value);

    void replace(Selection *selection);

    void add(Selection *selection);

    void subtract(Selection *selection);

    void intersect(Selection *selection);

    void symmetricdifference(Selection *selection);

    QByteArray pixelData(int x, int y, int w, int h) const;

    void setPixelData(QByteArray value, int x, int y, int w, int h);

private:
    friend class Document;
    friend class FilterLayer;
    friend class FillLayer;
    friend class SelectionMask;
    friend class TransparencyMask;
    friend class FilterMask;

    KisSelectionSP selection() const;

    struct Private;
    Private *const d;

};

#endif
