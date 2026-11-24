/*
 * SPDX-FileCopyrightText: 2020 Mathias Wein <lynx.mw+kde@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef WGCONFIG_H
#define WGCONFIG_H

#include <KisColorSelectorConfiguration.h>

#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include <QVector4D>

#include <type_traits>

class KoColorSpace;

namespace WGConfig {

template<class T> struct GenericSetting
{
    typedef T ValueType;

    T readValue(const KConfigGroup &group) const
    {
        return group.readEntry(name, defaultValue);
    }

    void writeValue(KConfigGroup &group, const T &value) const
    {
        group.writeEntry(name, value);
    }

    QString name;
    T defaultValue;
};

template<class T> struct NumericSetting
{
    typedef T ValueType;
    using ST = typename std::conditional_t<std::is_enum<T>::value, std::underlying_type<T>, std::remove_cv<T>>::type;

    T readValue(const KConfigGroup &group) const
    {
        T value = static_cast<T>(group.readEntry(name, static_cast<ST>(defaultValue)));
        return applyLimits(value);
    }

    void writeValue(KConfigGroup &group, const T &value) const
    {
        group.writeEntry(name, static_cast<ST>(value));
    }

    T boundFunc(const T &min, const T &val, const T &max) const
    {
        return qBound(min, val, max);
    }

    T applyLimits(T value) const {
        if (enforceLimits) {
            return boundFunc(minValue, value, maxValue);
        }
        return value;
    }
    QString name;
    T defaultValue;
    T minValue;
    T maxValue;
    bool enforceLimits {false};
};

template<>
inline QSize NumericSetting<QSize>::boundFunc(const QSize &min, const QSize &val, const QSize &max) const
{
    return val.expandedTo(min).boundedTo(max);
}

struct ShadeLine {
    ShadeLine() = default;
    explicit ShadeLine(QVector4D grad, QVector4D offs=QVector4D(), int patchC=-1)
        : gradient(grad), offset(offs), patchCount(patchC) {}
    QVector4D gradient;
    QVector4D offset;
    int patchCount {-1};
};

class WGConfig
{
public:

    explicit WGConfig(bool readOnly = true);
    ~WGConfig();

    template<class T>
    typename T::ValueType get(const T &setting, bool defaultValue = false) const
    {
        if (defaultValue) {
            return setting.defaultValue;
        }
        return setting.readValue(m_cfg);
    }

    template<class T>
    void set(const T &setting, const typename T::ValueType &value) { setting.writeValue(m_cfg, value); }

    static QString configGroupName();

    KisColorSelectorConfiguration colorSelectorConfiguration() const;
    void setColorSelectorConfiguration(const KisColorSelectorConfiguration &config);

    QVector<KisColorSelectorConfiguration> favoriteConfigurations(bool defaultValue = false) const;
    static QVector<KisColorSelectorConfiguration> defaultFavoriteConfigurations();
    void setFavoriteConfigurations(const QVector<KisColorSelectorConfiguration> &favoriteConfigs);

    static QVector<ShadeLine> defaultShadeSelectorLines();
    QVector<ShadeLine> shadeSelectorLines(bool defaultValue = false) const;
    void setShadeSelectorLines(const QVector<ShadeLine> &shadeLines);

    const KoColorSpace* customSelectionColorSpace(bool defaultValue = false) const;
    void setCustomSelectionColorSpace(const KoColorSpace *cs);

    template<class T>
    void writeEntry(const QString& name, const T& value) {
        m_cfg.writeEntry(name, value);
    }

    template<class T>
    T readEntry(const QString& name, const T& defaultValue=T()) {
        return m_cfg.readEntry(name, defaultValue);
    }

    static const KisColorSelectorConfiguration defaultColorSelectorConfiguration;

private:
    KConfigGroup m_cfg;
    bool m_readOnly;
};

typedef WGConfig Accessor;

class WGConfigNotifier : public QObject
{
    Q_OBJECT
public:
    WGConfigNotifier() = default;
    WGConfigNotifier(const WGConfigNotifier&) = delete;
    ~WGConfigNotifier() override = default;

    WGConfigNotifier operator=(const WGConfigNotifier&) = delete;

    void notifyConfigChanged();
    void notifySelectorConfigChanged();

Q_SIGNALS:
    void configChanged();
    void selectorConfigChanged();
};

WGConfigNotifier* notifier();

enum Scrolling {
    ScrollNone,
    ScrollLongitudinal,
    ScrollLaterally
};

struct ColorPatches
{
    NumericSetting<Qt::Orientation> orientation;
    NumericSetting<QSize> patchSize;
    NumericSetting<int> maxCount;
    NumericSetting<int> rows;
    NumericSetting<Scrolling> scrolling;
};

extern const ColorPatches colorHistory;
extern const ColorPatches commonColors;
extern const ColorPatches popupPatches;

extern const GenericSetting<bool> proofToPaintingColors;
extern const GenericSetting<bool> colorHistoryEnabled;
extern const GenericSetting<bool> colorHistoryFromDocument;
extern const GenericSetting<bool> commonColorsEnabled;
extern const GenericSetting<bool> colorHistoryShowClearButton;
extern const GenericSetting<bool> commonColorsAutoUpdate;

extern const GenericSetting<bool> quickSettingsEnabled;
extern const NumericSetting<int> popupSize;

extern const GenericSetting<bool> minimalShadeSelectorEnabled;
extern const NumericSetting<int> shadeSelectorLineHeight;
extern const GenericSetting<bool> shadeSelectorUpdateOnExternalChanges;
extern const GenericSetting<bool> shadeSelectorUpdateOnInteractionEnd;
extern const GenericSetting<bool> shadeSelectorUpdateOnRightClick;

}

#endif
