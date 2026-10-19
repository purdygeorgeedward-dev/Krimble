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
int s_align = 0;               // 0 left, 1 center, 2 right
QColor s_color = QColor(Qt::black);
}

static QString krimbleTextSvg(const QString &text)
{
    const QStringList lines = text.split(QLatin1Char('\n'));
    const QString anchor = (s_align == 0) ? QStringLiteral("start") : (s_align == 1 ? QStringLiteral("middle") : QStringLiteral("end"));
    QString family = s_family;
    family.remove(QLatin1Char('\''));
    family.remove(QLatin1Char('"'));
    QString style = QStringLiteral("font-family:'%1'; font-size:%2pt; fill:%3; text-anchor:%4;")
                        .arg(family.toHtmlEscaped(), QString::number(s_size), s_color.name(), anchor);
    if (s_bold) style += QStringLiteral(" font-weight:bold;");
    if (s_italic) style += QStringLiteral(" font-style:italic;");
    QString svg = QStringLiteral("<text xml:space=\"preserve\" style=\"%1\">").arg(style);
    for (int i = 0; i < lines.size(); ++i) {
        const QString line = lines.at(i).isEmpty() ? QStringLiteral(" ") : lines.at(i).toHtmlEscaped();
        if (i == 0) {
            svg += QStringLiteral("<tspan>%1</tspan>").arg(line);
        } else {
            svg += QStringLiteral("<tspan x=\"0\" dy=\"1.2em\">%1</tspan>").arg(line);
        }
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
    QLabel *heading = new QLabel(dialog.windowTitle(), &dialog);
    QFont headingFont = heading->font();
    headingFont.setBold(true);
    headingFont.setPointSizeF(headingFont.pointSizeF() * 1.3);
    heading->setFont(headingFont);
    heading->setAlignment(Qt::AlignHCenter);
    layout->addWidget(heading);

    QPlainTextEdit *edit = new QPlainTextEdit(&dialog);
    edit->setPlainText(*text);
    edit->setMinimumHeight(edit->fontMetrics().height() * 5);
    layout->addWidget(edit);

    QFontComboBox *fontBox = new QFontComboBox(&dialog);
    fontBox->setCurrentFont(QFont(s_family));
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
        QFont previewFont = fontBox->currentFont();
        previewFont.setPointSizeF(qBound(8.0, sizeBox->value(), 40.0));
        previewFont.setBold(boldButton->isChecked());
        previewFont.setItalic(italicButton->isChecked());
        preview->setFont(previewFont);
        const QString background = chosenColor.lightness() > 190 ? QStringLiteral("#333344") : QStringLiteral("#ffffff");
        preview->setStyleSheet(QStringLiteral("QLabel { background: %1; color: %2; padding: 6px; }").arg(background, chosenColor.name()));
        const int alignId = qMax(0, alignGroup->checkedId());
        preview->setAlignment((alignId == 0 ? Qt::AlignLeft : (alignId == 1 ? Qt::AlignHCenter : Qt::AlignRight)) | Qt::AlignVCenter);
        const QString sample = edit->toPlainText();
        preview->setText(sample.isEmpty() ? i18n("Sample text") : sample);
    };
    QObject::connect(edit, &QPlainTextEdit::textChanged, &dialog, [&]() { updatePreview(); });
    QObject::connect(fontBox, &QFontComboBox::currentFontChanged, &dialog, [&]() { updatePreview(); });
    QObject::connect(sizeBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), &dialog, [&]() { updatePreview(); });
    QObject::connect(boldButton, &QPushButton::toggled, &dialog, [&]() { updatePreview(); });
    QObject::connect(italicButton, &QPushButton::toggled, &dialog, [&]() { updatePreview(); });
    QObject::connect(alignGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), &dialog, [&]() { updatePreview(); });
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
    s_family = fontBox->currentFont().family();
    s_size = sizeBox->value();
    s_bold = boldButton->isChecked();
    s_italic = italicButton->isChecked();
    s_align = qMax(0, alignGroup->checkedId());
    s_color = chosenColor;
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
    }
}

static bool krimbleReadTextShape(KoSvgTextShape *shape, QString *text)
{
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
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void KrimbleTextTool::mousePressEvent(KoPointerEvent *event)
{
    event->accept();
}

void KrimbleTextTool::mouseMoveEvent(KoPointerEvent *event)
{
    Q_UNUSED(event);
}

void KrimbleTextTool::mouseReleaseEvent(KoPointerEvent *event)
{
    event->accept();
    const QPointF where = event->point;
    // the window opens after this event is finished
    QTimer::singleShot(0, QApplication::instance(), [this, where]() { askAndAddText(where); });
}

void KrimbleTextTool::askAndAddText(const QPointF &where)
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
    params->setProperty("shapeRect", QVariant(QRectF(where, QSizeF(200, 60))));
    params->setProperty("origin", QVariant(where));

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
