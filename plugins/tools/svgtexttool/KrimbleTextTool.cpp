/*
 * Krimble 2026-10-18: see KrimbleTextTool.h.
 */
#include "KrimbleTextTool.h"

#include <KoCanvasBase.h>
#include <KoColorBackground.h>
#include <KoPointerEvent.h>
#include <KoProperties.h>
#include <KoShapeContainer.h>
#include <KoShapeController.h>
#include <KoShapeControllerBase.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoSvgTextShape.h>
#include <KoSvgTextShapeMarkupConverter.h>
#include <KoShapeManager.h>
#include <KoViewConverter.h>
#include <QPainter>   // Krimble 2026-10-20: the dashed text box
#include <commands/KoShapeBackgroundCommand.h>
#include "SvgTextChangeCommand.h"
#include <KoIcon.h>
#include <klocalizedstring.h>
#include <kundo2command.h>

#include <functional>
#include <QFrame>
#include <QApplication>
#include <QButtonGroup>
#include <QColorDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFontComboBox>
#include <QFontMetricsF>   // Krimble 2026-10-20: kerning preview
#include <QTextDocument>   // Krimble 2026-10-20: kerning preview
#include <QPixmap>   // Krimble 2026-10-20: kerning preview
#include <QFontDatabase>   // Krimble 2026-10-20: the font list
#include <QComboBox>   // Krimble 2026-10-20: the font list
#include <QStringListModel>   // Krimble 2026-10-19: Noto families taken out of the font list
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QXmlStreamReader>

namespace {
// what was chosen last time is what the window starts with next time
QString s_family = QStringLiteral("sans-serif");
qreal s_size = 24.0;
bool s_bold = false;
bool s_italic = false;
// Krimble 2026-10-20: George (1.0.39, test list, item 9): "New type tool just makes one long line of text. You can't make a paragraph area and have it
// flow into it." The width of the text box (document units), or 0 for text that is one line per typed line (no box).
qreal s_boxWidth = 0.0;
// Krimble 2026-10-20: George (1.0.40, with his fonts): "they have horrible kerning issues. I think we need kerning controls for the small adjustments
// between letters in a display typeface." Tracking (letter-spacing for the whole text, pt) and kerning (an extra gap, pt, before the character at each
// position of the typed text; negative pulls the letters together).
qreal s_tracking = 0.0;
QVector<qreal> s_kern;
int s_align = 0;               // 0 left, 1 center, 2 right
QColor s_color = QColor(Qt::black);
}

// Krimble 2026-10-20: the dx list (SVG per-character shifts) for the characters text[from .. from+count), from the kerning; empty when all are 0
static QString krimbleDxList(int from, int count)
{
    int last = -1;
    for (int i = 0; i < count; ++i) {
        if (from + i < s_kern.size() && qAbs(s_kern.at(from + i)) > 0.0001) last = i;
    }
    if (last < 0) return QString();
    QStringList values;
    for (int i = 0; i <= last; ++i) {
        const qreal v = (from + i < s_kern.size()) ? s_kern.at(from + i) : 0.0;
        values << QString::number(v, 'f', 2);
    }
    return values.join(QLatin1Char(' '));
}

