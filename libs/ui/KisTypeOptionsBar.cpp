/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "KisTypeOptionsBar.h"

#include <QWidgetAction>
#include <QToolBar>
#include <QToolButton>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QSignalBlocker>

#include <klocalizedstring.h>

#include <KisViewManager.h>
#include <KisMainWindow.h>
#include <kactioncollection.h>
#include <KisTextPropertiesManager.h>
#include <kis_icon_utils.h>
#include <kis_image.h>
#include <kis_font_family_combo_box.h>

#include <KoToolManager.h>
#include <KoSvgText.h>
#include <KoSvgTextProperties.h>
#include <KoSvgTextPropertiesInterface.h>

namespace {
// Qt 5 font weights (0 to 99) to CSS weights (100 to 900).
int cssWeightFromQtWeight(int qtWeight)
{
    if (qtWeight <= 12) return 100;
    if (qtWeight <= 37) return 300;
    if (qtWeight <= 53) return 400;
    if (qtWeight <= 60) return 500;
    if (qtWeight <= 69) return 600;
    if (qtWeight <= 78) return 700;
    if (qtWeight <= 84) return 800;
    return 900;
}
}

KisTypeOptionsBar::KisTypeOptionsBar(KisViewManager *viewManager)
    : QObject(viewManager)
    , m_view(viewManager)
{
    m_font = new KisFontComboBoxes();
    m_size = new QDoubleSpinBox();
    m_size->setRange(1.0, 2000.0);
    m_size->setDecimals(1);
    m_size->setSuffix(i18nc("Short for points, after a font size", " pt"));
    m_size->setValue(12.0);
    m_size->setKeyboardTracking(false);

    m_alignGroup = new QButtonGroup(this);
    m_alignGroup->setExclusive(false);   // "none" must be possible (justified text); the click handler keeps one checked

    registerAction(QStringLiteral("type_font"), i18n("Type: Font and Style"), m_font);
    registerAction(QStringLiteral("type_size"), i18n("Type: Size"), m_size);

    const char *alignIcons[3] = {"format-justify-left", "format-justify-center", "format-justify-right"};
    const char *alignNames[3] = {"type_align_left", "type_align_center", "type_align_right"};
    const QString alignTexts[3] = {i18n("Type: Align Left"), i18n("Type: Align Center"), i18n("Type: Align Right")};
    for (int i = 0; i < 3; ++i) {
        QToolButton *button = new QToolButton();
        button->setCheckable(true);
        button->setAutoRaise(true);
        button->setIcon(KisIconUtils::loadIcon(alignIcons[i]));
        button->setToolTip(alignTexts[i]);
        m_alignGroup->addButton(button, i);
        registerAction(QString::fromLatin1(alignNames[i]), alignTexts[i], button);
    }

    connect(m_font, SIGNAL(fontChanged(QString)), this, SLOT(slotFontChanged()));
    connect(m_size, SIGNAL(valueChanged(double)), this, SLOT(slotSizeChanged(double)));
    connect(m_alignGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotAlignClicked(int)));

    connect(m_view->textPropertyManager(), SIGNAL(sigInterfaceChanged(KoSvgTextPropertiesInterface*)),
            this, SLOT(slotInterfaceChanged(KoSvgTextPropertiesInterface*)));
    connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*)), this, SLOT(slotToolChanged()));

    updateVisibility();
}

QWidgetAction *KisTypeOptionsBar::registerAction(const QString &name, const QString &text, QWidget *widget)
{
    QWidgetAction *action = new QWidgetAction(this);
    action->setText(text);
    action->setDefaultWidget(widget);
    m_view->actionCollection()->addAction(name, action);
    m_actions << action;
    return action;
}

double KisTypeOptionsBar::pointsPerPixel() const
{
    // KisImage::xRes() is in pixels per point.
    if (m_view && m_view->image() && m_view->image()->xRes() > 0.0) {
        return 1.0 / m_view->image()->xRes();
    }
    return 1.0;
}

void KisTypeOptionsBar::updateVisibility()
{
    const bool show = m_interface && KoToolManager::instance()->activeToolId() == QLatin1String("SvgTextTool");
    // Krimble 2026-10-15: George (1.0.36): "Type Options toolbar is nothing? Why?? There should not be empty or blank toolbars." and "I thought the
    // type toolbar would just be ghosted when type is inactive." The buttons stay in the bar and are ghosted (disabled) while the Type tool is not
    // active, so the bar is never empty; it is shown when the Type tool is chosen, and not hidden again by this code.
    Q_FOREACH (QWidgetAction *action, m_actions) {
        // action->setVisible(show);
        action->setVisible(true);
        action->setEnabled(show);
        if (QWidget *actionWidget = action->defaultWidget()) {
            actionWidget->setEnabled(show);
        }
    }
    if (m_view && m_view->mainWindow()) {
        if (QToolBar *toolBar = m_view->mainWindow()->findChild<QToolBar*>(QStringLiteral("TypeOptionsBar"))) {
            // toolBar->setVisible(show);
            if (show) toolBar->setVisible(true);
        }
    }
}

