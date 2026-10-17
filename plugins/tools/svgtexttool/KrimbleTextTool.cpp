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
#include <KoIcon.h>
#include <klocalizedstring.h>
#include <kundo2command.h>

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
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>

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
static bool krimbleAskText(QWidget *parent, QString *text)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(i18n("Text"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

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
        }
    });
    layout->addWidget(colorButton);

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
    QString text;
    if (!krimbleAskText(QApplication::activeWindow(), &text)) return;
    if (text.trimmed().isEmpty()) return;

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
