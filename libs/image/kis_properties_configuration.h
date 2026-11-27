/*
 *  SPDX-FileCopyrightText: 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef _KIS_PROPERTIES_CONFIGURATION_H_
#define _KIS_PROPERTIES_CONFIGURATION_H_

#include <QString>
#include <QMap>
#include <QVariant>
#include <kis_debug.h>
#include <kis_cubic_curve.h>
#include <KoColor.h>

class QDomElement;
class QDomDocument;

#include "kis_serializable_configuration.h"
#include "kritaimage_export.h"
#include "kis_types.h"

class KRITAIMAGE_EXPORT KisPropertiesConfiguration : public KisSerializableConfiguration
{

public:

    KisPropertiesConfiguration();
    ~KisPropertiesConfiguration() override;

    KisPropertiesConfiguration(const KisPropertiesConfiguration& rhs);

    KisPropertiesConfiguration& operator=(const KisPropertiesConfiguration& rhs);

public:

    bool fromXML(const QString& xml, bool clear = true) override;

    void fromXML(const QDomElement&) override;

    void toXML(QDomDocument&, QDomElement&) const override;

    QString toXML() const override;

    virtual bool hasProperty(const QString& name) const;

    virtual void setProperty(const QString & name, const QVariant & value);

    virtual bool getProperty(const QString & name, QVariant & value) const;

    virtual QVariant getProperty(const QString & name) const;

    template <typename T>
        T getPropertyLazy(const QString & name, const T &defaultValue) const {
        QVariant value = getProperty(name);
        return value.isValid() ? value.value<T>() : defaultValue;
    }

    QString getPropertyLazy(const QString & name, const char *defaultValue) const {
        return getPropertyLazy(name, QString(defaultValue));
    }

    int getInt(const QString & name, int def = 0) const;

    double getDouble(const QString & name, double def = 0.0) const;

    float getFloat(const QString& name, float def = 0.0) const;

    bool getBool(const QString & name, bool def = false) const;

    QString getString(const QString & name, const QString & def = QString()) const;

    KisCubicCurve getCubicCurve(const QString & name, const KisCubicCurve & curve = KisCubicCurve()) const;

    KoColor getColor(const QString& name, const KoColor& color = KoColor()) const;

    virtual QMap<QString, QVariant> getProperties() const;

    void clearProperties();

    void setPropertyNotSaved(const QString & name);

    void removeProperty(const QString & name);

    virtual QList<QString> getPropertiesKeys() const;

    void getPrefixedProperties(const QString &prefix, KisPropertiesConfiguration *config) const;

    void getPrefixedProperties(const QString &prefix, KisPropertiesConfigurationSP config) const;

    void setPrefixedProperties(const QString &prefix, const KisPropertiesConfiguration *config);

    void setPrefixedProperties(const QString &prefix, const KisPropertiesConfigurationSP config);

    static QString extractedPrefixKey();

    static QString escapeString(const QString &string);
    static QString unescapeString(const QString &string);

    void setProperty(const QString &name, const QStringList &value);
    QStringList getStringList(const QString &name, const QStringList &defaultValue = QStringList()) const;
    QStringList getPropertyLazy(const QString &name, const QStringList &defaultValue) const;

    virtual bool compareTo(const KisPropertiesConfiguration* rhs) const;

public:

    virtual void dump() const;

private:

    struct Private;
    Private* const d;
};

class KRITAIMAGE_EXPORT KisPropertiesConfigurationFactory : public KisSerializableConfigurationFactory
{
public:
    KisPropertiesConfigurationFactory();
    ~KisPropertiesConfigurationFactory() override;
    KisSerializableConfigurationSP createDefault() override;
    KisSerializableConfigurationSP create(const QDomElement& e) override;
private:
    struct Private;
    Private* const d;
};

#endif