static QString krimbleTextSvg(const QString &text)
{
    const QStringList lines = text.split(QLatin1Char('\n'));
    const QString anchor = (s_align == 0) ? QStringLiteral("start") : (s_align == 1 ? QStringLiteral("middle") : QStringLiteral("end"));
    QString family = s_family;
    family.remove(QLatin1Char('\''));
    family.remove(QLatin1Char('"'));
    // QString style = QStringLiteral("font-family:'%1'; font-size:%2pt; fill:%3; text-anchor:%4;")
    // Krimble 2026-10-19: George (1.0.39, test list, item 8): the Type tool "does not display emojis". The chosen font has no emoji, and the text
    // engine was given only that one family. The phone's color emoji fonts are now listed after it, as fallbacks for the characters it lacks
    // (Android's own, and Samsung's). Reading the style back (krimbleApplyTextProperty) still takes only the first family.
    QString style = QStringLiteral("font-family:'%1', 'Noto Color Emoji', 'Samsung Color Emoji'; font-size:%2pt; fill:%3; text-anchor:%4;")
                        .arg(family.toHtmlEscaped(), QString::number(s_size), s_color.name(), anchor);
    if (s_bold) style += QStringLiteral(" font-weight:bold;");
    if (s_italic) style += QStringLiteral(" font-style:italic;");
    if (qAbs(s_tracking) > 0.0001) style += QStringLiteral(" letter-spacing:%1;").arg(QString::number(s_tracking, 'f', 2));   // Krimble 2026-10-20: tracking
    // Krimble 2026-10-20: a text box: the text engine wraps the words at the box's width (SVG 2 inline-size), and the lines the user typed are kept
    // (white-space: pre-wrap), all in one piece of text instead of one tspan a line (separate tspans with their own x would not flow).
    if (s_boxWidth > 0.0) {
        style += QStringLiteral(" inline-size:%1; white-space:pre-wrap;").arg(QString::number(s_boxWidth, 'f', 2));
        // return QStringLiteral("<text style=\"%1\">%2</text>").arg(style, text.toHtmlEscaped());
        const QString dx = krimbleDxList(0, text.size());   // Krimble 2026-10-20: kerning
        const QString dxAttr = dx.isEmpty() ? QString() : QStringLiteral(" dx=\"%1\"").arg(dx);
        return QStringLiteral("<text style=\"%1\"%3>%2</text>").arg(style, text.toHtmlEscaped(), dxAttr);
    }
    QString svg = QStringLiteral("<text xml:space=\"preserve\" style=\"%1\">").arg(style);
    int lineStart = 0;   // Krimble 2026-10-20: where each line starts in the typed text (for the kerning)
    for (int i = 0; i < lines.size(); ++i) {
        const QString line = lines.at(i).isEmpty() ? QStringLiteral(" ") : lines.at(i).toHtmlEscaped();
        // Krimble 2026-10-20: the kerning of this line's characters (the first one of a line is not shifted)
        QString dx = lines.at(i).isEmpty() ? QString() : krimbleDxList(lineStart, lines.at(i).size());
        if (!dx.isEmpty()) dx = QStringLiteral("0") + dx.mid(dx.indexOf(QLatin1Char(' ')) < 0 ? dx.size() : dx.indexOf(QLatin1Char(' ')));
        const QString dxAttr = dx.isEmpty() || dx == QLatin1String("0") ? QString() : QStringLiteral(" dx=\"%1\"").arg(dx);
        if (i == 0) {
            // svg += QStringLiteral("<tspan>%1</tspan>").arg(line);
            svg += QStringLiteral("<tspan%2>%1</tspan>").arg(line, dxAttr);
        } else {
            // svg += QStringLiteral("<tspan x=\"0\" dy=\"1.2em\">%1</tspan>").arg(line);
            svg += QStringLiteral("<tspan x=\"0\" dy=\"1.2em\"%2>%1</tspan>").arg(line, dxAttr);
        }
        lineStart += lines.at(i).size() + 1;
    }
    svg += QStringLiteral("</text>");
    return svg;
}

