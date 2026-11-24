/*
 *  SPDX-FileCopyrightText: 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_FILE_LAYER_H
#define KIS_FILE_LAYER_H

#include "kritaui_export.h"

#include "kis_external_layer_iface.h"
#include "kis_safe_document_loader.h"
#include "kis_signal_auto_connection.h"

#include <kis_paint_device.h>
class KRITAUI_EXPORT KisFileLayer : public KisExternalLayer
{
    Q_OBJECT
public:

    enum ScalingMethod {
        None,
        ToImageSize,
        ToImagePPI
    };

    KisFileLayer(KisImageWSP image, const QString &name, quint8 opacity);
    KisFileLayer(KisImageWSP image, const QString& basePath, const QString &filename, ScalingMethod scalingMethod, QString scalingFilter, const QString &name, quint8 opacity, const KoColorSpace *fallbackColorSpace = 0);
    ~KisFileLayer() override;
    KisFileLayer(const KisFileLayer& rhs);

    QIcon icon() const override;

    void resetCache(const KoColorSpace *colorSpace = 0) override;

    KisPaintDeviceSP original() const override;
    KisPaintDeviceSP paintDevice() const override;
    void setSectionModelProperties(const KisBaseNode::PropertyList &properties) override;
    KisBaseNode::PropertyList sectionModelProperties() const override;

    void setFileName(const QString &basePath, const QString &filename);
    QString fileName() const;
    QString path() const;

    ScalingMethod scalingMethod() const;
    void setScalingMethod(ScalingMethod method);

    QString scalingFilter() const;
    void setScalingFilter(QString method);

    KisNodeSP clone() const override;
    bool allowAsChild(KisNodeSP) const override;

    bool accept(KisNodeVisitor&) override;
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter) override;

    KUndo2Command* crop(const QRect & rect) override;
    KUndo2Command* transform(const QTransform &transform) override;

    void setImage(KisImageWSP image) override;

private Q_SLOTS:
    void slotLoadingFinished(KisPaintDeviceSP projection, qreal xRes, qreal yRes, const QSize &size);
    void slotLoadingFailed();
    void slotFileExistsStateChanged(bool exists);
    void openFile() const;
    void slotImageSizeChanged();
    void slotImageResolutionChanged();

Q_SIGNALS:
    void sigRequestOpenFile();

private:
    enum State {
        FileLoaded,
        FileNotFound,
        FileLoadingFailed
    };

    void changeState(State newState);

private:
    QString m_basePath;
    QString m_filename;
    ScalingMethod m_scalingMethod {None};
    QString m_scalingFilter;

    KisPaintDeviceSP m_paintDevice;
    KisSafeDocumentLoader m_loader;
    QSize m_generatedForImageSize;
    qreal m_generatedForXRes = 0.0;
    qreal m_generatedForYRes = 0.0;

    State m_state = FileNotFound;

    KisSignalAutoConnectionsStore m_imageConnections;
};

#endif
