/*
 * KDE. Krita Project.
 *
 * SPDX-FileCopyrightText: 2020 Deif Lou <ginoba@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISGENERICGRADIENTEDITOR_H
#define KISGENERICGRADIENTEDITOR_H

#include <QWidget>
#include <QScopedPointer>

#include <kritaui_export.h>
#include <KoAbstractGradient.h>
#include <KoCanvasResourcesInterface.h>

class KRITAUI_EXPORT KisGenericGradientEditor : public QWidget
{
    Q_OBJECT
public:
    KisGenericGradientEditor(QWidget* parent = 0);
    ~KisGenericGradientEditor() override;

    void loadUISettings(const QString &prefix = QString());
    void saveUISettings(const QString &prefix = QString());

    KoAbstractGradientSP gradient() const;
    KoCanvasResourcesInterfaceSP canvasResourcesInterface() const;
    bool compactMode() const;
    bool isConvertGradientButtonVisible() const;
    bool isUpdateGradientButtonVisible() const;
    bool isAddGradientButtonVisible() const;
    bool isGradientPresetChooserVisible() const;
    bool isGradientPresetChooserOptionsButtonVisible() const;
    bool useGradientPresetChooserPopUp() const;
    bool compactGradientPresetChooserMode() const;
    bool compactGradientEditorMode() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    bool event(QEvent *e) override;

public Q_SLOTS:
    void setGradient(KoAbstractGradientSP newGradient);
    void setCanvasResourcesInterface(KoCanvasResourcesInterfaceSP newCanvasResourcesInterface);
    void setCompactMode(bool compact);
    void setConvertGradientButtonVisible(bool visible);
    void setUpdateGradientButtonVisible(bool visible);
    void setAddGradientButtonVisible(bool visible);
    void setGradientPresetChooserVisible(bool visible);
    void setGradientPresetChooserOptionsButtonVisible(bool visible);
    void setUseGradientPresetChooserPopUp(bool use);
    void setCompactGradientPresetChooserMode(bool compact);
    void setCompactGradientEditorMode(bool compact);

Q_SIGNALS:
    void sigGradientChanged();

private:
    class Private;
    QScopedPointer<Private> m_d;

    void updateConvertGradientButton();
    void updateUpdateGradientButton();
    void updateAddGradientButton();
    void updateGradientPresetChooser();
    void updateGradientEditor();

private Q_SLOTS:
    void on_buttonConvertGradient_clicked();
    void on_buttonUpdateGradient_clicked();
    void on_buttonAddGradient_clicked();
    void on_widgetGradientPresetChooser_resourceClicked(KoResourceSP resource);
    void on_widgetGradientEditor_sigGradientChanged();
};

#endif
