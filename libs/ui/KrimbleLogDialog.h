#ifndef KRIMBLE_LOG_DIALOG_H
#define KRIMBLE_LOG_DIALOG_H

#include <QDialog>

class QPlainTextEdit;

class KrimbleLogDialog : public QDialog
{
public:
    KrimbleLogDialog(QWidget *parent, const QString &logPath, const QString &extraText);

    static QString summaryOf(const QString &logText, int maxSessions);

private:
    QPlainTextEdit *m_text = nullptr;
};

#endif
