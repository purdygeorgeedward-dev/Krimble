/*
 * Krimble 2026-10-24: see KrimbleTextDialog.h.
 */
#include "KrimbleTextDialog.h"

#include <klocalizedstring.h>

#include <functional>

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFontDatabase>
#include <QFontMetricsF>
#include <QFrame>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPaintEvent>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QScreen>
#include <QScrollArea>
#include <QSettings>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextCursor>
#include <QTextDocument>
#include <QToolButton>
#include <QVBoxLayout>

namespace {

// four lines of text, left, centered, right or both edges
QIcon alignIcon(int kind, const QColor &pen)
{
    QPixmap pm(64, 64);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(pen, 4.5, Qt::SolidLine, Qt::RoundCap));
    const int full = 44;
    const int widths[4] = {full, 30, full, 22};
    for (int i = 0; i < 4; ++i) {
        const int w = (kind == 3) ? full : widths[i];
        int x0 = 10;
        if (kind == 1) x0 = 10 + (full - w) / 2;
        else if (kind == 2) x0 = 10 + (full - w);
        const int y = 14 + i * 11;
        p.drawLine(x0, y, x0 + w, y);
    }
    return QIcon(pm);
}

// an eyedropper, drawn
QIcon dropperIcon(const QColor &pen)
{
    QPixmap pm(64, 64);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(pen, 5, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(16, 48, 36, 28);
    p.drawLine(30, 22, 42, 34);
    p.setPen(QPen(pen, 6, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(38, 26, 48, 16);
    p.drawLine(44, 12, 52, 20);
    p.setPen(QPen(pen, 3, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(12, 52, 16, 48);
    return QIcon(pm);
}

QIcon swatchIcon(const QColor &color, const QSize &size)
{
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(QColor(120, 120, 130), 1.5));
    p.setBrush(color);
    p.drawRoundedRect(QRectF(1, 1, size.width() - 2, size.height() - 2), 4, 4);
    return QIcon(pm);
}

// the preview: a widget that paints itself, so that it always uses its present width
class KrimblePreviewWidget : public QWidget
{
public:
    explicit KrimblePreviewWidget(QWidget *parent) : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    std::function<void(QPainter &, int, int)> painter;   // painter, width, height
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        if (painter) painter(p, width(), height());
        p.setPen(QPen(palette().color(QPalette::Mid), 1));
        p.setBrush(Qt::NoBrush);
        p.drawRect(rect().adjusted(0, 0, -1, -1));
    }
};

QFrame *separator(QWidget *parent)
{
    QFrame *line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

QToolButton *toggleButton(const QString &text, QWidget *parent)
{
    QToolButton *button = new QToolButton(parent);
    button->setText(text);
    button->setCheckable(true);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setMinimumHeight(int(button->fontMetrics().height() * 2.0));
    return button;
}

QStringList colorsFromSettings()
{
    QSettings settings(QStringLiteral("Krimble"), QStringLiteral("TypeTool"));
    return settings.value(QStringLiteral("recentColors")).toStringList();
}

} // namespace

KrimbleTextDialog::KrimbleTextDialog(QWidget *parent, const QString &text, const KrimbleTextStyle &style, bool editing, const QColor &foreground)
    : QDialog(parent)
    , m_initial(style)
    , m_color(style.color)
    , m_foreground(foreground)
    , m_kern(style.kern)
{
    build(text, editing);
    applyStyleToControls(style);
    if (m_kern.size() < text.size()) m_kern.resize(text.size());
    updateKernDisplay();
    updatePreview();
}

void KrimbleTextDialog::build(const QString &text, bool editing)
{
    setWindowTitle(editing ? i18n("Edit Text") : i18n("Text Tool"));
    // a phone held sideways is only about 340 px high: the Text tab is then in two columns (the words and the preview on the left, the controls on the right)
    bool landscape = false;
    if (QScreen *screen = QGuiApplication::primaryScreen()) landscape = screen->availableGeometry().width() > screen->availableGeometry().height();
    if (qEnvironmentVariableIsSet("KRIMBLE_TEXTDLG_ORIENTATION")) landscape = qgetenv("KRIMBLE_TEXTDLG_ORIENTATION") == "landscape";   // for the bench
    m_landscape = landscape;
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(6, 6, 6, 6);
    root->setSpacing(6);

    // the title: a T badge, the name, a close button
    {
        QHBoxLayout *header = new QHBoxLayout();
        QLabel *badge = new QLabel(QStringLiteral("T"), this);
        badge->setAlignment(Qt::AlignCenter);
        const int side = int(fontMetrics().height() * 2.1);
        badge->setFixedSize(side, side);
        badge->setStyleSheet(QStringLiteral("QLabel { border: 1px solid %1; border-radius: 5px; background: %2; color: %3; font-weight: bold; font-family: serif; font-size: %4px; }")
                                 .arg(palette().color(QPalette::Highlight).lighter(130).name(), palette().color(QPalette::Highlight).darker(110).name(),
                                      palette().color(QPalette::HighlightedText).name(), QString::number(int(side * 0.62))));
        QLabel *title = new QLabel(windowTitle(), this);
        QFont titleFont = title->font();
        titleFont.setBold(true);
        titleFont.setPointSizeF(titleFont.pointSizeF() * 1.35);
        title->setFont(titleFont);
        QToolButton *close = new QToolButton(this);
        close->setText(QStringLiteral("\u00D7"));
        close->setAutoRaise(true);
        QFont closeFont = close->font();
        closeFont.setPointSizeF(closeFont.pointSizeF() * 1.8);
        close->setFont(closeFont);
        connect(close, &QToolButton::clicked, this, &QDialog::reject);
        header->addWidget(badge);
        header->addWidget(title, 1);
        header->addWidget(close);
        if (landscape) {
            // sideways there is no room for the title row (the Cancel button closes the window)
            badge->hide();
            title->hide();
            close->hide();
        } else {
            root->addLayout(header);
        }
    }

    m_tabs = new QTabWidget(this);
    root->addWidget(m_tabs, 1);
    // every page of the tabs scrolls, so that the window also fits a phone held sideways (about 340 px high)
    auto addPage = [this](QWidget *page, const QString &title) {
        QScrollArea *area = new QScrollArea(m_tabs);
        area->setWidgetResizable(true);
        area->setFrameShape(QFrame::NoFrame);
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setWidget(page);
        m_tabs->addTab(area, title);
    };

    // ---- the Text tab: everything for ordinary use, on one page ----
    QWidget *textPage = new QWidget(m_tabs);
    QVBoxLayout *tp = new QVBoxLayout(textPage);
    tp->setContentsMargins(4, 6, 4, 4);
    tp->setSpacing(5);

    m_edit = new QPlainTextEdit(textPage);
    m_edit->setPlainText(text);
    // (1.0.42: the typing box was given the color emoji fonts as extra families, and the space between words came out with the wide space of one of them: "Jezzy     english".
    // The box now uses the font as it is; the system's own fallback draws an emoji.)
    m_edit->setMinimumHeight(m_edit->fontMetrics().height() * 4);
    // tp->addWidget(m_edit);
    // tp->addWidget(separator(textPage));
    // (the words, the controls and the preview are put in the page after they are made, below)
    QWidget *controls = new QWidget(textPage);
    QVBoxLayout *cl = new QVBoxLayout(controls);
    cl->setContentsMargins(0, 0, 0, 0);
    cl->setSpacing(5);

    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(6);
    grid->setVerticalSpacing(6);
    int row = 0;

    // Font + favorite star
    m_fontBox = new QComboBox(textPage);
    {
        // every family except the phone's own script and system fonts (as before: Noto, SEC, New SEC, Samsung, AndroidClock); each drawn in its own font
        const QStringList hiddenPrefixes = {QStringLiteral("Noto"), QStringLiteral("SEC"), QStringLiteral("New SEC"), QStringLiteral("Samsung"), QStringLiteral("AndroidClock")};
        QStringList families = QFontDatabase().families();
        families.removeDuplicates();
        families.sort(Qt::CaseInsensitive);
        Q_FOREACH (const QString &family, families) {
            bool hidden = false;
            Q_FOREACH (const QString &prefix, hiddenPrefixes) {
                if (family.startsWith(prefix, Qt::CaseInsensitive)) { hidden = true; break; }
            }
            if (hidden) continue;
            m_fontBox->addItem(family);
            QFont itemFont(family);
            itemFont.setPointSizeF(m_fontBox->font().pointSizeF() * 1.15);
            m_fontBox->setItemData(m_fontBox->count() - 1, itemFont, Qt::FontRole);
        }
    }
    m_fontBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);   // not as wide as the longest family name
    m_fontBox->setMinimumContentsLength(10);
    m_starButton = new QToolButton(textPage);
    m_starButton->setText(QStringLiteral("\u2605"));
    m_starButton->setCheckable(true);
    m_starButton->setToolTip(i18n("Favorite font"));
    m_starButton->setMinimumHeight(int(m_starButton->fontMetrics().height() * 2.0));
    grid->addWidget(new QLabel(i18n("Font"), textPage), row, 0);
    grid->addWidget(m_fontBox, row, 1, 1, 3);
    grid->addWidget(m_starButton, row, 4);
    ++row;

    // Size, Color (swatch, recent colors in its menu), eyedropper (the foreground color)
    m_sizeBox = new QDoubleSpinBox(textPage);
    m_sizeBox->setRange(4.0, 600.0);
    m_sizeBox->setDecimals(1);
    m_sizeBox->setSuffix(i18n(" pt"));
    m_colorButton = new QToolButton(textPage);
    m_colorButton->setPopupMode(QToolButton::MenuButtonPopup);
    m_colorButton->setIconSize(QSize(int(fontMetrics().height() * 2.6), int(fontMetrics().height() * 1.2)));
    m_colorButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_colorButton->setMinimumHeight(int(m_colorButton->fontMetrics().height() * 2.0));
    m_colorButton->setMenu(new QMenu(m_colorButton));
    m_dropperButton = new QToolButton(textPage);
    m_dropperButton->setIcon(dropperIcon(palette().color(QPalette::ButtonText)));
    m_dropperButton->setIconSize(QSize(int(fontMetrics().height() * 1.4), int(fontMetrics().height() * 1.4)));
    m_dropperButton->setToolTip(i18n("Use the foreground color"));
    m_dropperButton->setEnabled(m_foreground.isValid());
    m_dropperButton->setMinimumHeight(int(m_dropperButton->fontMetrics().height() * 2.0));
    // grid->addWidget(new QLabel(i18n("Size"), textPage), row, 0);
    // grid->addWidget(m_sizeBox, row, 1);
    // grid->addWidget(new QLabel(i18n("Color"), textPage), row, 2);
    // grid->addWidget(m_colorButton, row, 3);
    // grid->addWidget(m_dropperButton, row, 4);
    // On the phone (about 340 px wide) Size and Color in one row made the window wider than the screen; they are two rows (George's picture has them in one).
    grid->addWidget(new QLabel(i18n("Size"), textPage), row, 0);
    grid->addWidget(m_sizeBox, row, 1, 1, 4);
    ++row;
    grid->addWidget(new QLabel(i18n("Color"), textPage), row, 0);
    grid->addWidget(m_colorButton, row, 1, 1, 3);
    grid->addWidget(m_dropperButton, row, 4);
    ++row;
    cl->addLayout(grid);

    // B I U S Aa
    {
        QHBoxLayout *styles = new QHBoxLayout();
        styles->setSpacing(6);
        m_boldButton = toggleButton(QStringLiteral("B"), textPage);
        QFont f = m_boldButton->font(); f.setBold(true); m_boldButton->setFont(f);
        m_italicButton = toggleButton(QStringLiteral("I"), textPage);
        f = m_italicButton->font(); f.setItalic(true); m_italicButton->setFont(f);
        m_underlineButton = toggleButton(QStringLiteral("U"), textPage);
        f = m_underlineButton->font(); f.setUnderline(true); m_underlineButton->setFont(f);
        m_strikeButton = toggleButton(QStringLiteral("S"), textPage);
        f = m_strikeButton->font(); f.setStrikeOut(true); m_strikeButton->setFont(f);
        m_caseButton = new QToolButton(textPage);
        m_caseButton->setText(QStringLiteral("Aa"));
        m_caseButton->setPopupMode(QToolButton::InstantPopup);
        m_caseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_caseButton->setMinimumHeight(int(m_caseButton->fontMetrics().height() * 2.0));
        QMenu *caseMenu = new QMenu(m_caseButton);
        const QStringList caseNames = {i18n("UPPERCASE"), i18n("lowercase"), i18n("Title Case"), i18n("Sentence case")};
        for (int i = 0; i < caseNames.size(); ++i) {
            QAction *action = caseMenu->addAction(caseNames.at(i));
            connect(action, &QAction::triggered, this, [this, i]() { applyCase(i); });
        }
        m_caseButton->setMenu(caseMenu);
        styles->addWidget(m_boldButton);
        styles->addWidget(m_italicButton);
        styles->addWidget(m_underlineButton);
        styles->addWidget(m_strikeButton);
        styles->addWidget(m_caseButton);
        cl->addLayout(styles);
    }
    cl->addWidget(separator(textPage));

    // Alignment
    {
        QHBoxLayout *aligns = new QHBoxLayout();
        aligns->setSpacing(4);
        aligns->addWidget(new QLabel(i18n("Alignment"), textPage));
        m_alignGroup = new QButtonGroup(this);
        const QStringList alignTips = {i18n("Left"), i18n("Center"), i18n("Right"), i18n("Justify")};
        const QColor pen = palette().color(QPalette::ButtonText);
        for (int i = 0; i < 4; ++i) {
            QToolButton *button = new QToolButton(textPage);
            button->setCheckable(true);
            button->setIcon(alignIcon(i, pen));
            button->setIconSize(QSize(int(fontMetrics().height() * 1.5), int(fontMetrics().height() * 1.5)));
            button->setToolTip(alignTips.at(i));
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            button->setMinimumHeight(int(button->fontMetrics().height() * 2.2));
            m_alignGroup->addButton(button, i);
            aligns->addWidget(button, 1);
        }
        cl->addLayout(aligns);
    }
    cl->addWidget(separator(textPage));

    // Spacing: letters, lines; Kerning at the cursor
    {
        QGridLayout *spacing = new QGridLayout();
        spacing->setHorizontalSpacing(6);
        spacing->setVerticalSpacing(6);
        m_trackBox = new QDoubleSpinBox(textPage);
        m_trackBox->setRange(-50.0, 200.0);
        m_trackBox->setDecimals(1);
        m_trackBox->setSingleStep(0.5);
        m_trackBox->setSuffix(i18n(" pt"));
        m_lineBox = new QSpinBox(textPage);
        m_lineBox->setRange(50, 400);
        m_lineBox->setSingleStep(5);
        m_lineBox->setSuffix(QStringLiteral(" %"));
        m_kernBox = new QDoubleSpinBox(textPage);
        m_kernBox->setRange(-100.0, 100.0);
        m_kernBox->setDecimals(1);
        m_kernBox->setSingleStep(0.5);
        m_kernBox->setSuffix(i18n(" pt"));
        QPushButton *tighter = new QPushButton(i18n("\u25C0 Tighter"), textPage);
        QPushButton *looser = new QPushButton(i18n("Looser \u25B6"), textPage);
        tighter->setMinimumHeight(int(tighter->fontMetrics().height() * 2.0));
        looser->setMinimumHeight(int(looser->fontMetrics().height() * 2.0));
        QLabel *spacingTitle = new QLabel(i18n("Spacing"), textPage);
        spacing->addWidget(spacingTitle, 0, 0, 1, 4);
        spacing->addWidget(new QLabel(i18n("Letter Spacing"), textPage), 1, 0, 1, 2);
        spacing->addWidget(m_trackBox, 1, 2, 1, 2);
        spacing->addWidget(new QLabel(i18n("Line Spacing"), textPage), 2, 0, 1, 2);
        spacing->addWidget(m_lineBox, 2, 2, 1, 2);
        spacing->addWidget(new QLabel(i18n("Kerning at Cursor"), textPage), 3, 0, 1, 2);
        spacing->addWidget(m_kernBox, 3, 2, 1, 2);
        spacing->addWidget(tighter, 4, 0, 1, 2);
        spacing->addWidget(looser, 4, 2, 1, 2);
        cl->addLayout(spacing);
        connect(tighter, &QPushButton::clicked, this, [this]() { nudgeKern(-1.0); m_edit->setFocus(); });
        connect(looser, &QPushButton::clicked, this, [this]() { nudgeKern(1.0); m_edit->setFocus(); });
    }

    // the preview
    m_preview = new KrimblePreviewWidget(textPage);
    m_preview->setFixedHeight(m_preview->fontMetrics().height() * 4);

    if (!landscape) {
        tp->addWidget(m_edit);
        tp->addWidget(separator(textPage));
        tp->addWidget(controls);
        tp->addWidget(separator(textPage));
        tp->addWidget(m_preview);
        tp->addStretch(1);
        addPage(textPage, i18n("Text"));
    } else {
        QHBoxLayout *columns = new QHBoxLayout();
        QVBoxLayout *left = new QVBoxLayout();
        left->addWidget(m_edit, 1);
        left->addWidget(m_preview);
        QScrollArea *controlsArea = new QScrollArea(textPage);
        controlsArea->setWidgetResizable(true);
        controlsArea->setFrameShape(QFrame::NoFrame);
        controlsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        controlsArea->setWidget(controls);
        columns->addLayout(left, 1);
        columns->addWidget(controlsArea, 1);
        tp->addLayout(columns, 1);
        m_tabs->addTab(textPage, i18n("Text"));   // not in a scroll area of its own: the right column scrolls
    }
    m_textPage = textPage;

    // ---- the Font tab: a long list of the fonts, searched, with the favorites ----
    {
        QWidget *page = new QWidget(m_tabs);
        QVBoxLayout *lay = new QVBoxLayout(page);
        lay->setContentsMargins(4, 6, 4, 4);
        m_fontSearch = new QLineEdit(page);
        m_fontSearch->setPlaceholderText(i18n("Search the fonts"));
        m_fontSearch->setClearButtonEnabled(true);
        m_favoritesOnly = new QCheckBox(i18n("Favorites only"), page);
        m_fontList = new QListWidget(page);
        m_fontList->setMinimumHeight(m_fontList->fontMetrics().height() * 10);
        lay->addWidget(m_fontSearch);
        lay->addWidget(m_favoritesOnly);
        lay->addWidget(m_fontList, 1);
        addPage(page, i18n("Font"));
        connect(m_fontSearch, &QLineEdit::textChanged, this, [this]() { fillFontList(); });
        connect(m_favoritesOnly, &QCheckBox::toggled, this, [this]() { fillFontList(); });
        connect(m_fontList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            const int index = m_fontBox->findText(item->data(Qt::UserRole).toString(), Qt::MatchFixedString);
            if (index >= 0) m_fontBox->setCurrentIndex(index);
        });
    }

    // ---- the Style tab: colors ----
    {
        QWidget *page = new QWidget(m_tabs);
        QVBoxLayout *lay = new QVBoxLayout(page);
        lay->setContentsMargins(4, 6, 4, 4);
        lay->addWidget(new QLabel(i18n("Color"), page));
        QGridLayout *swatches = new QGridLayout();
        swatches->setSpacing(6);
        const QList<QColor> presets = {QColor("#000000"), QColor("#ffffff"), QColor("#808080"), QColor("#d32f2f"), QColor("#f57c00"), QColor("#fbc02d"),
                                       QColor("#388e3c"), QColor("#0097a7"), QColor("#1976d2"), QColor("#7b1fa2"), QColor("#e91e63"), QColor("#6d4c41")};
        for (int i = 0; i < presets.size(); ++i) {
            QToolButton *swatch = new QToolButton(page);
            swatch->setIcon(swatchIcon(presets.at(i), QSize(48, 48)));
            swatch->setIconSize(QSize(int(fontMetrics().height() * 2.2), int(fontMetrics().height() * 2.2)));
            swatch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            const QColor color = presets.at(i);
            connect(swatch, &QToolButton::clicked, this, [this, color]() { setColor(color); });
            // swatches->addWidget(swatch, i / 6, i % 6);
            swatches->addWidget(swatch, i / 4, i % 4);   // four a row: six made the window wider than a phone
        }
        lay->addLayout(swatches);
        QPushButton *more = new QPushButton(i18n("More colors..."), page);
        connect(more, &QPushButton::clicked, this, [this]() {
            const QColor picked = QColorDialog::getColor(m_color, this, i18n("Text color"));
            if (picked.isValid()) setColor(picked);
        });
        lay->addWidget(more);
        QPushButton *fore = new QPushButton(i18n("Use the foreground color"), page);
        fore->setEnabled(m_foreground.isValid());
        connect(fore, &QPushButton::clicked, this, [this]() { if (m_foreground.isValid()) setColor(m_foreground); });
        lay->addWidget(fore);
        lay->addStretch(1);
        addPage(page, i18n("Style"));
    }

    // ---- the Layout tab: the text box ----
    {
        QWidget *page = new QWidget(m_tabs);
        QVBoxLayout *lay = new QVBoxLayout(page);
        lay->setContentsMargins(4, 6, 4, 4);
        lay->addWidget(new QLabel(i18n("Text box width"), page));
        m_boxWidthBox = new QDoubleSpinBox(page);
        m_boxWidthBox->setRange(0.0, 20000.0);
        m_boxWidthBox->setDecimals(0);
        m_boxWidthBox->setSingleStep(10.0);
        m_boxWidthBox->setSpecialValueText(i18n("No box"));
        lay->addWidget(m_boxWidthBox);
        QLabel *hint = new QLabel(i18n("In a box the words wrap at the box's width. Drag with a finger on the picture to draw a box; 0 means one line for each line typed."), page);
        hint->setWordWrap(true);
        lay->addWidget(hint);
        lay->addStretch(1);
        addPage(page, i18n("Layout"));
    }

    // ---- the buttons at the bottom ----
    {
        QHBoxLayout *buttons = new QHBoxLayout();
        QPushButton *reset = new QPushButton(QStringLiteral("\u21BA  ") + i18n("Reset"), this);
        QPushButton *cancel = new QPushButton(i18n("Cancel"), this);
        QPushButton *ok = new QPushButton(i18n("OK"), this);
        ok->setDefault(true);
        ok->setStyleSheet(QStringLiteral("QPushButton { background: %1; color: %2; font-weight: bold; }")
                              .arg(palette().color(QPalette::Highlight).name(), palette().color(QPalette::HighlightedText).name()));
        const int h = int(fontMetrics().height() * 2.3);
        reset->setMinimumHeight(h);
        cancel->setMinimumHeight(h);
        ok->setMinimumHeight(h);
        buttons->addWidget(reset);
        buttons->addStretch(1);
        buttons->addWidget(cancel);
        buttons->addWidget(ok);
        root->addLayout(buttons);
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
        connect(ok, &QPushButton::clicked, this, [this]() { rememberColor(m_color); accept(); });
        connect(reset, &QPushButton::clicked, this, [this]() {
            KrimbleTextStyle defaults;
            defaults.boxWidth = m_boxWidthBox->value();
            m_kern.fill(0.0);
            applyStyleToControls(defaults);
            updateKernDisplay();
            updatePreview();
        });
    }

    // ---- the connections ----
    connect(m_edit->document(), &QTextDocument::contentsChange, this, [this](int position, int removed, int added) {
        // the kerning follows the typing: characters typed or deleted move the kerning after them
        if (m_changingCase || position < 0) return;
        if (position > m_kern.size()) m_kern.resize(position);
        m_kern.remove(position, qMin(removed, m_kern.size() - position));
        m_kern.insert(position, added, 0.0);
    });
    connect(m_edit, &QPlainTextEdit::textChanged, this, [this]() { updateKernDisplay(); updatePreview(); });
    connect(m_edit, &QPlainTextEdit::cursorPositionChanged, this, [this]() { updateKernDisplay(); });
    connect(m_fontBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        m_starButton->blockSignals(true);
        m_starButton->setChecked(favorites().contains(m_fontBox->currentText()));
        m_starButton->blockSignals(false);
        updatePreview();
    });
    connect(m_starButton, &QToolButton::clicked, this, [this]() { toggleFavorite(); });
    connect(m_sizeBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { updatePreview(); });
    connect(m_colorButton, &QToolButton::clicked, this, [this]() {
        const QColor picked = QColorDialog::getColor(m_color, this, i18n("Text color"));
        if (picked.isValid()) setColor(picked);
    });
    connect(m_dropperButton, &QToolButton::clicked, this, [this]() { if (m_foreground.isValid()) setColor(m_foreground); });
    Q_FOREACH (QToolButton *button, QList<QToolButton*>({m_boldButton, m_italicButton, m_underlineButton, m_strikeButton})) {
        connect(button, &QToolButton::toggled, this, [this]() { updatePreview(); });
    }
    connect(m_alignGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [this]() { updatePreview(); });
    connect(m_trackBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { updatePreview(); });
    connect(m_lineBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this]() { updatePreview(); });
    connect(m_kernBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
        if (m_settingKern) return;
        const int at = m_edit->textCursor().position();
        if (at <= 0 || at >= m_edit->toPlainText().size()) return;
        if (m_kern.size() <= at) m_kern.resize(at + 1);
        m_kern[at] = value;
        updatePreview();
    });
    rebuildColorMenu();
    fillFontList();

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        // the height the Text tab wants (the scroll area itself asks for less), but never more than the screen has
        const QSize areaHint = m_tabs->currentWidget()->sizeHint();
        const int wanted = sizeHint().height() - areaHint.height() + m_textPage->sizeHint().height() + 6;
        resize(int(screen->availableGeometry().width() * 0.96), m_landscape ? int(screen->availableGeometry().height() * 0.94)
                                                                          : qMin(wanted, int(screen->availableGeometry().height() * 0.94)));
    }
    m_edit->setFocus();
}

