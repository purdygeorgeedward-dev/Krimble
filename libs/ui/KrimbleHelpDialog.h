#ifndef KRIMBLE_HELP_DIALOG_H
#define KRIMBLE_HELP_DIALOG_H

#include <QDialog>

class QComboBox;
class QLineEdit;
class QTextBrowser;

class KrimbleHelpDialog : public QDialog
{
public:
    KrimbleHelpDialog(QWidget *parent, const QString &markdown);

    static QString handbookText();

private:
    void goToPart(const QString &heading);
    void findText(bool backwards);

    QTextBrowser *m_browser = nullptr;
    QComboBox *m_parts = nullptr;
    QLineEdit *m_find = nullptr;
};

#endif
