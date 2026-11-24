/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_PAINTOP_OPTION_H
#define KIS_PAINTOP_OPTION_H

#include <kis_types.h>
#include <kritaui_export.h>
#include <kis_properties_configuration.h>
#include <brushengine/kis_locked_properties_proxy.h>
#include <KisPaintopPropertiesBase.h>

#include <lager/reader.hpp>
#include <lager/cursor.hpp>

class QWidget;
class QString;
class KisPaintopLodLimitations;

class KRITAUI_EXPORT KisPaintOpOption : public QObject
{
    Q_OBJECT
public:

    using OptionalLodLimitationsReader = std::optional<lager::reader<KisPaintopLodLimitations>>;

    enum PaintopCategory {
        GENERAL,
        COLOR,
        TEXTURE,
        FILTER,
        MASKING_BRUSH
    };

    KisPaintOpOption(const QString &label, KisPaintOpOption::PaintopCategory category, bool checked);
    KisPaintOpOption(const QString &label, KisPaintOpOption::PaintopCategory category,
                     lager::cursor<bool> checkedCursor);
    KisPaintOpOption(const QString &label, KisPaintOpOption::PaintopCategory category,
                     lager::cursor<bool> checkedCursor,
                     lager::reader<bool> externallyEnabledLink);
    ~KisPaintOpOption() override;

    KisPaintOpOption::PaintopCategory category() const;
    virtual bool isCheckable() const;

    virtual bool isChecked() const;
    virtual void setChecked(bool checked);

    bool isEnabled() const;

    void setLocked(bool value);
    bool isLocked() const;

    QString label() const;

    virtual void setImage(KisImageWSP image);
    virtual void setNode(KisNodeWSP node);
    virtual void setResourcesInterface(KisResourcesInterfaceSP resourcesInterface);
    virtual void setCanvasResourcesInterface(KoCanvasResourcesInterfaceSP canvasResourcesInterface);

    void startReadOptionSetting(const KisPropertiesConfigurationSP setting);
    void startWriteOptionSetting(KisPropertiesConfigurationSP setting) const;

    QWidget *configurationPage() const;

    virtual void lodLimitations(KisPaintopLodLimitations *l) const;
    OptionalLodLimitationsReader effectiveLodLimitations() const;

protected:
    virtual OptionalLodLimitationsReader lodLimitationsReader() const;
    void setConfigurationPage(QWidget *page);

    KisResourcesInterfaceSP resourcesInterface() const;
    KoCanvasResourcesInterfaceSP canvasResourcesInterface() const;

protected:
    virtual void writeOptionSetting(KisPropertiesConfigurationSP setting) const {
        Q_UNUSED(setting);
    }

    virtual void readOptionSetting(const KisPropertiesConfigurationSP setting) {
        Q_UNUSED(setting);
    }

protected Q_SLOTS:
    void emitSettingChanged();
    void emitCheckedChanged(bool checked);
    void emitEnabledChanged(bool enabled);

Q_SIGNALS:

    void sigSettingChanged();

    void sigCheckedChanged(bool value);
    void sigEnabledChanged(bool value);

private:
    void slotEnablePageWidget(bool value);

protected:

    bool m_checkable {false};
    bool m_locked {false};

private:

    struct Private;
    Private* const m_d;
};

#endif
