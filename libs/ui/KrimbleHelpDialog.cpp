/*
 * Krimble 2026-10-25: see KrimbleHelpDialog.h.
 */
#include "KrimbleHelpDialog.h"

#include <klocalizedstring.h>

#include <QComboBox>
#include <QFile>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScreen>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTextDocument>
#include <QVBoxLayout>

QString KrimbleHelpDialog::handbookText()
{
    QFile file(QStringLiteral(":/krimble-help.md"));
    if (!file.open(QIODevice::ReadOnly)) return QString();
    return QString::fromUtf8(file.readAll());
}

KrimbleHelpDialog::KrimbleHelpDialog(QWidget *parent, const QString &markdown)
    : QDialog(parent)
{
    setWindowTitle(i18n("Krimble Help"));
    setAttribute(Qt::WA_DeleteOnClose, parent != nullptr);
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    // "Go to" the parts of the handbook (its "## " headings), and Find
    m_parts = new QComboBox(this);
    m_parts->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_parts->setMinimumContentsLength(8);
    m_parts->addItem(i18n("Go to..."));
    QStringList headings;
    const QStringList lines = markdown.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        if (line.startsWith(QLatin1String("## "))) headings << line.mid(3).trimmed();
    }
    m_parts->addItems(headings);
    m_find = new QLineEdit(this);
    m_find->setPlaceholderText(i18n("Find"));
    m_find->setClearButtonEnabled(true);
    QHBoxLayout *top = new QHBoxLayout();
    top->addWidget(m_parts, 1);
    top->addWidget(m_find, 1);
    root->addLayout(top);

    m_browser = new QTextBrowser(this);
    m_browser->setOpenExternalLinks(true);
    m_browser->document()->setDefaultStyleSheet(QStringLiteral("td, th { padding: 4px; } code { font-family: monospace; }"));
    if (markdown.trimmed().isEmpty()) m_browser->setPlainText(i18n("The help could not be found."));
    else m_browser->setMarkdown(markdown);
    root->addWidget(m_browser, 1);

    QPushButton *closeButton = new QPushButton(i18n("Close"), this);
    root->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, [this]() { this->close(); });
    connect(m_parts, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
        if (index > 0) goToPart(m_parts->itemText(index));
    });
    connect(m_find, &QLineEdit::returnPressed, this, [this]() { findText(false); });
    connect(m_find, &QLineEdit::textChanged, this, [this](const QString &text) {
        // as it is typed: from the start of the word being typed
        if (text.isEmpty()) return;
        QTextCursor c = m_browser->textCursor();
        c.setPosition(qMin(c.selectionStart(), c.position()));
        m_browser->setTextCursor(c);
        findText(false);
    });

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        resize(int(screen->availableGeometry().width() * 0.92), int(screen->availableGeometry().height() * 0.90));
    }
}

void KrimbleHelpDialog::goToPart(const QString &heading)
{
    QTextDocument *doc = m_browser->document();
    for (QTextBlock b = doc->begin(); b.isValid(); b = b.next()) {
        if (b.blockFormat().headingLevel() == 2 && b.text().trimmed() == heading) {
            QTextCursor c(b);
            m_browser->setTextCursor(c);
            // the heading at the top of the window
            QScrollBar *bar = m_browser->verticalScrollBar();
            bar->setValue(bar->value() + m_browser->cursorRect(c).top() - 4);
            return;
        }
    }
}

void KrimbleHelpDialog::findText(bool backwards)
{
    const QString text = m_find->text();
    if (text.isEmpty()) return;
    QTextDocument::FindFlags flags;
    if (backwards) flags |= QTextDocument::FindBackward;
    if (!m_browser->find(text, flags)) {
        // from the other end
        m_browser->moveCursor(backwards ? QTextCursor::End : QTextCursor::Start);
        m_browser->find(text, flags);
    }
}