void KrimbleTextDialog::applyStyleToControls(const KrimbleTextStyle &style)
{
    int index = m_fontBox->findText(style.family, Qt::MatchFixedString);
    if (index < 0) index = m_fontBox->findText(QStringLiteral("sans-serif"), Qt::MatchFixedString);
    m_fontBox->setCurrentIndex(qMax(0, index));
    m_starButton->setChecked(favorites().contains(m_fontBox->currentText()));
    m_sizeBox->setValue(style.size);
    m_boldButton->setChecked(style.bold);
    m_italicButton->setChecked(style.italic);
    m_underlineButton->setChecked(style.underline);
    m_strikeButton->setChecked(style.strike);
    if (QAbstractButton *button = m_alignGroup->button(qBound(0, style.align, 3))) button->setChecked(true);
    m_trackBox->setValue(style.tracking);
    m_lineBox->setValue(style.lineSpacing);
    m_boxWidthBox->setValue(style.boxWidth);
    setColor(style.color);
}

void KrimbleTextDialog::setColor(const QColor &color)
{
    m_color = color;
    m_colorButton->setIcon(swatchIcon(color, m_colorButton->iconSize() * 2));
    updatePreview();
}

QString KrimbleTextDialog::text() const
{
    return m_edit->toPlainText();
}

