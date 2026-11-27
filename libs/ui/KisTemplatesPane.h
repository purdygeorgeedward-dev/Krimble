/*
 * SPDX-FileCopyrightText: 2005-2006 Peter Simonsson <psn@linux.se>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIS_TEMPLATES_PANE_H
#define KIS_TEMPLATES_PANE_H

#include "KisDetailsPane.h"

class KisTemplate;
class KisTemplateGroup;

class KisTemplatesPanePrivate;

class KisTemplatesPane : public KisDetailsPane
{

    Q_OBJECT
public:
    KisTemplatesPane(QWidget* parent, const QString& header,
                    KisTemplateGroup* group, KisTemplate* defaultTemplate);
    ~KisTemplatesPane() override;

    bool isSelected();

Q_SIGNALS:
    void alwaysUseChanged(KisTemplatesPane* sender, const QString& alwaysUse);

protected Q_SLOTS:
    void selectionChanged(const QModelIndex& index) override;

    void openFile() override;
    void openFile(const QModelIndex& index) override;
    void alwaysUseClicked();
    void changeAlwaysUseTemplate(KisTemplatesPane* sender, const QString& alwaysUse);

private:
    KisTemplatesPanePrivate * const d;
};

#endif
