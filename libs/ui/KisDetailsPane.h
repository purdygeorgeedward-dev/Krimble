/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KISDETAILSPANE_H
#define KISDETAILSPANE_H

#include "ui_KisDetailsPaneBase.h"

class QEvent;
class QUrl;
class QStandardItemModel;

struct KisDetailsPanePrivate;

class KisDetailsPane : public QWidget, public Ui_KisDetailsPaneBase
{
    Q_OBJECT

public:
    KisDetailsPane(QWidget* parent, const QString& header);
    ~KisDetailsPane() override;

    bool eventFilter(QObject* watched, QEvent* e) override;

    QStandardItemModel* model() const;

Q_SIGNALS:
    void openUrl(const QUrl&);

    void splitterResized(KisDetailsPane* sender, const QList<int>& sizes);

public Q_SLOTS:
    void resizeSplitter(KisDetailsPane* sender, const QList<int>& sizes);

protected Q_SLOTS:
    virtual void selectionChanged(const QModelIndex& index) = 0;
    virtual void openFile();
    virtual void openFile(const QModelIndex& index) = 0;

    void changePalette();

protected:
    enum Extents {
        IconExtent = 64,
        PreviewExtent = 128
    };

private:
    KisDetailsPanePrivate * const d;
};

#endif
