/*
 * Krimble 2026-10-25: see KrimbleBatchDialog.h.
 */
#include "KrimbleBatchDialog.h"

#include <klocalizedstring.h>

#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFormLayout>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QVBoxLayout>

KrimbleBatchDialog::KrimbleBatchDialog(QWidget *parent, PickFunction pick, SaveFunction save, const QString &whereText)
    : QDialog(parent)
    , m_pick(pick)
    , m_save(save)
    , m_where(whereText)
    , m_cancelRequested(new bool(false))
{
    using namespace KrimbleBatch;
    setWindowTitle(i18n("Batch Process"));
    setAttribute(Qt::WA_DeleteOnClose, parent != nullptr);
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    QLabel *intro = new QLabel(i18n("Pick a recipe (or change its settings), then the pictures. Results go to %1; the originals are not changed.", m_where), this);
    intro->setWordWrap(true);
    // the recipe and its settings (and the first line): in a scroll area, so that it fits a phone held sideways
    QWidget *form = new QWidget();
    QVBoxLayout *formLayout = new QVBoxLayout(form);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->setSpacing(6);
    formLayout->addWidget(intro);

    m_recipeCombo = new QComboBox(form);
    m_recipeCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_recipeCombo->setMinimumContentsLength(12);
    QHBoxLayout *recipeRow = new QHBoxLayout();
    m_saveRecipeButton = new QPushButton(i18n("Save recipe..."), form);
    m_deleteRecipeButton = new QPushButton(i18n("Delete"), form);
    recipeRow->addWidget(m_saveRecipeButton, 1);
    recipeRow->addWidget(m_deleteRecipeButton);
    formLayout->addWidget(new QLabel(i18n("Recipe"), form));
    formLayout->addWidget(m_recipeCombo);
    formLayout->addLayout(recipeRow);

    QFrame *line1 = new QFrame(form);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    formLayout->addWidget(line1);

    // resize
    m_resizeCombo = new QComboBox(form);
    m_resizeCombo->addItems(QStringList() << i18n("Keep the size") << i18n("Longest side") << i18n("Width") << i18n("Height"));
    m_sizeBox = new QSpinBox(form);
    m_sizeBox->setRange(16, 16000);
    m_sizeBox->setSingleStep(10);
    m_sizeBox->setSuffix(i18n(" px"));
    m_noEnlarge = new QCheckBox(i18n("Don't make small pictures bigger"), form);
    formLayout->addWidget(new QLabel(i18n("Size"), form));
    QHBoxLayout *sizeRow = new QHBoxLayout();
    sizeRow->addWidget(m_resizeCombo, 1);
    sizeRow->addWidget(m_sizeBox, 1);
    formLayout->addLayout(sizeRow);
    formLayout->addWidget(m_noEnlarge);

    // format and quality
    m_formatCombo = new QComboBox(form);
    m_formatCombo->addItems(QStringList() << i18n("JPG") << i18n("PNG") << i18n("WebP, lossless") << i18n("WebP, lossy"));
    m_qualitySlider = new QSlider(Qt::Horizontal, form);
    m_qualitySlider->setRange(1, 100);
    m_qualityValue = new QLabel(form);
    m_qualityValue->setMinimumWidth(m_qualityValue->fontMetrics().horizontalAdvance(QStringLiteral("100 %")) + 4);
    m_qualityLabel = new QLabel(i18n("Quality"), form);
    formLayout->addWidget(new QLabel(i18n("Format"), form));
    formLayout->addWidget(m_formatCombo);
    formLayout->addWidget(m_qualityLabel);
    QHBoxLayout *qualityRow = new QHBoxLayout();
    qualityRow->addWidget(m_qualitySlider, 1);
    qualityRow->addWidget(m_qualityValue);
    formLayout->addLayout(qualityRow);

    // the name
    m_suffixEdit = new QLineEdit(form);
    m_suffixEdit->setPlaceholderText(i18n("nothing added"));
    formLayout->addWidget(new QLabel(i18n("Add to the end of the name"), form));
    formLayout->addWidget(m_suffixEdit);

    m_hint = new QLabel(form);
    m_hint->setWordWrap(true);
    formLayout->addWidget(m_hint);
    formLayout->addStretch(1);

    QScrollArea *area = new QScrollArea(this);
    area->setWidgetResizable(true);
    area->setFrameShape(QFrame::NoFrame);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidget(form);
    root->addWidget(area, 1);

    // the run
    m_startButton = new QPushButton(i18n("Choose Pictures and Start..."), this);
    m_bar = new QProgressBar(this);
    m_bar->setRange(0, 100);
    m_bar->setValue(0);
    m_bar->setVisible(false);   // shown while a run goes on
    m_status = new QLabel(this);
    m_status->setWordWrap(true);
    m_cancelButton = new QPushButton(i18n("Cancel"), this);
    m_cancelButton->setEnabled(false);
    m_closeButton = new QPushButton(i18n("Close"), this);
    QHBoxLayout *buttonRow = new QHBoxLayout();
    buttonRow->addWidget(m_cancelButton);
    buttonRow->addWidget(m_closeButton);
    root->addWidget(m_startButton);
    root->addWidget(m_bar);
    root->addWidget(m_status);
    root->addLayout(buttonRow);

    // WebP cannot be written on this build: the two WebP formats are shown, but cannot be chosen
    if (!webpAvailable()) {
        if (QStandardItemModel *model = qobject_cast<QStandardItemModel*>(m_formatCombo->model())) {
            for (int i = FormatWebPLossless; i <= FormatWebPLossy; ++i) {
                if (QStandardItem *item = model->item(i)) item->setEnabled(false);
            }
        }
    }

    m_recipes = builtInRecipes() + userRecipes();
    fillRecipeCombo();

    connect(m_recipeCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
        if (index >= 0 && index < m_recipes.size()) loadRecipeIntoFields(m_recipes.at(index));
    });
    connect(m_resizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() { updateFieldStates(); });
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() { updateFieldStates(); });
    connect(m_qualitySlider, &QSlider::valueChanged, this, [this](int v) { m_qualityValue->setText(QStringLiteral("%1 %").arg(v)); });
    connect(m_saveRecipeButton, &QPushButton::clicked, this, [this]() { saveRecipeAs(); });
    connect(m_deleteRecipeButton, &QPushButton::clicked, this, [this]() { deleteRecipe(); });
    connect(m_closeButton, &QPushButton::clicked, this, [this]() { close(); });
    connect(m_cancelButton, &QPushButton::clicked, this, [this]() {
        *m_cancelRequested = true;
        m_cancelButton->setEnabled(false);
    });
    connect(m_startButton, &QPushButton::clicked, this, [this]() {
        const QStringList files = m_pick ? m_pick() : QStringList();
        if (!files.isEmpty()) runOn(files);
    });

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        resize(int(screen->availableGeometry().width() * 0.92), qMin(sizeHint().height() + 30, int(screen->availableGeometry().height() * 0.94)));
    }
}