void KisTypeOptionsBar::slotInterfaceChanged(KoSvgTextPropertiesInterface *interface)
{
    if (m_interface) {
        disconnect(m_interface, nullptr, this, nullptr);
    }
    m_interface = interface;
    if (m_interface) {
        connect(m_interface, SIGNAL(textSelectionChanged()), this, SLOT(slotSelectionChanged()));
        connect(m_interface, SIGNAL(textCharacterSelectionChanged()), this, SLOT(slotSelectionChanged()));
        slotSelectionChanged();
    }
    updateVisibility();
}

void KisTypeOptionsBar::slotToolChanged()
{
    updateVisibility();
}

void KisTypeOptionsBar::slotSelectionChanged()
{
    if (!m_interface) {
        return;
    }
    const bool span = m_interface->characterPropertiesEnabled() && m_interface->spanSelection();
    const QList<KoSvgTextProperties> list = span ? m_interface->getCharacterProperties() : m_interface->getSelectedProperties();
    const KoSvgTextProperties inherited = m_interface->getInheritedProperties();
    const KoSvgTextProperties props = list.isEmpty() ? inherited : list.first();
    auto value = [&](KoSvgTextProperties::PropertyId id) {
        return props.hasProperty(id) ? props.property(id) : inherited.propertyOrDefault(id);
    };

    m_updating = true;
    {
        QSignalBlocker fontBlocker(m_font);
        QSignalBlocker sizeBlocker(m_size);
        const QStringList families = value(KoSvgTextProperties::FontFamiliesId).toStringList();
        if (!families.isEmpty()) {
            m_font->setCurrentFamily(families.first());
        }
        const double px = value(KoSvgTextProperties::FontSizeId).toDouble();
        if (px > 0.0) {
            m_size->setValue(px * pointsPerPixel());
        }
    }
    int checked = -1;
    switch (value(KoSvgTextProperties::TextAlignAllId).toInt()) {
    case KoSvgText::AlignStart:
    case KoSvgText::AlignLeft:
        checked = 0;
        break;
    case KoSvgText::AlignCenter:
        checked = 1;
        break;
    case KoSvgText::AlignEnd:
    case KoSvgText::AlignRight:
        checked = 2;
        break;
    default:
        break;      // justified or unknown: none checked
    }
    Q_FOREACH (QAbstractButton *button, m_alignGroup->buttons()) {
        QSignalBlocker blocker(button);
        button->setChecked(m_alignGroup->id(button) == checked);
    }
    m_updating = false;
}

void KisTypeOptionsBar::applyToCharacters(const KoSvgTextProperties &properties)
{
    if (!m_interface) {
        return;
    }
    if (m_interface->characterPropertiesEnabled() && m_interface->spanSelection()) {
        m_interface->setCharacterPropertiesOnSelected(properties);
    } else {
        m_interface->setPropertiesOnSelected(properties);
    }
}

void KisTypeOptionsBar::slotFontChanged()
{
    if (m_updating || !m_interface) {
        return;
    }
    const QFont font = m_font->currentFont(12);
    KoSvgTextProperties out;
    out.setProperty(KoSvgTextProperties::FontFamiliesId, QVariant(QStringList() << m_font->currentFamily()));
    out.setProperty(KoSvgTextProperties::FontWeightId, QVariant(cssWeightFromQtWeight(font.weight())));
    // (KoSvgText::parseFontStyle is not exported from the library, so the value is built directly.)
    // out.setProperty(KoSvgTextProperties::FontStyleId,
    //                 QVariant::fromValue(KoSvgText::parseFontStyle(font.italic() ? QStringLiteral("italic") : QStringLiteral("normal"))));
    out.setProperty(KoSvgTextProperties::FontStyleId,
                    QVariant::fromValue(KoSvgText::CssFontStyleData(font.italic() ? QFont::StyleItalic : QFont::StyleNormal)));
    applyToCharacters(out);
}

void KisTypeOptionsBar::slotSizeChanged(double pointSize)
{
    if (m_updating || !m_interface) {
        return;
    }
    KoSvgTextProperties out;
    out.setProperty(KoSvgTextProperties::FontSizeId, QVariant(pointSize / pointsPerPixel()));
    applyToCharacters(out);
}

void KisTypeOptionsBar::slotAlignClicked(int id)
{
    if (m_updating || !m_interface) {
        return;
    }
    // exactly one of the three stays checked: the one just clicked
    Q_FOREACH (QAbstractButton *button, m_alignGroup->buttons()) {
        QSignalBlocker blocker(button);
        button->setChecked(m_alignGroup->id(button) == id);
    }
    KoSvgText::TextAlign align = KoSvgText::AlignLeft;
    if (id == 1) align = KoSvgText::AlignCenter;
    else if (id == 2) align = KoSvgText::AlignRight;
    KoSvgTextProperties out;
    out.setProperty(KoSvgTextProperties::TextAlignAllId, QVariant(int(align)));
    m_interface->setPropertiesOnSelected(out);
}
