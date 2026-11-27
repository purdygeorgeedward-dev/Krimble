/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Fela Winkelmolen <fela.kde@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KARBONCALLIGRAPHYOPTIONWIDGET_H
#define KARBONCALLIGRAPHYOPTIONWIDGET_H

#include <QWidget>
#include <QMap>

class KComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QToolButton;
class KisAngleSelector;

class KarbonCalligraphyOptionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KarbonCalligraphyOptionWidget();
    ~KarbonCalligraphyOptionWidget() override;

    void emitAll();

Q_SIGNALS:
    void usePathChanged(bool);
    void usePressureChanged(bool);
    void useAngleChanged(bool);
    void widthChanged(double);
    void thinningChanged(double);
    void angleChanged(int);
    void fixationChanged(double);
    void capsChanged(double);
    void massChanged(double);
    void dragChanged(double);

public Q_SLOTS:
    void increaseWidth();
    void decreaseWidth();
    void increaseAngle();
    void decreaseAngle();

private Q_SLOTS:
    void loadProfile(const QString &name);
    void toggleUseAngle(bool checked);
    void updateCurrentProfile();
    void saveProfileAs();
    void removeProfile();

    void setUsePathEnabled(bool enabled);

    void on_m_angleBox_angleChanged(qreal angle);

private:
    struct Profile {
        QString name;
        int index;
        bool usePath;
        bool usePressure;
        bool useAngle;
        qreal width;
        qreal thinning;
        int angle;
        qreal fixation;
        qreal caps;
        qreal mass;
        qreal drag;
    };

    void createConnections();

    void addDefaultProfiles();

    void loadProfiles();

    void loadCurrentProfile();

    void saveProfile(const QString &name);

    void removeProfile(const QString &name);

    int profilePosition(const QString &profileName);

private:
    typedef QMap<QString, Profile *> ProfileMap;
    ProfileMap m_profiles;

    KComboBox *m_comboBox;
    QCheckBox *m_usePath;
    QCheckBox *m_usePressure;
    QCheckBox *m_useAngle;
    QDoubleSpinBox  *m_widthBox;
    QDoubleSpinBox  *m_thinningBox;
    KisAngleSelector *m_angleBox;
    QDoubleSpinBox  *m_capsBox;
    QDoubleSpinBox  *m_fixationBox;
    QDoubleSpinBox  *m_massBox;
    QDoubleSpinBox  *m_dragBox;

    QToolButton *m_saveButton;
    QToolButton *m_removeButton;

    bool m_changingProfile;
};

#endif