void KrimbleBatchDialog::fillRecipeCombo(const QString &select)
{
    m_recipeCombo->clear();
    for (const KrimbleBatch::Recipe &r : m_recipes) {
        m_recipeCombo->addItem(r.builtIn ? i18n(r.name.toUtf8().constData()) : QStringLiteral("\u2605 ") + r.name);   // a star marks the user's own
    }
    int index = 0;
    if (!select.isEmpty()) {
        for (int i = 0; i < m_recipes.size(); ++i) if (m_recipes.at(i).name == select) index = i;
    }
    m_recipeCombo->setCurrentIndex(index);
    if (!m_recipes.isEmpty()) loadRecipeIntoFields(m_recipes.at(index));
}

void KrimbleBatchDialog::loadRecipeIntoFields(const KrimbleBatch::Recipe &recipe)
{
    m_resizeCombo->setCurrentIndex(recipe.resize);
    m_sizeBox->setValue(recipe.size > 0 ? recipe.size : 1920);
    m_noEnlarge->setChecked(recipe.noEnlarge);
    m_formatCombo->setCurrentIndex(recipe.format);
    m_qualitySlider->setValue(recipe.quality);
    m_qualityValue->setText(QStringLiteral("%1 %").arg(recipe.quality));
    m_suffixEdit->setText(recipe.suffix);
    updateFieldStates();
}

void KrimbleBatchDialog::updateFieldStates()
{
    using namespace KrimbleBatch;
    const bool resizes = m_resizeCombo->currentIndex() != ResizeNone;
    m_sizeBox->setEnabled(resizes);
    m_noEnlarge->setEnabled(resizes);
    const Recipe r = currentRecipe();
    m_qualitySlider->setEnabled(r.usesQuality());
    m_qualityLabel->setEnabled(r.usesQuality());
    m_qualityValue->setEnabled(r.usesQuality());
    QString hint;
    if (r.format == FormatJpg) hint = i18n("JPG has no transparency: transparent parts become white.");
    else if (r.format == FormatPng) hint = i18n("PNG keeps transparency. The file is not made smaller by quality.");
    else if (r.format == FormatWebPLossless) hint = i18n("WebP, lossless: keeps transparency and every pixel; the file is usually smaller than a PNG.");
    else hint = i18n("WebP, lossy: keeps transparency; a smaller file than lossless, with a little loss of detail.");
    if (!webpAvailable() && (r.format == FormatWebPLossless || r.format == FormatWebPLossy)) hint += QLatin1Char(' ') + i18n("This version of Krimble cannot write WebP: choose another format.");
    hint += QLatin1Char('\n') + i18n("Example: photo.png becomes %1", outputName(QStringLiteral("photo.png"), r));
    m_hint->setText(hint);
    m_deleteRecipeButton->setEnabled(m_recipeCombo->currentIndex() >= 0 && m_recipeCombo->currentIndex() < m_recipes.size() && !m_recipes.at(m_recipeCombo->currentIndex()).builtIn);
}

