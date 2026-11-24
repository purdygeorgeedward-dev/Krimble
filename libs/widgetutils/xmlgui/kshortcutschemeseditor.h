/*
 *  SPDX-FileCopyrightText: 2016 Michael Abrahams <miabraha@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QHBoxLayout>
#include <QString>
#include <QHash>

class KisShortcutsDialog;
class QPushButton;
class QComboBox;

class KisKShortcutSchemesEditor: public QHBoxLayout
{
    Q_OBJECT
public:
    KisKShortcutSchemesEditor(KisShortcutsDialog *parent);

    QString currentScheme();

private Q_SLOTS:
    void newScheme();
    void deleteScheme();
    void importShortcutsScheme();
    void exportShortcutsScheme();
    void loadCustomShortcuts();
    void saveCustomShortcuts();

Q_SIGNALS:
    void shortcutsSchemeChanged(const QString &);

protected:
    void updateDeleteButton();

private:
    QPushButton *m_newScheme {nullptr};
    QPushButton *m_deleteScheme {nullptr};
    QPushButton *m_exportScheme {nullptr};
    QComboBox *m_schemesList {nullptr};

    KisShortcutsDialog *m_dialog {nullptr};
    QHash<QString, QString> m_schemeFileLocations;
};