KrimbleTextStyle KrimbleTextDialog::style() const
{
    KrimbleTextStyle s;
    s.family = m_fontBox->currentText();
    s.size = m_sizeBox->value();
    s.bold = m_boldButton->isChecked();
    s.italic = m_italicButton->isChecked();
    s.underline = m_underlineButton->isChecked();
    s.strike = m_strikeButton->isChecked();
    s.align = qMax(0, m_alignGroup->checkedId());
    s.tracking = m_trackBox->value();
    s.lineSpacing = m_lineBox->value();
    s.color = m_color;
    s.kern = m_kern;
    s.kern.resize(m_edit->toPlainText().size());
    s.boxWidth = m_boxWidthBox->value();
    return s;
}

void KrimbleTextDialog::showTab(int index)
{
    m_tabs->setCurrentIndex(index);
}

void KrimbleTextDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    if (m_preview) updatePreview();
}

QStringList KrimbleTextDialog::favorites() const
{
    QSettings settings(QStringLiteral("Krimble"), QStringLiteral("TypeTool"));
    return settings.value(QStringLiteral("favoriteFonts")).toStringList();
}

void KrimbleTextDialog::toggleFavorite()
{
    QSettings settings(QStringLiteral("Krimble"), QStringLiteral("TypeTool"));
    QStringList list = settings.value(QStringLiteral("favoriteFonts")).toStringList();
    const QString family = m_fontBox->currentText();
    if (list.contains(family)) list.removeAll(family); else list.prepend(family);
    settings.setValue(QStringLiteral("favoriteFonts"), list);
    settings.sync();
    m_starButton->setChecked(list.contains(family));
    fillFontList();
}

