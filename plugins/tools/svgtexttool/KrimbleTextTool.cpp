/*
 * Krimble 2026-10-18: see KrimbleTextTool.h.
 */
#include "KrimbleTextTool.h"
#include "KrimbleTextDialog.h"
#include <KoCanvasResourceProvider.h>
#include <KoColor.h>
#include <commands/KoShapeMoveCommand.h>
#include <KoSelection.h>
#include <KoShapeManager.h>

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
#include <QPainter>
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
#include <QFontMetricsF>
#include <QTextDocument>
#include <QPixmap>
#include <QFontDatabase>
#include <QComboBox>
#include <QStringListModel>
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
qreal s_boxWidth = 0.0;
qreal s_tracking = 0.0;
QVector<qreal> s_kern;
int s_align = 0;               // 0 left, 1 center, 2 right (3 justified: Krimble 2026-10-24)
bool s_underline = false;
bool s_strike = false;
int s_lineSpacing = 100;
QColor s_color = QColor(Qt::black);
}

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
    const QString anchor = (s_align == 0 || s_align == 3) ? QStringLiteral("start") : (s_align == 1 ? QStringLiteral("middle") : QStringLiteral("end"));
    const QString lineStepEm = QString::number(1.2 * qBound(50, s_lineSpacing, 400) / 100.0, 'f', 3);
    QString family = s_family;
    family.remove(QLatin1Char('\''));
    family.remove(QLatin1Char('"'));
    QString style = QStringLiteral("font-family:'%1', 'Noto Emoji', 'Noto Color Emoji', 'Samsung Color Emoji'; font-size:%2pt; fill:%3; text-anchor:%4;")
                        .arg(family.toHtmlEscaped(), QString::number(s_size), s_color.name(), anchor);
    if (s_bold) style += QStringLiteral(" font-weight:bold;");
    if (s_italic) style += QStringLiteral(" font-style:italic;");
    if (qAbs(s_tracking) > 0.0001) style += QStringLiteral(" letter-spacing:%1;").arg(QString::number(s_tracking, 'f', 2));
    if (s_underline || s_strike) {
        style += QStringLiteral(" text-decoration:%1%2%3;").arg(s_underline ? QStringLiteral("underline") : QString(), (s_underline && s_strike) ? QStringLiteral(" ") : QString(),
                                                              s_strike ? QStringLiteral("line-through") : QString());
    }
    if (s_boxWidth > 0.0) {
        style += QStringLiteral(" inline-size:%1; white-space:pre-wrap;").arg(QString::number(s_boxWidth, 'f', 2));
        if (s_lineSpacing != 100) style += QStringLiteral(" line-height:%1;").arg(lineStepEm);
        if (s_align == 3) style += QStringLiteral(" text-align:justify;");
        const QString dx = krimbleDxList(0, text.size());
        const QString dxAttr = dx.isEmpty() ? QString() : QStringLiteral(" dx=\"%1\"").arg(dx);
        return QStringLiteral("<text style=\"%1\"%3>%2</text>").arg(style, text.toHtmlEscaped(), dxAttr);
    }
    QString svg = QStringLiteral("<text xml:space=\"preserve\" style=\"%1\">").arg(style);
    int lineStart = 0;
    for (int i = 0; i < lines.size(); ++i) {
        const QString line = lines.at(i).isEmpty() ? QStringLiteral(" ") : lines.at(i).toHtmlEscaped();
        QString dx = lines.at(i).isEmpty() ? QString() : krimbleDxList(lineStart, lines.at(i).size());
        if (!dx.isEmpty()) dx = QStringLiteral("0") + dx.mid(dx.indexOf(QLatin1Char(' ')) < 0 ? dx.size() : dx.indexOf(QLatin1Char(' ')));
        const QString dxAttr = dx.isEmpty() || dx == QLatin1String("0") ? QString() : QStringLiteral(" dx=\"%1\"").arg(dx);
        if (i == 0) {
            // svg += QStringLiteral("<tspan>%1</tspan>").arg(line);
            svg += QStringLiteral("<tspan%2>%1</tspan>").arg(line, dxAttr);
        } else {
            svg += QStringLiteral("<tspan x=\"0\" dy=\"%3em\"%2>%1</tspan>").arg(line, dxAttr, lineStepEm);
        }
        lineStart += lines.at(i).size() + 1;
    }
    svg += QStringLiteral("</text>");
    return svg;
}

