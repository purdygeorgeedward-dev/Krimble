/*
 * Krimble 2026-10-25: see KrimbleLogDialog.h.
 */
#include "KrimbleLogDialog.h"

#include <klocalizedstring.h>

#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScreen>
#include <QStringList>
#include <QVBoxLayout>

QString KrimbleLogDialog::summaryOf(const QString &logText, int maxSessions)
{
    // a session starts at "SESSION:"; the first piece is whatever comes before the first one
    const QStringList pieces = logText.split(QStringLiteral("SESSION:"));
    QStringList lines;
    int shown = 0, notClosed = 0, total = qMax(0, pieces.size() - 1);
    for (int i = pieces.size() - 1; i >= 1; --i) {
        const QString &piece = pieces.at(i);
        const bool closed = piece.contains(QStringLiteral("CLOSING SESSION"));
        const bool flagged = piece.contains(QStringLiteral("DID NOT CLOSE CORRECTLY"));
        if (!closed || flagged) ++notClosed;
        if (shown >= maxSessions) continue;
        ++shown;
        const QString title = piece.section(QLatin1Char('\n'), 0, 0).trimmed();
        QString line = QStringLiteral("%1. %2\n     %3").arg(shown).arg(title.left(110),
                       flagged ? QStringLiteral("ENDED WITHOUT CLOSING CORRECTLY") : (closed ? QStringLiteral("closed correctly") : QStringLiteral("still running, or no closing line")));
        // "Last exit:" says why the run BEFORE this one ended (it is written near the top of this session)
        const QStringList pieceLines = piece.split(QLatin1Char('\n'));
        for (const QString &l : pieceLines) {
            const int at = l.indexOf(QStringLiteral("Last exit:"));
            if (at >= 0) { line += QStringLiteral("\n     before it: ") + l.mid(at + 10).trimmed(); break; }
        }
        lines << line;
    }
    return QStringLiteral("Sessions in the log: %1. Not closed correctly: %2.\n\n").arg(total).arg(notClosed) + lines.join(QStringLiteral("\n\n"));
}

KrimbleLogDialog::KrimbleLogDialog(QWidget *parent, const QString &logPath, const QString &extraText)
    : QDialog(parent)
{
    setWindowTitle(i18n("Krimble Log"));
    setAttribute(Qt::WA_DeleteOnClose, parent != nullptr);
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    QString text;
    QFile file(logPath);
    QString whole;
    if (file.open(QIODevice::ReadOnly)) {
        whole = QString::fromUtf8(file.readAll());
    }
    text += i18n("Log file: %1 (%2 KB)", logPath, QFileInfo(logPath).size() / 1024) + QLatin1String("\n\n");
    if (!extraText.trimmed().isEmpty()) {
        text += i18n("What Android told Krimble:") + QLatin1Char('\n') + extraText.trimmed() + QLatin1String("\n\n");
    }
    if (whole.isEmpty()) {
        text += i18n("The log is empty or could not be read.");
    } else {
        text += summaryOf(whole, 12);
        // the end of the log
        const QStringList all = whole.split(QLatin1Char('\n'));
        const int from = qMax(0, all.size() - 250);
        text += QStringLiteral("\n\n-------- ") + i18n("the end of the log (last %1 lines)", all.size() - from) + QStringLiteral(" --------\n");
        text += all.mid(from).join(QLatin1Char('\n'));
    }

    m_text = new QPlainTextEdit(this);
    m_text->setReadOnly(true);
    m_text->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    m_text->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_text->setPlainText(text);
    root->addWidget(m_text, 1);

    QPushButton *copyButton = new QPushButton(i18n("Copy"), this);
    QPushButton *closeButton = new QPushButton(i18n("Close"), this);
    QHBoxLayout *row = new QHBoxLayout();
    row->addWidget(copyButton, 1);
    row->addWidget(closeButton, 1);
    root->addLayout(row);
    connect(copyButton, &QPushButton::clicked, this, [this]() { QGuiApplication::clipboard()->setText(m_text->toPlainText()); });
    connect(closeButton, &QPushButton::clicked, this, [this]() { this->close(); });

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        resize(int(screen->availableGeometry().width() * 0.94), int(screen->availableGeometry().height() * 0.90));
    }
}
