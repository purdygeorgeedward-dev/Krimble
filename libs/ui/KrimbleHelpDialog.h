/*
 * Krimble 2026-10-25: the window of Help > Krimble Help. George: "Do we have a help file? We should." It shows the handbook (krita/data/aboutdata/krimble-help.md,
 * built by tools/help/make_help.py, compiled into the app by aboutdata.qrc) with a "Go to" list of its parts and a Find box. Qt widgets only (tools/help-bench).
 */
#ifndef KRIMBLE_HELP_DIALOG_H
#define KRIMBLE_HELP_DIALOG_H

#include <QDialog>

class QComboBox;
class QLineEdit;
class QTextBrowser;

class KrimbleHelpDialog : public QDialog
{
public:
    // markdown: the text of the handbook (an empty text shows a short message)
    KrimbleHelpDialog(QWidget *parent, const QString &markdown);

    static QString handbookText();   // the handbook from the app's resources (":/krimble-help.md")

private:
    void goToPart(const QString &heading);
    void findText(bool backwards);

    QTextBrowser *m_browser = nullptr;
    QComboBox *m_parts = nullptr;
    QLineEdit *m_find = nullptr;
};

#endif