static bool krimbleAskText(QWidget *parent, QString *text, bool editing = false, const QColor &foreground = QColor())
{
    KrimbleTextStyle chosen;
    chosen.family = s_family;
    chosen.size = s_size;
    chosen.bold = s_bold;
    chosen.italic = s_italic;
    chosen.underline = s_underline;
    chosen.strike = s_strike;
    chosen.align = s_align;
    chosen.tracking = s_tracking;
    chosen.lineSpacing = s_lineSpacing;
    chosen.color = s_color;
    chosen.kern = s_kern;
    chosen.boxWidth = s_boxWidth;
    KrimbleTextDialog dialog(parent, *text, chosen, editing, foreground);
    if (dialog.exec() != QDialog::Accepted) return false;
    *text = dialog.text();
    chosen = dialog.style();
    s_family = chosen.family;
    s_size = chosen.size;
    s_bold = chosen.bold;
    s_italic = chosen.italic;
    s_underline = chosen.underline;
    s_strike = chosen.strike;
    s_align = chosen.align;
    s_tracking = chosen.tracking;
    s_lineSpacing = chosen.lineSpacing;
    s_color = chosen.color;
    s_kern = chosen.kern;
    s_kern.resize(text->size());
    s_boxWidth = chosen.boxWidth;
    return true;
}

#if 0
static bool krimbleAskText(QWidget *parent, QString *text, bool editing = false)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(editing ? i18n("Edit Text") : i18n("Text"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);


    QPlainTextEdit *edit = new QPlainTextEdit(&dialog);
    edit->setPlainText(*text);
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    {
        QFont editFont = edit->font();
        editFont.setFamilies(QStringList() << editFont.family() << QStringLiteral("Noto Color Emoji") << QStringLiteral("Samsung Color Emoji"));
        edit->setFont(editFont);
    }
#endif
    edit->setMinimumHeight(edit->fontMetrics().height() * 5);
    layout->addWidget(edit);

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
        QFont previewFont(fontBox->currentText());
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
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
    s_tracking = trackBox->value();
    s_kern = kern;
    s_kern.resize(text->size());
    return true;
}

#endif

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
    } else if (name == QLatin1String("text-align")) {
        if (value == QLatin1String("justify")) s_align = 3;
    } else if (name == QLatin1String("text-decoration") || name == QLatin1String("text-decoration-line")) {
        s_underline = value.contains(QLatin1String("underline"));
        s_strike = value.contains(QLatin1String("line-through"));
    } else if (name == QLatin1String("line-height")) {
        bool ok = false;
        const qreal number = QString(value).remove(QRegularExpression(QStringLiteral("[a-zA-Z%]+$"))).toDouble(&ok);
        if (ok && number > 0.0) s_lineSpacing = qBound(50, qRound(number / 1.2 * 100.0), 400);
    } else if (name == QLatin1String("fill")) {
        const QColor color(value);
        if (color.isValid()) s_color = color;
    } else if (name == QLatin1String("letter-spacing")) {
        bool ok = false;
        const qreal number = QString(value).remove(QRegularExpression(QStringLiteral("[a-zA-Z%]+$"))).toDouble(&ok);
        s_tracking = ok ? number : 0.0;
    } else if (name == QLatin1String("inline-size")) {
        bool ok = false;
        const qreal number = QString(value).remove(QRegularExpression(QStringLiteral("[a-zA-Z%]+$"))).toDouble(&ok);
        s_boxWidth = (ok && number > 0) ? number : 0.0;
    }
}