// the text window; returns false when it is cancelled
static bool krimbleAskText(QWidget *parent, QString *text, bool editing = false)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(editing ? i18n("Edit Text") : i18n("Text"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // Krimble 2026-10-19: George (1.0.39, test list, item 8): "The new type tool should have a title at the top". On the phone a dialog has no
    // title bar, so the window's title is not seen; it is now also shown as a bold heading at the top of the window.
// Krimble 2026-10-20 (2nd): George: "Leave it out. It just sucks." The heading is not shown (the 203rd entry's lines, kept, not run).
    // QLabel *heading = new QLabel(dialog.windowTitle(), &dialog);
    // QFont headingFont = heading->font();
    // headingFont.setBold(true);
    // headingFont.setPointSizeF(headingFont.pointSizeF() * 1.3);
    // heading->setFont(headingFont);
    // heading->setAlignment(Qt::AlignHCenter);
    // layout->addWidget(heading);

    QPlainTextEdit *edit = new QPlainTextEdit(&dialog);
    edit->setPlainText(*text);
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    {
        // Krimble 2026-10-19: emoji show in the typing box too
        QFont editFont = edit->font();
        editFont.setFamilies(QStringList() << editFont.family() << QStringLiteral("Noto Color Emoji") << QStringLiteral("Samsung Color Emoji"));
        edit->setFont(editFont);
    }
#endif
    edit->setMinimumHeight(edit->fontMetrics().height() * 5);
    layout->addWidget(edit);

    // QFontComboBox *fontBox = new QFontComboBox(&dialog);
    // // Krimble 2026-10-19: George (1.0.39, test list, item 8): "There are still lots of foreign language Noto fonts" and "I'm starting to really hate
    // // Noto." The phone's own Noto families (one per script) are taken out of this list. They stay installed, so text in other scripts still draws
    // // (the text engine falls back to them); they are only not offered here.
    // if (QStringListModel *families = qobject_cast<QStringListModel*>(fontBox->model())) {
    //     QStringList kept;
    //     Q_FOREACH (const QString &family, families->stringList()) {
    //         if (!family.startsWith(QLatin1String("Noto"), Qt::CaseInsensitive)) kept << family;
    //     }
    //     families->setStringList(kept);
    // }
    // fontBox->setCurrentFont(QFont(s_family));
    // layout->addWidget(fontBox);
    // Krimble 2026-10-20: George (1.0.40): "There is also still a ton of Noto and SEC crapola in the fonts. Can we remove all of that or is it built in?"
    // They are the phone's own system fonts (Android's Noto, one per script; Samsung's SEC and Samsung fonts), not Krimble's. The filter above did
    // not hold: QFontComboBox rebuilds its list from the font database whenever it likes (setCurrentFont does it), so the Noto names came back. The
    // list is now a plain combo box filled once, here, with every family except the phone's script and system fonts (names starting with Noto, SEC,
    // New SEC, Samsung, AndroidClock); each name is drawn in its own font. The hidden fonts stay installed, so text in other scripts still draws.
    QComboBox *fontBox = new QComboBox(&dialog);
    {
        const QStringList hiddenPrefixes = {QStringLiteral("Noto"), QStringLiteral("SEC"), QStringLiteral("New SEC"),
                                            QStringLiteral("Samsung"), QStringLiteral("AndroidClock")};
        QStringList families = QFontDatabase().families();
        families.removeDuplicates();
        families.sort(Qt::CaseInsensitive);
        Q_FOREACH (const QString &family, families) {
            bool hidden = false;
            Q_FOREACH (const QString &prefix, hiddenPrefixes) {
                if (family.startsWith(prefix, Qt::CaseInsensitive)) { hidden = true; break; }
            }
            if (hidden) continue;
            fontBox->addItem(family);
            QFont itemFont(family);
            itemFont.setPointSizeF(fontBox->font().pointSizeF() * 1.15);
            fontBox->setItemData(fontBox->count() - 1, itemFont, Qt::FontRole);
        }
        int index = fontBox->findText(s_family, Qt::MatchFixedString);
        if (index < 0) index = fontBox->findText(QStringLiteral("sans-serif"), Qt::MatchFixedString);
        fontBox->setCurrentIndex(qMax(0, index));
    }
    layout->addWidget(fontBox);

    QHBoxLayout *row1 = new QHBoxLayout();
    QLabel *sizeLabel = new QLabel(i18n("Size (pt):"), &dialog);
    QDoubleSpinBox *sizeBox = new QDoubleSpinBox(&dialog);
    sizeBox->setRange(4.0, 600.0);
    sizeBox->setDecimals(1);
    sizeBox->setValue(s_size);
    QPushButton *boldButton = new QPushButton(i18n("B"), &dialog);
    boldButton->setCheckable(true);
    boldButton->setChecked(s_bold);
    QFont boldFont = boldButton->font();
    boldFont.setBold(true);
    boldButton->setFont(boldFont);
    QPushButton *italicButton = new QPushButton(i18n("I"), &dialog);
    italicButton->setCheckable(true);
    italicButton->setChecked(s_italic);
    QFont italicFont = italicButton->font();
    italicFont.setItalic(true);
    italicButton->setFont(italicFont);
    row1->addWidget(sizeLabel);
    row1->addWidget(sizeBox, 1);
    row1->addWidget(boldButton);
    row1->addWidget(italicButton);
    layout->addLayout(row1);

    QHBoxLayout *row2 = new QHBoxLayout();
    QButtonGroup *alignGroup = new QButtonGroup(&dialog);
    const QStringList alignNames = { i18n("Left"), i18n("Center"), i18n("Right") };
    for (int i = 0; i < 3; ++i) {
        QPushButton *button = new QPushButton(alignNames.at(i), &dialog);
        button->setCheckable(true);
        alignGroup->addButton(button, i);
        row2->addWidget(button);
        if (i == s_align) button->setChecked(true);
    }
    layout->addLayout(row2);

    // Krimble 2026-10-20: George (1.0.40): "I think we need kerning controls for the small adjustments between letters in a display typeface."
    // Spacing: tracking for the whole text, in pt. Kern: the gap before the letter at the typing cursor (put the cursor between two letters in the
    // box above), tighter or looser by 2% of the size a tap; Reset clears all kerning. The preview shows both.
    QVector<qreal> kern = s_kern;
    if (kern.size() < text->size()) kern.resize(text->size());
    QHBoxLayout *row3 = new QHBoxLayout();
    QLabel *trackLabel = new QLabel(i18n("Spacing (pt):"), &dialog);
    QDoubleSpinBox *trackBox = new QDoubleSpinBox(&dialog);
    trackBox->setRange(-50.0, 200.0);
    trackBox->setDecimals(1);
    trackBox->setSingleStep(0.5);
    trackBox->setValue(s_tracking);
    row3->addWidget(trackLabel);
    row3->addWidget(trackBox, 1);
    layout->addLayout(row3);
    QHBoxLayout *row4 = new QHBoxLayout();
    QLabel *kernLabel = new QLabel(&dialog);
    QPushButton *tighterButton = new QPushButton(i18n("◀ Tighter"), &dialog);
    QPushButton *looserButton = new QPushButton(i18n("Looser ▶"), &dialog);
    QPushButton *kernResetButton = new QPushButton(i18n("Reset"), &dialog);
    row4->addWidget(kernLabel, 1);
    row4->addWidget(tighterButton);
    row4->addWidget(looserButton);
    row4->addWidget(kernResetButton);
    layout->addLayout(row4);
    // the kerning follows the typing: characters typed or deleted move the kerning after them
    QObject::connect(edit->document(), &QTextDocument::contentsChange, &dialog, [&kern](int position, int removed, int added) {
        if (position < 0) return;
        if (position > kern.size()) kern.resize(position);
        kern.remove(position, qMin(removed, kern.size() - position));
        kern.insert(position, added, 0.0);
    });
    auto showKern = [&]() {
        const int at = edit->textCursor().position();
        const qreal v = (at >= 0 && at < kern.size()) ? kern.at(at) : 0.0;
        kernLabel->setText(i18n("Kern at cursor: %1 pt", QString::number(v, 'f', 1)));
    };
    QObject::connect(edit, &QPlainTextEdit::cursorPositionChanged, &dialog, [&]() { showKern(); });
    showKern();

    // Krimble 2026-10-18 (14th): George, asked what to do with fonts: "A preview of the text in the window." (see updatePreview below)
    std::function<void()> updatePreview;
    QColor chosenColor = s_color;
    QPushButton *colorButton = new QPushButton(i18n("Color..."), &dialog);
    auto paintColorButton = [colorButton, &chosenColor]() {
        colorButton->setStyleSheet(QStringLiteral("QPushButton { border-left: 24px solid %1; }").arg(chosenColor.name()));
    };
    paintColorButton();
    QObject::connect(colorButton, &QPushButton::clicked, &dialog, [&]() {
        const QColor picked = QColorDialog::getColor(chosenColor, &dialog, i18n("Text color"));
        if (picked.isValid()) {
            chosenColor = picked;
            paintColorButton();
            if (updatePreview) updatePreview();
        }
    });
    layout->addWidget(colorButton);

    // the preview: the words in the font, size (at most 40 pt, so that it fits), bold, italic, alignment and color that are chosen, changing as they are chosen. On a dark
    // background when the color is light, so that it can be seen.
    QLabel *preview = new QLabel(&dialog);
    preview->setTextFormat(Qt::PlainText);
    preview->setWordWrap(true);
    preview->setFrameShape(QFrame::Box);
    preview->setMinimumHeight(preview->fontMetrics().height() * 4);
    layout->addWidget(preview);
    updatePreview = [&]() {
        // QFont previewFont = fontBox->currentFont();
        QFont previewFont(fontBox->currentText());   // Krimble 2026-10-20: a plain combo box now
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
        // Krimble 2026-10-19: emoji show in the preview too (the same fallbacks as the text on the canvas)
        previewFont.setFamilies(QStringList() << previewFont.family() << QStringLiteral("Noto Color Emoji") << QStringLiteral("Samsung Color Emoji"));
#endif
        previewFont.setPointSizeF(qBound(8.0, sizeBox->value(), 40.0));
        previewFont.setBold(boldButton->isChecked());
        previewFont.setItalic(italicButton->isChecked());
        preview->setFont(previewFont);
        const QString background = chosenColor.lightness() > 190 ? QStringLiteral("#333344") : QStringLiteral("#ffffff");
        preview->setStyleSheet(QStringLiteral("QLabel { background: %1; color: %2; padding: 6px; }").arg(background, chosenColor.name()));
        const int alignId = qMax(0, alignGroup->checkedId());
        preview->setAlignment((alignId == 0 ? Qt::AlignLeft : (alignId == 1 ? Qt::AlignHCenter : Qt::AlignRight)) | Qt::AlignVCenter);
        const QString sample = edit->toPlainText();
        // preview->setText(sample.isEmpty() ? i18n("Sample text") : sample);
        // Krimble 2026-10-20: drawn by hand, so the tracking and the kerning show: each line is drawn in pieces between kerned letters (the font's own
        // kerning still applies inside a piece), with the tracking as the font's letter spacing, scaled to the preview's size.
        const QString shown = sample.isEmpty() ? i18n("Sample text") : sample;
        const qreal scale = previewFont.pointSizeF() / qMax(1.0, sizeBox->value());
        QFont drawFont = previewFont;
        drawFont.setLetterSpacing(QFont::AbsoluteSpacing, trackBox->value() * scale * preview->logicalDpiX() / 72.0);
        const QFontMetricsF fm(drawFont);
        const qreal toPx = scale * preview->logicalDpiX() / 72.0;
        const QStringList lines = shown.split(QLatin1Char('\n'));
        const int width = qMax(100, preview->width() - 12);
        const int height = int(fm.lineSpacing() * lines.size()) + 8;
        QPixmap pixmap(width, height);
        pixmap.fill(QColor(background));
        {
            QPainter p(&pixmap);
            p.setFont(drawFont);
            p.setPen(chosenColor);
            int lineStart = 0;
            for (int li = 0; li < lines.size(); ++li) {
                const QString &line = lines.at(li);
                // the pieces and the width of the line
                QList<QPair<QString, qreal>> pieces;   // text, gap before it (px)
                int pieceStart = 0;
                for (int ci = 1; ci <= line.size(); ++ci) {
                    const int pos = lineStart + ci;
                    const bool cut = ci == line.size() || (!sample.isEmpty() && pos < kern.size() && qAbs(kern.at(pos)) > 0.0001);
                    if (cut) {
                        const int gapPos = lineStart + pieceStart;
                        const qreal gap = (pieceStart > 0 && !sample.isEmpty() && gapPos < kern.size()) ? kern.at(gapPos) * toPx : 0.0;
                        pieces << qMakePair(line.mid(pieceStart, ci - pieceStart), gap);
                        pieceStart = ci;
                    }
                }
                qreal lineWidth = 0.0;
                for (const auto &piece : pieces) lineWidth += piece.second + fm.horizontalAdvance(piece.first);
                const int alignNow = qMax(0, alignGroup->checkedId());
                qreal x = alignNow == 0 ? 0.0 : (alignNow == 1 ? (width - lineWidth) / 2.0 : width - lineWidth);
                const qreal y = 4 + fm.ascent() + li * fm.lineSpacing();
                for (const auto &piece : pieces) {
                    x += piece.second;
                    p.drawText(QPointF(x, y), piece.first);
                    x += fm.horizontalAdvance(piece.first);
                }
                lineStart += line.size() + 1;
            }
        }
        preview->setPixmap(pixmap);
    };
    QObject::connect(edit, &QPlainTextEdit::textChanged, &dialog, [&]() { updatePreview(); });
    // QObject::connect(fontBox, &QFontComboBox::currentFontChanged, &dialog, [&]() { updatePreview(); });
    QObject::connect(fontBox, QOverload<int>::of(&QComboBox::currentIndexChanged), &dialog, [&]() { updatePreview(); });
    QObject::connect(sizeBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), &dialog, [&]() { updatePreview(); });
    QObject::connect(boldButton, &QPushButton::toggled, &dialog, [&]() { updatePreview(); });
    QObject::connect(italicButton, &QPushButton::toggled, &dialog, [&]() { updatePreview(); });
    QObject::connect(alignGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), &dialog, [&]() { updatePreview(); });
    // Krimble 2026-10-20: tracking and kerning
    QObject::connect(trackBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), &dialog, [&]() { updatePreview(); });
    auto nudge = [&](qreal direction) {
        const int at = edit->textCursor().position();
        if (at <= 0 || at >= edit->toPlainText().size()) return;   // between two letters only
        if (kern.size() <= at) kern.resize(at + 1);
        kern[at] += direction * sizeBox->value() * 0.02;
        showKern();
        updatePreview();
    };
    QObject::connect(tighterButton, &QPushButton::clicked, &dialog, [&]() { nudge(-1.0); edit->setFocus(); });
    QObject::connect(looserButton, &QPushButton::clicked, &dialog, [&]() { nudge(1.0); edit->setFocus(); });
    QObject::connect(kernResetButton, &QPushButton::clicked, &dialog, [&]() { kern.fill(0.0); showKern(); updatePreview(); });
    QObject::connect(edit, &QPlainTextEdit::textChanged, &dialog, [&]() { showKern(); });
    updatePreview();

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        dialog.resize(int(screen->availableGeometry().width() * 0.92), dialog.sizeHint().height());
    }
    edit->setFocus();

    if (dialog.exec() != QDialog::Accepted) return false;
    *text = edit->toPlainText();
    // s_family = fontBox->currentFont().family();
    s_family = fontBox->currentText();
    s_size = sizeBox->value();
    s_bold = boldButton->isChecked();
    s_italic = italicButton->isChecked();
    s_align = qMax(0, alignGroup->checkedId());
    s_color = chosenColor;
    s_tracking = trackBox->value();   // Krimble 2026-10-20
    s_kern = kern;
    s_kern.resize(text->size());
    return true;
}

