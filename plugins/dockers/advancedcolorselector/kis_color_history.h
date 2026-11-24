/*
 *  SPDX-FileCopyrightText: 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_COLOR_HISTORY_H
#define KIS_COLOR_HISTORY_H

#include "kis_color_patches.h"

class QToolButton;
class KisCanvasResourceProvider;
class KisColorHistoryNotifier;
class KisUniqueColorSet;

class KisColorHistory : public KisColorPatches
{
    Q_OBJECT
public:
    explicit KisColorHistory(QWidget *parent = 0);
    ~KisColorHistory();
    void setCanvas(KisCanvas2 *canvas) override;
    void unsetCanvas() override;

protected:
    KisColorSelectorBase* createPopup() const override;

public Q_SLOTS:
    void addColorToHistory(const KoColor& color);

    void clearColorHistory();

    void updateUserSettings();

    void colorHistoryChanged();

private:
    KisUniqueColorSet *colorHistoryModel();
    std::unique_ptr<KisUniqueColorSet> m_fallbackColorHistoryModel;

    void updateColorHistory(const QList<KoColor> &history);

    bool m_history_per_document = false;

    QToolButton* m_clearButton;
    KisDocument *m_document;
    KisCanvasResourceProvider  *m_resourceProvider;
};

class KisColorHistoryNotifier : public QObject
{
Q_OBJECT

public:
    void notifyColorHistoryChanged(const QList<KoColor> &history);

Q_SIGNALS:

    void colorHistoryChanged(const QList<KoColor> &history);
};

#endif