void KrimbleTextDialog::fillFontList()
{
    m_fontList->clear();
    const QString needle = m_fontSearch->text().trimmed();
    const bool onlyFavorites = m_favoritesOnly->isChecked();
    const QStringList favs = favorites();
    for (int i = 0; i < m_fontBox->count(); ++i) {
        const QString family = m_fontBox->itemText(i);
        if (!needle.isEmpty() && !family.contains(needle, Qt::CaseInsensitive)) continue;
        if (onlyFavorites && !favs.contains(family)) continue;
        QListWidgetItem *item = new QListWidgetItem((favs.contains(family) ? QStringLiteral("\u2605 ") : QString()) + family, m_fontList);
        item->setData(Qt::UserRole, family);
        QFont f(family);
        f.setPointSizeF(m_fontList->font().pointSizeF() * 1.3);
        item->setFont(f);
        if (family == m_fontBox->currentText()) m_fontList->setCurrentItem(item);
    }
}

void KrimbleTextDialog::rememberColor(const QColor &color)
{
    QSettings settings(QStringLiteral("Krimble"), QStringLiteral("TypeTool"));
    QStringList list = settings.value(QStringLiteral("recentColors")).toStringList();
    list.removeAll(color.name());
    list.prepend(color.name());
    while (list.size() > 8) list.removeLast();
    settings.setValue(QStringLiteral("recentColors"), list);
    settings.sync();
}