static bool krimbleReadTextShape(KoSvgTextShape *shape, QString *text)
{
    s_boxWidth = 0.0;
    s_tracking = 0.0;
    s_underline = false;
    s_strike = false;
    s_lineSpacing = 100;
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

void KrimbleTextTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (selectedStillThere()) {
        const QRectF frame = (m_handle != NoHandle) ? m_frameNow : selectedFrame();
        const QRectF viewFrame = converter.documentToView(frame).adjusted(-6, -6, 6, 6);
        painter.save();
        QPen light(QColor(255, 255, 255));
        light.setWidthF(3.0);
        light.setCosmetic(true);
        painter.setPen(light);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(viewFrame);
        QPen dark(QColor(30, 30, 40));
        dark.setWidthF(1.5);
        dark.setStyle(Qt::DashLine);
        dark.setCosmetic(true);
        painter.setPen(dark);
        painter.drawRect(viewFrame);
        const qreal half = 9.0;
        const QPointF centers[6] = {viewFrame.topLeft(), viewFrame.topRight(), viewFrame.bottomLeft(), viewFrame.bottomRight(),
                                    QPointF(viewFrame.left(), viewFrame.center().y()), QPointF(viewFrame.right(), viewFrame.center().y())};
        painter.setPen(QPen(QColor(30, 30, 40), 2.0));
        painter.setBrush(QColor(255, 255, 255));
        for (int i = 0; i < 6; ++i) {
            const QRectF handleRect(centers[i] - QPointF(half, half), QSizeF(2 * half, 2 * half));
            if (i < 4) painter.drawRect(handleRect);   // corners: size
            else painter.drawEllipse(handleRect);      // sides: width of the text box
        }
        painter.restore();
    }
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
    m_handle = NoHandle;
    if (selectedStillThere()) {
        const Handle hit = handleAt(event->point);
        if (hit != NoHandle) {
            m_handle = hit;
            m_frameStart = selectedFrame();
            m_frameNow = m_frameStart;
        }
    }
}

void KrimbleTextTool::mouseMoveEvent(KoPointerEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) return;
    if (m_handle != NoHandle) {
        const QRectF before = m_frameNow;
        m_frameNow = frameForDrag(m_handle, m_frameStart, event->point - m_pressPoint);
        repaintFrame(before.united(m_frameNow));
        event->accept();
        return;
    }
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
    if (m_handle != NoHandle) {
        const Handle handle = m_handle;
        const QRectF from = m_frameStart;
        const QRectF to = m_frameNow;
        const QPointF viewDelta = canvas()->viewConverter()->documentToView(event->point) - canvas()->viewConverter()->documentToView(m_pressPoint);
        m_handle = NoHandle;
        repaintFrame(from.united(to));
        if (viewDelta.manhattanLength() > 10) {
            applyFrameChange(handle, from, to);
        } else if (handle == MoveHandle) {
            // a tap inside the frame: the window opens, as it did before the frame was there
            const QPointF where = event->point;
            QTimer::singleShot(0, QApplication::instance(), [this, where]() { askAndAddText(where, QRectF()); });
        }
        return;
    }
    QPointF where = event->point;
    QRectF box;
    if (m_dragging) {
        box = QRectF(m_pressPoint, event->point).normalized();
        canvas()->updateCanvas(box.adjusted(-20, -20, 20, 20));
        where = box.topLeft();
    }
    m_dragging = false;
    // the window opens after this event is finished
    QTimer::singleShot(0, QApplication::instance(), [this, where, box]() { askAndAddText(where, box); });
}


void KrimbleTextTool::deactivate()
{
    if (m_selected) repaintFrame(selectedFrame());
    m_selected = nullptr;
    m_handle = NoHandle;
    KoToolBase::deactivate();
}

bool KrimbleTextTool::selectedStillThere() const
{
    // the shape may have been taken away (undo, a deleted layer) since it was chosen
    if (!m_selected) return false;
    return canvas()->shapeManager()->shapes().contains(static_cast<KoShape*>(m_selected));
}

QRectF KrimbleTextTool::selectedFrame() const
{
    return m_selected ? m_selected->boundingRect() : QRectF();
}