// Krimble 2026-10-18 (7th): George: "Yes, continue work on the Type tool. It will be our showpiece in this app." Tapping existing text opens the same window with its words and
// look in it. What the shape says is read back from the SVG the program makes of it (the converter that made it), so text from other files can be edited too.
static void krimbleApplyTextProperty(const QString &name, const QString &rawValue)
{
    const QString value = rawValue.trimmed();
    if (name == QLatin1String("font-family")) {
        QString family = value.section(QLatin1Char(','), 0, 0).trimmed();
        family.remove(QLatin1Char('\'')).remove(QLatin1Char('"'));
        if (!family.isEmpty()) s_family = family;
    } else if (name == QLatin1String("font-size")) {
        bool ok = false;
        const qreal number = QString(value).remove(QRegularExpression(QStringLiteral("[a-zA-Z%]+$"))).toDouble(&ok);
        if (ok && number > 0) s_size = value.endsWith(QLatin1String("px")) ? number * 0.75 : number;
    } else if (name == QLatin1String("font-weight")) {
        s_bold = (value == QLatin1String("bold") || value.toInt() >= 600);
    } else if (name == QLatin1String("font-style")) {
        s_italic = (value == QLatin1String("italic") || value == QLatin1String("oblique"));
    } else if (name == QLatin1String("text-anchor")) {
        s_align = (value == QLatin1String("middle")) ? 1 : (value == QLatin1String("end") ? 2 : 0);
    } else if (name == QLatin1String("fill")) {
        const QColor color(value);
        if (color.isValid()) s_color = color;
    } else if (name == QLatin1String("letter-spacing")) {
        // Krimble 2026-10-20: tracking
        bool ok = false;
        const qreal number = QString(value).remove(QRegularExpression(QStringLiteral("[a-zA-Z%]+$"))).toDouble(&ok);
        s_tracking = ok ? number : 0.0;
    } else if (name == QLatin1String("inline-size")) {
        // Krimble 2026-10-20: the width of a text box (see s_boxWidth)
        bool ok = false;
        const qreal number = QString(value).remove(QRegularExpression(QStringLiteral("[a-zA-Z%]+$"))).toDouble(&ok);
        s_boxWidth = (ok && number > 0) ? number : 0.0;
    }
}

