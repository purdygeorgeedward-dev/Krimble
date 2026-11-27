/*
 *  SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KIS_RECOVER_NAMED_AUTOSAVE_DIALOG_H
#define KIS_RECOVER_NAMED_AUTOSAVE_DIALOG_H

#include <QDialog>
#include "kis_types.h"

namespace Ui {
class KisRecoverNamedAutosaveDialog;
}

class KisRecoverNamedAutosaveDialog : public QDialog
{
    Q_OBJECT

public:
    enum ResultType {
        OpenAutosave,
        OpenMainFile,
        Cancel
    };

public:
    explicit KisRecoverNamedAutosaveDialog(QWidget *parent = 0, QString mainFile = "", QString autosaveFile = "");
    ~KisRecoverNamedAutosaveDialog() override;

private Q_SLOTS:
    void slotOkRequested();
    void slotCancelRequested();

private:
    Ui::KisRecoverNamedAutosaveDialog *ui;

};

#endif