void KrimbleTextTool::repaintFrame(const QRectF &frame)
{
    if (!canvas()) return;
    // a margin of 40 view pixels, for the handles and the line
    const QRectF viewRect = canvas()->viewConverter()->documentToView(frame).adjusted(-40, -40, 40, 40);
    canvas()->updateCanvas(canvas()->viewConverter()->viewToDocument(viewRect));
}

// which handle (or the inside of the frame) is at the point; the handles are tested in view pixels so that a finger can hit them
KrimbleTextTool::Handle KrimbleTextTool::handleAt(const QPointF &documentPoint) const
{
    const QRectF frame = canvas()->viewConverter()->documentToView(selectedFrame()).adjusted(-6, -6, 6, 6);
    const QPointF p = canvas()->viewConverter()->documentToView(documentPoint);
    const qreal reach = 34.0;
    struct Candidate { QPointF at; Handle handle; };
    const Candidate candidates[6] = {
        {frame.topLeft(), TopLeftHandle}, {frame.topRight(), TopRightHandle}, {frame.bottomLeft(), BottomLeftHandle}, {frame.bottomRight(), BottomRightHandle},
        {QPointF(frame.left(), frame.center().y()), LeftHandle}, {QPointF(frame.right(), frame.center().y()), RightHandle}};
    Handle best = NoHandle;
    qreal bestDistance = reach;
    for (int i = 0; i < 6; ++i) {
        const qreal d = QLineF(p, candidates[i].at).length();
        if (d < bestDistance) { bestDistance = d; best = candidates[i].handle; }
    }
    if (best != NoHandle) return best;
    if (frame.contains(p)) return MoveHandle;
    return NoHandle;
}

// the frame while a handle is dragged by delta (document units)
QRectF KrimbleTextTool::frameForDrag(Handle handle, const QRectF &start, const QPointF &delta) const
{
    const qreal minWidth = 20.0;
    QRectF r = start;
    switch (handle) {
    case MoveHandle:
        r.translate(delta);
        break;
    case LeftHandle:
        r.setLeft(qMin(start.left() + delta.x(), start.right() - minWidth));
        break;
    case RightHandle:
        r.setRight(qMax(start.right() + delta.x(), start.left() + minWidth));
        break;
    default: {
        // a corner: the text is made bigger or smaller, in proportion; the opposite corner stays
        const bool left = (handle == TopLeftHandle || handle == BottomLeftHandle);
        const bool top = (handle == TopLeftHandle || handle == TopRightHandle);
        const qreal width = qMax(minWidth, start.width() + (left ? -delta.x() : delta.x()));
        const qreal scale = width / qMax(1.0, start.width());
        const qreal height = start.height() * scale;
        const QPointF anchor(left ? start.right() : start.left(), top ? start.bottom() : start.top());
        r = QRectF(QPointF(left ? anchor.x() - width : anchor.x(), top ? anchor.y() - height : anchor.y()), QSizeF(width, height));
        break;
    }
    }
    return r;
}

