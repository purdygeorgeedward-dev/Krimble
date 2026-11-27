/*
 *  SPDX-FileCopyrightText: 2016 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef VIDEO_SAVER_H_
#define VIDEO_SAVER_H_

#include <QObject>

#include "kis_types.h"

#include <KisImportExportFilter.h>

class KisDocument;
class KisAnimationRenderingOptions;

#include "kritaui_export.h"

class KRITAUI_EXPORT KisAnimationVideoSaver : public QObject {
    Q_OBJECT
public:
    KisAnimationVideoSaver(KisDocument* doc, bool batchMode);
    ~KisAnimationVideoSaver() override;

    KisImageSP image();

    KisImportExportErrorCode encode(const QString &framesDirectory,
                                    const QString &savedFilesMask,
                                    const QStringList &savedFiles,
                                    const KisAnimationRenderingOptions &options);

    static KisImportExportErrorCode convert(KisDocument *document,
                                            const QString &framesDirectory,
                                            const QString &savedFilesMask,
                                            const QStringList &savedFiles,
                                            const KisAnimationRenderingOptions &options,
                                            bool batchMode);

private:
    KisImageSP m_image;
    KisDocument* m_doc;
    bool m_batchMode;
};

#endif
