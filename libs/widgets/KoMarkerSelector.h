/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOMARKERSELECTOR_H
#define KOMARKERSELECTOR_H

#include "kritawidgets_export.h"

#include <KoFlake.h>
#include <QComboBox>

class KoMarker;

class KRITAWIDGETS_EXPORT KoMarkerSelector : public QComboBox
{
    Q_OBJECT
public:
    explicit KoMarkerSelector(KoFlake::MarkerPosition position, QWidget *parent = 0);
    ~KoMarkerSelector() override;

    void setMarker(KoMarker *marker);
    KoMarker *marker() const;

    QVariant itemData(int index, int role = Qt::UserRole) const;

    void updateMarkers(const QList<KoMarker*> markers);

protected:
    void paintEvent(QPaintEvent *pe) override;

private:
    class Private;
    Private * const d;
};

#endif
