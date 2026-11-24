/*
 * SPDX-FileCopyrightText: 2005 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2005 C. Boemann <cbo@boemann.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_CUSTOM_IMAGE_WIDGET_H
#define KIS_CUSTOM_IMAGE_WIDGET_H

#include "kis_global.h"
#include "KoUnit.h"
#include "kis_properties_configuration.h"
#include "KisOpenPane.h"

#include <ui_wdgnewimage.h>

class KisDocument;
class KisDocument;

enum CustomImageWidgetType { CUSTOM_DOCUMENT, NEW_IMG_FROM_CB };

class WdgNewImage : public QWidget, public Ui::WdgNewImage
{
    Q_OBJECT

public:
    WdgNewImage(QWidget *parent)
        : QWidget(parent)
    {
        setupUi(this);
    }
};

class KisCustomImageWidget : public WdgNewImage
{
    Q_OBJECT
public:
    KisCustomImageWidget(QWidget *parent, qint32 defWidth, qint32 defHeight, double resolution, const QString & defColorModel, const QString & defColorDepth, const QString & defColorProfile, const QString & imageName);
    ~KisCustomImageWidget() override;

private Q_SLOTS:
    void widthUnitChanged(int index);
    void widthChanged(double value);
    void heightUnitChanged(int index);
    void heightChanged(double value);
    void resolutionChanged(double value);
    void predefinedClicked(int index);
    void saveAsPredefined();
    void setLandscape();
    void setPortrait();
    void switchWidthHeight();
    void createImage();
    void switchPortraitLandscape();
    void changeDocumentInfoLabel();
    void resolutionUnitChanged();

protected:

    KisDocument *createNewImage();

    void setNumberOfLayers(int layers);

    void showEvent(QShowEvent *) override;

    KisOpenPane *m_openPane;
private:

    double m_width, m_height;
    quint8 backgroundOpacity() const;
    void setBackgroundOpacity(quint8 value);

    void fillPredefined();

    KoUnit m_widthUnit, m_heightUnit;
    QList<KisPropertiesConfigurationSP> m_predefined;

};

#endif
