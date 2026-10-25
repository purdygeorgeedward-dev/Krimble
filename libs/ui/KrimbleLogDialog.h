/*
 * Krimble 2026-10-25: Help > Show Krimble Log... George: "Krimble relaunches constantly on switching ... I want it to stop crashing." Krimble already writes a log (krita.log) that says, at the start of
 * every run, why the run before it ended ("Last exit: reason ..., importance ..., signal ...") and marks a run that did not end correctly ("KRITA DID NOT CLOSE CORRECTLY"), but nothing in the app
 * showed it. This window shows a summary of the last sessions (newest first), then the end of the log. Qt widgets only (tools/log-bench).
 */
#ifndef KRIMBLE_LOG_DIALOG_H
#define KRIMBLE_LOG_DIALOG_H

#include <QDialog>

class QPlainTextEdit;

class KrimbleLogDialog : public QDialog
{
public:
    // logPath: the file krita.log. extraText: what the Android side knows (why the window was last destroyed); may be empty.
    KrimbleLogDialog(QWidget *parent, const QString &logPath, const QString &extraText);

    static QString summaryOf(const QString &logText, int maxSessions);   // the summary of the sessions, newest first (also used by the bench)

private:
    QPlainTextEdit *m_text = nullptr;
};

#endif