static bool krimbleReadTextShape(KoSvgTextShape *shape, QString *text)
{
    s_boxWidth = 0.0;   // Krimble 2026-10-20: set again below if this text is a box
    s_tracking = 0.0;   // Krimble 2026-10-20: and the tracking and kerning
    s_kern.clear();
    KoSvgTextShapeMarkupConverter converter(shape);
    QString svg;
    QString defs;
    if (!converter.convertToSvg(&svg, &defs)) return false;
    QXmlStreamReader xml(svg);
    QString result;
    bool inText = false;
    bool firstLine = true;
    int tspanDepth = 0;
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            // Krimble 2026-10-20: a dx list (kerning) on the text or a line: its values belong to the characters from here on
            if (xml.name() == QLatin1String("text") || xml.name() == QLatin1String("tspan")) {
                const QString dxText = xml.attributes().value(QStringLiteral("dx")).toString();
                if (!dxText.isEmpty()) {
                    int at = result.size();
                    if (xml.name() == QLatin1String("tspan") && inText && tspanDepth == 0 && !firstLine
                            && (xml.attributes().hasAttribute(QStringLiteral("x")) || xml.attributes().hasAttribute(QStringLiteral("dy")))) {
                        at += 1;   // the line break added just below comes first
                    }
                    const QStringList values = dxText.split(QRegularExpression(QStringLiteral("[\\s,]+")), QString::SkipEmptyParts);
                    if (s_kern.size() < at + values.size()) s_kern.resize(at + values.size());
                    for (int k = 0; k < values.size(); ++k) s_kern[at + k] = values.at(k).toDouble();
                }
            }
            if (xml.name() == QLatin1String("text") && !inText) {
                inText = true;
                const QXmlStreamAttributes attributes = xml.attributes();
                Q_FOREACH (const QXmlStreamAttribute &attribute, attributes) {
                    krimbleApplyTextProperty(attribute.name().toString(), attribute.value().toString());
                }
                const QString style = attributes.value(QStringLiteral("style")).toString();
                Q_FOREACH (const QString &declaration, style.split(QLatin1Char(';'), QString::SkipEmptyParts)) {
                    krimbleApplyTextProperty(declaration.section(QLatin1Char(':'), 0, 0).trimmed(), declaration.section(QLatin1Char(':'), 1));
                }
            } else if (xml.name() == QLatin1String("tspan") && inText) {
                if (tspanDepth == 0) {
                    const QXmlStreamAttributes attributes = xml.attributes();
                    if (!firstLine && (attributes.hasAttribute(QStringLiteral("x")) || attributes.hasAttribute(QStringLiteral("dy")))) {
                        result += QLatin1Char('\n');
                    }
                    firstLine = false;
                }
                ++tspanDepth;
            }
        } else if (xml.isEndElement()) {
            if (xml.name() == QLatin1String("tspan") && tspanDepth > 0) --tspanDepth;
            if (xml.name() == QLatin1String("text")) inText = false;
        } else if (xml.isCharacters() && inText && !xml.isWhitespace()) {
            result += xml.text().toString();
        }
    }
    *text = result;
    return !result.isEmpty();
}

