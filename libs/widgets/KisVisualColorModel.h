/*
 * SPDX-FileCopyrightText: 2016 Wolthera van Hovell tot Westerflier <griffinvalley@gmail.com>
 * SPDX-FileCopyrightText: 2022 Mathias Wein <lynx.mw+kde@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef KIS_VISUAL_COLOR_MODEL_H
#define KIS_VISUAL_COLOR_MODEL_H

#include <QObject>
#include <QScopedPointer>

#include <KoColor.h>

#include "kritawidgets_export.h"

class KoColorSpace;
class KoColorDisplayRendererInterface;

class KRITAWIDGETS_EXPORT KisVisualColorModel : public QObject
{
    Q_OBJECT
public:
    enum ColorModel { None, Channel, HSV, HSL, HSI, HSY, YUV };

    KisVisualColorModel();
    ~KisVisualColorModel() override;

    KoColor currentColor() const;
    QVector4D channelValues() const;
    int colorChannelCount() const;
    ColorModel colorModel() const;
    QVector4D maxChannelValues() const;
    void setMaxChannelValues(const QVector4D &maxValues);
    void copyState(const KisVisualColorModel &other);
    void setRGBColorModel(ColorModel model);
    const KoColorSpace* colorSpace() const;
    bool isHSXModel() const;
    bool supportsExposure() const;
    KoColor convertChannelValuesToKoColor(const QVector4D &values) const;
    QVector4D convertKoColorToChannelValues(KoColor c) const;

public Q_SLOTS:

    void slotSetColor(const KoColor &c);
    void slotSetColorSpace(const KoColorSpace *cs);
    void slotSetChannelValues(const QVector4D &values);
    void slotLoadACSConfig();

private:
    void loadColorSpace(const KoColorSpace *cs);
    void emitChannelValues();

Q_SIGNALS:
    void sigColorModelChanged();
    void sigColorSpaceChanged();
    void sigChannelValuesChanged(const QVector4D &values, quint32 channelFlags);
    void sigNewColor(const KoColor &c);

private:
    struct Private;
    const QScopedPointer<Private> m_d;

};

typedef QSharedPointer<KisVisualColorModel> KisVisualColorModelSP;

#endif