// the text is changed to fit the frame that was dragged
void KrimbleTextTool::applyFrameChange(Handle handle, const QRectF &from, const QRectF &to)
{
    KoSvgTextShape *shape = m_selected;
    if (!shape || !selectedStillThere()) return;
    if (handle == MoveHandle) {
        const QPointF offset = to.topLeft() - from.topLeft();
        if (offset.manhattanLength() < 0.01) return;
        KUndo2Command *move = new KoShapeMoveCommand(QList<KoShape*>({shape}), offset);
        move->setText(kundo2_i18n("Move text"));
        canvas()->addCommand(move);
        repaintFrame(from.united(to));
        return;
    }
    // the words and the look of the text as it is now (this sets what was chosen last time, s_family and the others, to the text's own)
    QString text;
    if (!krimbleReadTextShape(shape, &text)) return;
    // the fill is not written into the svg: it is the shape's own
    QSharedPointer<KoColorBackground> fill = qSharedPointerDynamicCast<KoColorBackground>(shape->background());
    if (fill) s_color = fill->color();
    if (s_kern.size() < text.size()) s_kern.resize(text.size());
    if (handle == LeftHandle || handle == RightHandle) {
        s_boxWidth = qMax(20.0, to.width());   // a text box of this width (text that had no box gets one)
    } else {
        const qreal ratio = to.width() / qMax(1.0, from.width());
        s_size = qBound(4.0, s_size * ratio, 600.0);
        if (s_boxWidth > 0.0) s_boxWidth = qMax(20.0, s_boxWidth * ratio);
        s_tracking *= ratio;
        for (int i = 0; i < s_kern.size(); ++i) s_kern[i] *= ratio;
    }
    KUndo2Command *change = new KUndo2Command(kundo2_i18n("Resize text"));
    new SvgTextChangeCommand(shape, krimbleTextSvg(text), QStringLiteral("<defs/>"), change);
    canvas()->addCommand(change);
    // the edge or corner opposite to the handle stays where it was: the text is moved by what it moved
    const QRectF now = shape->boundingRect();
    const bool left = (handle == LeftHandle || handle == TopLeftHandle || handle == BottomLeftHandle);
    const bool top = (handle == TopLeftHandle || handle == TopRightHandle);
    const QPointF shift((left ? from.right() - now.right() : from.left() - now.left()), (top ? from.bottom() - now.bottom() : from.top() - now.top()));
    if (shift.manhattanLength() > 0.01) {
        KUndo2Command *move = new KoShapeMoveCommand(QList<KoShape*>({shape}), shift);
        move->setText(kundo2_i18n("Resize text"));
        canvas()->addCommand(move);
    }
    repaintFrame(from.united(to).united(shape->boundingRect()));
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
    if (m_selected && selectedStillThere()) repaintFrame(selectedFrame());
    m_selected = existing;
    if (m_selected) repaintFrame(selectedFrame());
    if (!existing) s_boxWidth = box.isValid() ? box.width() : 0.0;
    if (!krimbleAskText(QApplication::activeWindow(), &text, existing != nullptr, canvas()->resourceManager()->foregroundColor().toQColor())) return;
    if (text.trimmed().isEmpty()) return;

    if (existing) {
        // the words and the look are changed on the shape that is there (one undo step)
        // KUndo2Command *editCommand = new KUndo2Command(i18n("Edit text"));   // compile error (1.0.38 build): the name of an undo command is a KUndo2MagicString
        KUndo2Command *editCommand = new KUndo2Command(kundo2_i18n("Edit text"));
        new SvgTextChangeCommand(existing, krimbleTextSvg(text), QStringLiteral("<defs/>"), editCommand);
        new KoShapeBackgroundCommand(existing, QSharedPointer<KoShapeBackground>(new KoColorBackground(s_color)), editCommand);
        canvas()->addCommand(editCommand);
        repaintFrame(existing->boundingRect());
        return;
    }

    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(QStringLiteral("KoSvgTextShapeID"));
    if (!factory) return;

    KoProperties *params = new KoProperties();
    params->setProperty("svgText", krimbleTextSvg(text));
    params->setProperty("defs", QStringLiteral("<defs/>"));
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
    KoShapeContainer *parent = canvas()->shapeController()->documentBase()->createParentForShapes(QList<KoShape*>({shape}), true, parentCommand);
    KUndo2Command *cmd = canvas()->shapeController()->addShape(shape, parent, parentCommand);
    parentCommand->setText(cmd->text());
    canvas()->addCommand(parentCommand);
    if (KoSelection *selection = canvas()->shapeManager()->selection()) {
        selection->deselectAll();
        selection->select(shape);
    }
    m_selected = dynamic_cast<KoSvgTextShape*>(shape);
    if (m_selected) repaintFrame(m_selected->boundingRect());
}

KrimbleTextToolFactory::KrimbleTextToolFactory()
    : KoToolFactoryBase("KrimbleTextTool")
{
    setToolTip(i18n("Krimble Type"));
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