KrimbleTextTool::KrimbleTextTool(KoCanvasBase *canvas)
    : KoToolBase(canvas)
{
}

// Krimble 2026-10-20: George (1.0.39, test list, item 9): "You can't make a paragraph area and have it flow into it." A tap still makes text where it
// was tapped (one line per typed line); a DRAG draws a text box (a dashed frame while dragging): the text then wraps at the box's width.
void KrimbleTextTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (!m_dragging) return;
    const QRectF viewRect = converter.documentToView(QRectF(m_pressPoint, m_dragPoint).normalized());
    painter.save();
    QPen pen(QColor(255, 140, 0));
    pen.setWidthF(2.0);
    pen.setStyle(Qt::DashLine);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(viewRect);
    painter.restore();
}

void KrimbleTextTool::mousePressEvent(KoPointerEvent *event)
{
    event->accept();
    m_pressPoint = event->point;
    m_dragPoint = event->point;
    m_dragging = false;
}

void KrimbleTextTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) return;
    const QRectF before = QRectF(m_pressPoint, m_dragPoint).normalized();
    m_dragPoint = event->point;
    const QPointF viewDelta = canvas()->viewConverter()->documentToView(m_dragPoint) - canvas()->viewConverter()->documentToView(m_pressPoint);
    if (!m_dragging && viewDelta.manhattanLength() > 24) m_dragging = true;   // a drag, not a shaky tap
    if (m_dragging) {
        canvas()->updateCanvas(before.united(QRectF(m_pressPoint, m_dragPoint).normalized()).adjusted(-20, -20, 20, 20));
    }
    event->accept();
}

