/*
 *  kis_clipboard.h - part of Krayon
 *
 *  SPDX-FileCopyrightText: 2004 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2019 Dmitrii Utkin <loentar@gmail.com>
 *  SPDX-FileCopyrightText: 2022 L. E. Segovia <amy@amyspark.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef __KIS_CLIPBOARD_H_
#define __KIS_CLIPBOARD_H_

#include <QObject>
#include <QSize>

#include <kis_types.h>

#include <kritaui_export.h>

class QRect;
class QMimeData;
class KisTimeSpan;
class KoColorSpace;

class KisClipboardPrivate;

class KRITAUI_EXPORT KisClipboard : public QObject
{

    Q_OBJECT
    Q_PROPERTY(bool clip READ hasClip NOTIFY clipChanged)

public:
    enum PasteBehaviour { PASTE_ASSUME_WEB = 0, PASTE_ASSUME_MONITOR, PASTE_ASK };

    enum PasteFormatBehaviour { PASTE_FORMAT_ASK = 0, PASTE_FORMAT_DOWNLOAD, PASTE_FORMAT_LOCAL, PASTE_FORMAT_CLIP };

    KisClipboard();
    ~KisClipboard() override;

    static KisClipboard* instance();

    void setClip(KisPaintDeviceSP dev, const QPoint& topLeft);

    void setClip(KisPaintDeviceSP dev, const QPoint& topLeft, const KisTimeSpan &range);

    KisPaintDeviceSP clip(const QRect &imageBounds,
                          bool showPopup,
                          int overridePasteBehaviour = -1,
                          KisTimeSpan *clipRange = nullptr) const;

    QPair<bool, PasteFormatBehaviour> askUserForSource(const QMimeData *data,
                                                       bool useClipboardFallback = false) const;

    QPair<bool, PasteFormatBehaviour> askUserForSourceWithData(QImage qimage,
                                                               const QList<QUrl> urls,
                                                               bool useClipboardFallback = false) const;
    KisPaintDeviceSP clipFromMimeData(const QMimeData *data,
                                      const QRect &imageBounds,
                                      bool showPopup,
                                      int overridePasteBehaviour = -1,
                                      KisTimeSpan *clipRange = nullptr,
                                      bool useClipboardFallback = false) const;

    KisPaintDeviceSP clipFromKritaLayers(const KoColorSpace *cs) const;

    KisPaintDeviceSP clipFromBoardContents(const QMimeData *data,
                          const QRect &imageBounds,
                          bool showPopup,
                          int overridePasteBehaviour = -1,
                          bool useClipboardFallback = false,
                          QPair<bool, PasteFormatBehaviour> source = {
                              false,
                              PasteFormatBehaviour::PASTE_FORMAT_ASK}) const;

    KisPaintDeviceSP clipFromBoardContentsWithData(
        QImage image,
        const QList<QUrl> urls,
        const QRect &imageBounds,
        bool showPopup,
        int overridePasteBehaviour = -1,
        bool useClipboardFallback = false,
        QPair<bool, PasteFormatBehaviour> source = {false, PasteFormatBehaviour::PASTE_FORMAT_ASK}) const;

    QImage getImageWithFallback(const QMimeData *cbData, bool useClipboardFallback = false) const;

    bool hasClip() const;

    QSize clipSize() const;

    void setLayers(KisNodeList nodes, KisImageSP image, bool forceCopy = false);
    bool hasLayers() const;
    bool hasLayerStyles() const;

    const QMimeData* layersMimeData() const;

    bool hasUrls() const;
    bool hasImage() const;

Q_SIGNALS:
    void clipChanged();

private Q_SLOTS:
    void clipboardDataChanged();

private:
    Q_DISABLE_COPY(KisClipboard);

    KisPaintDeviceSP
    clipFromKritaSelection(const QMimeData *data, const QRect &imageBounds, KisTimeSpan *clipRange) const;

    KisPaintDeviceSP fetchImageByURL(const QUrl &originalUrl) const;

    QImage getImageFromMimeData(const QMimeData *cbData) const;

    KisClipboardPrivate *const d;
};

#endif