void KrimbleTextDialog::rebuildColorMenu()
{
    QMenu *menu = m_colorButton->menu();
    menu->clear();
    const QStringList recents = colorsFromSettings();
    if (!recents.isEmpty()) menu->addSection(i18n("Recent colors"));
    Q_FOREACH (const QString &name, recents) {
        const QColor color(name);
        QAction *action = menu->addAction(swatchIcon(color, QSize(32, 32)), name);
        connect(action, &QAction::triggered, this, [this, color]() { setColor(color); });
    }
    QAction *more = menu->addAction(i18n("More colors..."));
    connect(more, &QAction::triggered, this, [this]() {
        const QColor picked = QColorDialog::getColor(m_color, this, i18n("Text color"));
        if (picked.isValid()) setColor(picked);
    });
}

void KrimbleTextDialog::updateKernDisplay()
{
    if (!m_kernBox || !m_edit) return;
    const int at = m_edit->textCursor().position();
    const qreal v = (at > 0 && at < m_kern.size()) ? m_kern.at(at) : 0.0;
    m_settingKern = true;
    m_kernBox->setValue(v);
    m_settingKern = false;
    m_kernBox->setEnabled(at > 0 && at < m_edit->toPlainText().size());   // between two letters only
}

void KrimbleTextDialog::nudgeKern(qreal direction)
{
    const int at = m_edit->textCursor().position();
    if (at <= 0 || at >= m_edit->toPlainText().size()) return;   // between two letters only
    if (m_kern.size() <= at) m_kern.resize(at + 1);
    m_kern[at] += direction * m_sizeBox->value() * 0.02;
    updateKernDisplay();
    updatePreview();
}

