/*  This file is part of the KDE Librariesfsignals
 *  SPDX-FileCopyrightText: 1998 Thomas Tanghus (tanghus@earthling.net)
 *  SPDX-FileCopyrightText: 1999-2000 Espen Sand (espen@kde.org)
 *  SPDX-FileCopyrightText: 1999-2000 Holger Freyther <freyther@kde.org>
 *  SPDX-FileCopyrightText: 2005-2009 Olivier Goffart <ogoffart @ kde.org>
 *  SPDX-FileCopyrightText: 2006 Tobias Koenig <tokoe@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KODIALOG_H
#define KODIALOG_H

class QPushButton;
class QMenu;
class KoDialogPrivate;

#include <kritawidgets_export.h>
#include <kconfiggroup.h>
#include <kguiitem.h>

#include <QDialog>

class KRITAWIDGETS_EXPORT KoDialog : public QDialog
{
    Q_OBJECT
    Q_ENUMS(ButtonCode)
    Q_DECLARE_PRIVATE(KoDialog)

public:

    enum ButtonCode {
        None    = 0x00000000,
        Help    = 0x00000001,
        Default = 0x00000002,
        Ok      = 0x00000004,
        Apply   = 0x00000008,
        Try     = 0x00000010,
        Cancel  = 0x00000020,
        Close   = 0x00000040,
        No      = 0x00000080,
        Yes     = 0x00000100,
        Reset   = 0x00000200,
        Details = 0x00000400,
        User1   = 0x00001000,
        User2   = 0x00002000,
        User3   = 0x00004000,
        NoDefault = 0x00008000
    };
    Q_DECLARE_FLAGS(ButtonCodes, ButtonCode)

    enum ButtonPopupMode {
        InstantPopup = 0,
        DelayedPopup = 1
    };
    Q_DECLARE_FLAGS(ButtonPopupModes, ButtonPopupMode)

public:
    explicit KoDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

    ~KoDialog() override;

    void setButtons(ButtonCodes buttonMask);

    void setButtonsOrientation(Qt::Orientation orientation);

    void setEscapeButton(ButtonCode id);

    void setDefaultButton(ButtonCode id);

    ButtonCode defaultButton() const;

    void showButtonSeparator(bool state);

    void showButton(ButtonCode id, bool state);

    void setButtonText(ButtonCode id, const QString &text);

    QString buttonText(ButtonCode id) const;

    void setButtonIcon(ButtonCode id, const QIcon &icon);

    QIcon buttonIcon(ButtonCode id) const;

    void setButtonToolTip(ButtonCode id, const QString &text);

    QString buttonToolTip(ButtonCode id) const;

    void setButtonWhatsThis(ButtonCode id, const QString &text);

    QString buttonWhatsThis(ButtonCode id) const;

    void setButtonGuiItem(ButtonCode id, const KGuiItem &item);

    void setButtonFocus(ButtonCode id);

    void setInitialSize(const QSize &size);

    void incrementInitialSize(const QSize &size);

    QString helpLinkText() const;

    bool isButtonEnabled(ButtonCode id) const;

    QPushButton *button(ButtonCode id) const;

    static int marginHint();

    static int spacingHint();

    static int groupSpacingHint();

    enum CaptionFlag {
        NoCaptionFlags = 0,
        AppNameCaption = 1,
        ModifiedCaption = 2,
        HIGCompliantCaption = AppNameCaption
    };
    Q_DECLARE_FLAGS(CaptionFlags, CaptionFlag)

    static QString makeStandardCaption(const QString &userCaption,
                                       QWidget *window = 0,
                                       CaptionFlags flags = HIGCompliantCaption);

    static void resizeLayout(QWidget *widget, int margin, int spacing);

    static void resizeLayout(QLayout *lay, int margin, int spacing);

    void setMainWidget(QWidget *widget);

    QWidget *mainWidget();

    QSize sizeHint() const override;

    QSize minimumSizeHint() const override;

public Q_SLOTS:
    virtual void setCaption(const QString &caption);

    virtual void setCaption(const QString &caption, bool modified);

    virtual void setPlainCaption(const QString &caption);

    void enableButton(ButtonCode id, bool state);

    void enableButtonOk(bool state);

    void enableButtonApply(bool state);

    void enableButtonCancel(bool state);

    void enableLinkedHelp(bool state);

    void setHelpLinkText(const QString &text);

    void setHelp(const QString &anchor, const QString &appname = QString());

    bool isDetailsWidgetVisible() const;

    void setDetailsWidgetVisible(bool visible);

    void setDetailsWidget(QWidget *detailsWidget);

    void delayedDestruct();

Q_SIGNALS:
    void layoutHintChanged();

    void helpClicked();

    void defaultClicked();

    void resetClicked();

    void user3Clicked();

    void user2Clicked();

    void user1Clicked();

    void applyClicked();

    void tryClicked();

    void okClicked();

    void yesClicked();

    void noClicked();

    void cancelClicked();

    void closeClicked();

    void buttonClicked(KoDialog::ButtonCode button);

    void hidden();

    void finished();

    void aboutToShowDetails();

protected:
    void hideEvent(QHideEvent *) override;

    void closeEvent(QCloseEvent *e) override;

    void keyPressEvent(QKeyEvent *) override;

    void showEvent(QShowEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

protected Q_SLOTS:
    virtual void slotButtonClicked(int button);

private:

    KoDialog(KoDialogPrivate &dd, QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());
    KoDialogPrivate *const d_ptr;

private:
    Q_DISABLE_COPY(KoDialog)
    Q_PRIVATE_SLOT(d_ptr, void queuedLayoutUpdate())
    Q_PRIVATE_SLOT(d_ptr, void helpLinkClicked())
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoDialog::ButtonCodes)
Q_DECLARE_OPERATORS_FOR_FLAGS(KoDialog::CaptionFlags)

#endif