KrimbleBatch::Recipe KrimbleBatchDialog::currentRecipe() const
{
    KrimbleBatch::Recipe r;
    r.resize = m_resizeCombo->currentIndex();
    r.size = m_sizeBox->value();
    r.noEnlarge = m_noEnlarge->isChecked();
    r.format = m_formatCombo->currentIndex();
    r.quality = m_qualitySlider->value();
    r.suffix = m_suffixEdit->text().trimmed();
    const int index = m_recipeCombo->currentIndex();
    if (index >= 0 && index < m_recipes.size()) r.name = m_recipes.at(index).name;
    return r;
}

void KrimbleBatchDialog::saveRecipeAs()
{
    bool ok = false;
    const int index = m_recipeCombo->currentIndex();
    const QString suggestion = (index >= 0 && index < m_recipes.size() && !m_recipes.at(index).builtIn) ? m_recipes.at(index).name : QString();
    const QString name = QInputDialog::getText(this, i18n("Save recipe"), i18n("A name for this recipe:"), QLineEdit::Normal, suggestion, &ok).trimmed();
    if (!ok || name.isEmpty()) return;
    for (const KrimbleBatch::Recipe &r : m_recipes) {
        if (r.builtIn && r.name.compare(name, Qt::CaseInsensitive) == 0) {
            QMessageBox::information(this, i18n("Save recipe"), i18n("A built-in recipe has that name. Please choose another."));
            return;
        }
    }
    KrimbleBatch::Recipe r = currentRecipe();
    r.name = name;
    r.builtIn = false;
    bool replaced = false;
    for (int i = 0; i < m_recipes.size(); ++i) {
        if (!m_recipes.at(i).builtIn && m_recipes.at(i).name.compare(name, Qt::CaseInsensitive) == 0) { m_recipes[i] = r; replaced = true; }
    }
    if (!replaced) m_recipes.append(r);
    KrimbleBatch::saveUserRecipes(m_recipes);
    fillRecipeCombo(name);
}

void KrimbleBatchDialog::deleteRecipe()
{
    const int index = m_recipeCombo->currentIndex();
    if (index < 0 || index >= m_recipes.size() || m_recipes.at(index).builtIn) return;
    if (QMessageBox::question(this, i18n("Delete recipe"), i18n("Delete the recipe \"%1\"?", m_recipes.at(index).name)) != QMessageBox::Yes) return;
    m_recipes.removeAt(index);
    KrimbleBatch::saveUserRecipes(m_recipes);
    fillRecipeCombo();
}

void KrimbleBatchDialog::runOn(const QStringList &files)
{
    using namespace KrimbleBatch;
    if (files.isEmpty()) return;
    const Recipe recipe = currentRecipe();
    if (!webpAvailable() && (recipe.format == FormatWebPLossless || recipe.format == FormatWebPLossy)) {
        m_status->setText(i18n("This version of Krimble cannot write WebP: choose another format."));
        return;
    }
    m_startButton->setEnabled(false);
    m_closeButton->setEnabled(false);
    m_cancelButton->setEnabled(true);
    m_bar->setVisible(true);
    *m_cancelRequested = false;
    const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/batch");
    QDir().mkpath(dirPath);
    QPointer<KrimbleBatchDialog> guard(this);
    int done = 0, failed = 0;
    for (int i = 0; i < files.size(); ++i) {
        m_status->setText(i18n("Picture %1 of %2...", i + 1, files.size()));
        m_bar->setValue(100 * i / files.size());
        QCoreApplication::processEvents();
        if (!guard) return;                     // the window was closed (Android's back button) while a picture was processed
        if (*m_cancelRequested) break;
        QImage image = loadPicture(files.at(i));
        if (image.isNull()) { ++failed; continue; }
        image = applyRecipe(image, recipe);
        QByteArray bytes;
        if (!encodePicture(image, recipe, &bytes)) { ++failed; continue; }
        const QString name = outputName(files.at(i), recipe);
        const QString temp = dirPath + QLatin1Char('/') + name;
        QFile file(temp);
        bool stored = false;
        if (file.open(QIODevice::WriteOnly) && file.write(bytes) == bytes.size()) {
            file.close();
            stored = m_save ? m_save(temp, recipe.mime(), name) : false;
        }
        QFile::remove(temp);
        if (stored) ++done; else ++failed;
    }
    if (!guard) return;
    const bool wasCancelled = *m_cancelRequested;
    m_bar->setValue(wasCancelled ? m_bar->value() : 100);
    m_status->setText(wasCancelled ? i18n("Cancelled: %1 saved in %2, %3 skipped, the rest not processed.", done, m_where, failed)
                                   : i18n("Done: %1 saved in %2, %3 skipped.", done, m_where, failed));
    m_cancelButton->setEnabled(false);
    m_closeButton->setEnabled(true);
    m_startButton->setEnabled(true);
}