void KrimbleTextDialog::applyCase(int mode)
{
    QTextCursor c = m_edit->textCursor();
    if (!c.hasSelection()) c.select(QTextCursor::Document);
    QString s = c.selectedText();
    s.replace(QChar(0x2029), QLatin1Char('\n'));
    QString out;
    if (mode == 0) out = s.toUpper();
    else if (mode == 1) out = s.toLower();
    else {
        bool startOfWord = true;       // Title Case
        bool startOfSentence = true;   // Sentence case
        Q_FOREACH (const QChar ch, s) {
            if (mode == 2) {
                out += ch.isLetter() ? (startOfWord ? ch.toUpper() : ch.toLower()) : ch;
                startOfWord = !(ch.isLetterOrNumber() || ch == QLatin1Char('\''));
            } else {
                out += ch.isLetter() ? (startOfSentence ? ch.toUpper() : ch.toLower()) : ch;
                if (ch.isLetterOrNumber()) startOfSentence = false;
                else if (ch == QLatin1Char('.') || ch == QLatin1Char('!') || ch == QLatin1Char('?') || ch == QLatin1Char('\n')) startOfSentence = true;
            }
        }
    }
    m_changingCase = true;   // the same number of characters: the kerning stays where it is
    c.insertText(out);
    m_changingCase = false;
    m_edit->setTextCursor(c);
    updateKernDisplay();
    updatePreview();
}

