/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KISTYPEOPTIONSBAR_H
#define KISTYPEOPTIONSBAR_H

#include <QObject>
#include <QPointer>
#include <QList>

class KisViewManager;
class KoSvgTextPropertiesInterface;
class KisFontComboBoxes;
class QDoubleSpinBox;
class QToolButton;
class QButtonGroup;
class QWidgetAction;
class QWidget;

class KisTypeOptionsBar : public QObject
{
    Q_OBJECT
public:
    explicit KisTypeOptionsBar(KisViewManager *viewManager);

private Q_SLOTS:
    void slotInterfaceChanged(KoSvgTextPropertiesInterface *interface);
    void slotSelectionChanged();
    void slotToolChanged();
    void slotFontChanged();
    void slotSizeChanged(double pointSize);
    void slotAlignClicked(int id);

private:
    QWidgetAction *registerAction(const QString &name, const QString &text, QWidget *widget);
    double pointsPerPixel() const;
    void updateVisibility();
    void applyToCharacters(const class KoSvgTextProperties &properties);

    QPointer<KisViewManager> m_view;
    QPointer<KoSvgTextPropertiesInterface> m_interface;
    KisFontComboBoxes *m_font {nullptr};
    QDoubleSpinBox *m_size {nullptr};
    QButtonGroup *m_alignGroup {nullptr};
    QList<QWidgetAction*> m_actions;
    bool m_updating {false};
};

#endif
