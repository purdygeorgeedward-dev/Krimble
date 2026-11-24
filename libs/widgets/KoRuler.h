/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>
   SPDX-FileCopyrightText: 2007 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef koRuler_h
#define koRuler_h

#include "kritawidgets_export.h"

#include <QWidget>
#include <QTextOption>

class QPaintEvent;

class KoViewConverter;
class KoCanvasBase;
class KoRulerPrivate;
class KoUnit;

class KRITAWIDGETS_EXPORT KoRuler : public QWidget
{
Q_OBJECT
public:
    KoRuler(QWidget* parent, Qt::Orientation orientation, const KoViewConverter* viewConverter);
    ~KoRuler() override;

    struct Tab {
        qreal position { 0.0 };
        QTextOption::TabType type { QTextOption::LeftTab };
    };

    KoUnit unit() const;

    qreal rulerLength() const;

    Qt::Orientation orientation() const;

    qreal firstLineIndent() const;

    qreal paragraphIndent() const;

    qreal endIndent() const;

    QWidget *tabChooser();

    void setPopupActionList(const QList<QAction*> &popupActionList);
    QList<QAction*> popupActionList() const;

    QSize minimumSizeHint() const override;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setUnit(const KoUnit &unit);

    void setOffset(int offset);

    void setRulerLength(qreal length);

    void setActiveRange(qreal start, qreal end);

    void setOverrideActiveRange(qreal start, qreal end);

    void setRightToLeft(bool isRightToLeft);

    void setShowIndents(bool show);

    void setFirstLineIndent(qreal indent);

    void setParagraphIndent(qreal indent);

    void setEndIndent(qreal indent);

    void setShowMousePosition(bool show);

    bool showMousePosition() const;

    void updateMouseCoordinate(int coordinate);

    void setShowSelectionBorders(bool show);

    void updateSelectionBorders(qreal first, qreal second);

    void setShowTabs(bool show);

    void setRelativeTabs(bool relative);

    void updateTabs(const QList<Tab> &tabs, qreal tabDistance);

    QList<Tab> tabs() const;

    void clearHotSpots();

    void setHotSpot(qreal position, int id = -1);

    bool removeHotSpot(int id);

    void createGuideToolConnection(KoCanvasBase *canvas);

    void setUnitPixelMultiple2(bool enabled);

Q_SIGNALS:
    void indentsChanged(bool final);

    void tabChanged(int originalTabIndex, KoRuler::Tab *tab);

    void aboutToChange();

    void hotSpotChanged(int id, qreal newPosition);

    void guideLineCreated(Qt::Orientation orientation, qreal viewPosition);

    void guideCreationInProgress(Qt::Orientation orientation, const QPoint &globalPos);
    void guideCreationFinished(Qt::Orientation orientation, const QPoint &globalPos);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;

private:
    KoRulerPrivate * const d;
    friend class KoRulerPrivate;
};

#endif