// the words in the font, size (at most 40 pt, so that it fits), bold, italic, underline, strike-out, alignment, line spacing and color that are chosen, changing as they are chosen, drawn by
// hand so that the tracking and the kerning show: each line is drawn in pieces between kerned letters (the font's own kerning still applies inside a piece). On a dark background when
// the color is light, so that it can be seen.
void KrimbleTextDialog::updatePreview()
{
    KrimblePreviewWidget *preview = static_cast<KrimblePreviewWidget*>(m_preview);
    if (!preview || !m_edit || !m_fontBox || m_fontBox->count() == 0) return;
    // everything the picture needs is taken now (not at the time of painting)
    QFont previewFont(m_fontBox->currentText());
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    previewFont.setFamilies(QStringList() << previewFont.family() << QStringLiteral("Noto Color Emoji") << QStringLiteral("Samsung Color Emoji"));
#endif
    const qreal sizeNow = qMax(1.0, m_sizeBox->value());
    previewFont.setPointSizeF(qBound(8.0, sizeNow, 40.0));
    previewFont.setBold(m_boldButton->isChecked());
    previewFont.setItalic(m_italicButton->isChecked());
    previewFont.setUnderline(m_underlineButton->isChecked());
    previewFont.setStrikeOut(m_strikeButton->isChecked());
    const QColor color = m_color;
    const QColor background = color.lightness() > 190 ? QColor("#333344") : QColor("#ffffff");
    const QString sample = m_edit->toPlainText();
    const QString shown = sample.isEmpty() ? i18n("Sample text") : sample;
    const qreal dpi = preview->logicalDpiX();
    const qreal scale = previewFont.pointSizeF() / sizeNow;
    const qreal toPx = scale * dpi / 72.0;
    QFont drawFont = previewFont;
    drawFont.setLetterSpacing(QFont::AbsoluteSpacing, m_trackBox->value() * toPx);
    const QFontMetricsF fm(drawFont);
    const qreal lineStep = fm.lineSpacing() * m_lineBox->value() / 100.0;
    const QStringList lines = shown.split(QLatin1Char('\n'));
    const int alignNow = qMax(0, m_alignGroup->checkedId());
    const QVector<qreal> kern = m_kern;
    preview->setFixedHeight(qMax(m_edit->fontMetrics().height() * 3, int(lineStep * lines.size() + fm.descent() + 14)));
    const qreal trackPx = m_trackBox->value() * toPx;
    const int linePercent = m_lineBox->value();
    preview->painter = [=](QPainter &p, int width, int height) {
        p.fillRect(0, 0, width, height, background);
        const qreal available = qMax(40, width - 8);
        // one pass measures the widest line, the second draws; a line wider than the preview makes the whole picture smaller, so that nothing is cut off
        auto run = [&](qreal factor, bool draw) -> qreal {
            QFont font = drawFont;
            font.setPointSizeF(drawFont.pointSizeF() * factor);
            font.setLetterSpacing(QFont::AbsoluteSpacing, trackPx * factor);
            const QFontMetricsF fm2(font);
            const qreal step = fm2.lineSpacing() * linePercent / 100.0;
            if (draw) {
                p.setFont(font);
                p.setPen(color);
            }
            qreal widest = 0.0;
            int lineStart = 0;
            for (int li = 0; li < lines.size(); ++li) {
                const QString &line = lines.at(li);
                QList<QPair<QString, qreal>> pieces;   // text, gap before it (px)
                int pieceStart = 0;
                for (int ci = 1; ci <= line.size(); ++ci) {
                    const int pos = lineStart + ci;
                    const bool cut = ci == line.size() || (!sample.isEmpty() && pos < kern.size() && qAbs(kern.at(pos)) > 0.0001);
                    if (cut) {
                        const int gapPos = lineStart + pieceStart;
                        const qreal gap = (pieceStart > 0 && !sample.isEmpty() && gapPos < kern.size()) ? kern.at(gapPos) * toPx * factor : 0.0;
                        pieces << qMakePair(line.mid(pieceStart, ci - pieceStart), gap);
                        pieceStart = ci;
                    }
                }
                qreal lineWidth = 0.0;
                for (const auto &piece : pieces) lineWidth += piece.second + fm2.horizontalAdvance(piece.first);
                widest = qMax(widest, lineWidth);
                if (draw) {
                    qreal x = (alignNow == 1) ? (width - lineWidth) / 2.0 : (alignNow == 2 ? width - lineWidth - 4.0 : 4.0);
                    const qreal y = 6 + fm2.ascent() + li * step;
                    for (const auto &piece : pieces) {
                        x += piece.second;
                        p.drawText(QPointF(x, y), piece.first);
                        x += fm2.horizontalAdvance(piece.first);
                    }
                }
                lineStart += line.size() + 1;
            }
            return widest;
        };
        const qreal widest = run(1.0, false);
        const qreal factor = widest > available ? qMax(0.15, available / widest * 0.98) : 1.0;
        run(factor, true);
    };
    preview->update();
}