void KrimbleTextTool::mouseReleaseEvent(KoPointerEvent *event)
{
    event->accept();
    QPointF where = event->point;
    QRectF box;
    if (m_dragging) {
        box = QRectF(m_pressPoint, event->point).normalized();
        canvas()->updateCanvas(box.adjusted(-20, -20, 20, 20));
        where = box.topLeft();
    }
    m_dragging = false;
    // the window opens after this event is finished
    // QTimer::singleShot(0, QApplication::instance(), [this, where]() { askAndAddText(where); });
    QTimer::singleShot(0, QApplication::instance(), [this, where, box]() { askAndAddText(where, box); });
}

void KrimbleTextTool::askAndAddText(const QPointF &where, const QRectF &box)
{
    // is there text where the finger was? (the boxes of the shapes, within about 14 view px of the tap)
    KoSvgTextShape *existing = nullptr;
    {
        const QPointF viewPoint = canvas()->viewConverter()->documentToView(where);
        const QRectF probe = canvas()->viewConverter()->viewToDocument(QRectF(viewPoint - QPointF(14, 14), QSizeF(28, 28)));
        const QList<KoShape*> shapes = canvas()->shapeManager()->shapesAt(probe);
        for (int i = shapes.size() - 1; i >= 0 && !existing; --i) {   // the top one first
            existing = dynamic_cast<KoSvgTextShape*>(shapes.at(i));
        }
    }
    QString text;
    if (existing && !krimbleReadTextShape(existing, &text)) existing = nullptr;
    // Krimble 2026-10-20: new text: a box if one was dragged (its width), else no box. Existing text keeps its own (read above).
    if (!existing) s_boxWidth = box.isValid() ? box.width() : 0.0;
    if (!krimbleAskText(QApplication::activeWindow(), &text, existing != nullptr)) return;
    if (text.trimmed().isEmpty()) return;

    if (existing) {
        // the words and the look are changed on the shape that is there (one undo step)
        // KUndo2Command *editCommand = new KUndo2Command(i18n("Edit text"));   // compile error (1.0.38 build): the name of an undo command is a KUndo2MagicString
        KUndo2Command *editCommand = new KUndo2Command(kundo2_i18n("Edit text"));
        new SvgTextChangeCommand(existing, krimbleTextSvg(text), QStringLiteral("<defs/>"), editCommand);
        new KoShapeBackgroundCommand(existing, QSharedPointer<KoShapeBackground>(new KoColorBackground(s_color)), editCommand);
        canvas()->addCommand(editCommand);
        return;
    }

    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(QStringLiteral("KoSvgTextShapeID"));
    if (!factory) return;

    KoProperties *params = new KoProperties();
    params->setProperty("svgText", krimbleTextSvg(text));
    params->setProperty("defs", QStringLiteral("<defs/>"));
    // params->setProperty("shapeRect", QVariant(QRectF(where, QSizeF(200, 60))));
    // params->setProperty("origin", QVariant(where));
    // Krimble 2026-10-20: in a box, the first line's baseline is one font size below the box's top, and the anchor point is the box's left, middle or
    // right edge (the text's alignment), so the text sits inside the box that was dragged.
    QPointF origin = where;
    QRectF shapeRect(where, QSizeF(200, 60));
    if (box.isValid() && s_boxWidth > 0.0) {
        const qreal anchorX = (s_align == 1) ? box.center().x() : (s_align == 2 ? box.right() : box.left());
        origin = QPointF(anchorX, box.top() + s_size);
        shapeRect = box;
    }
    params->setProperty("shapeRect", QVariant(shapeRect));
    params->setProperty("origin", QVariant(origin));

    KoShape *shape = factory->createShape(params, canvas()->shapeController()->resourceManager());
    if (!shape) return;
    // the fill is also set on the shape itself, as the old tool does (properties do not write their fills to the svg)
    shape->setBackground(QSharedPointer<KoShapeBackground>(new KoColorBackground(s_color)));

    KUndo2Command *parentCommand = new KUndo2Command();
    KoShapeContainer *parent = canvas()->shapeController()->documentBase()->createParentForShapes(QList<KoShape*>({shape}), false, parentCommand);
    KUndo2Command *cmd = canvas()->shapeController()->addShape(shape, parent, parentCommand);
    parentCommand->setText(cmd->text());
    canvas()->addCommand(parentCommand);
}

KrimbleTextToolFactory::KrimbleTextToolFactory()
    : KoToolFactoryBase("KrimbleTextTool")
{
    setToolTip(i18n("Type Tool"));
    setIconName(koIconNameCStr("draw-text"));
    setSection(ToolBoxSection::PSOrder);
    setPriority(1);
    setToolBoxPriority(170);
    setActivationShapeId(QStringLiteral("flake/always"));
}

KrimbleTextToolFactory::~KrimbleTextToolFactory()
{
}

KoToolBase *KrimbleTextToolFactory::createTool(KoCanvasBase *canvas)
{
    return new KrimbleTextTool(canvas);
}
