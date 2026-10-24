/*
 * Krimble 2026-10-24: the window of the Type tool, redesigned after George's picture of what it should be ("Text tool UI sucks. Here's what it is and what it should be.").
 * It depends on Qt widgets and i18n only, so it can be built and looked at on its own (tools/textdlg-bench).
 */
#ifndef KRIMBLE_TEXT_DIALOG_H
#define KRIMBLE_TEXT_DIALOG_H

#include <QColor>
#include <QDialog>
#include <QPixmap>
#include <QString>
#include <QStringList>
#include <QVector>

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QSpinBox;
class QTabWidget;
class QToolButton;
class QResizeEvent;

// what the window shows and gives back; the Type tool keeps one of these as "what was chosen last time"
struct KrimbleTextStyle {
    QString family = QStringLiteral("sans-serif");
    qreal size = 24.0;                // pt
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool strike = false;
    int align = 0;                    // 0 left, 1 center, 2 right, 3 justified
    qreal tracking = 0.0;             // pt, for the whole text
    int lineSpacing = 100;            // percent
    QColor color = QColor(Qt::black);
    QVector<qreal> kern;              // pt, an extra gap before the character at each position
    qreal boxWidth = 0.0;             // document units; 0 = no text box
};

class KrimbleTextDialog : public QDialog
{
public:
    KrimbleTextDialog(QWidget *parent, const QString &text, const KrimbleTextStyle &style, bool editing, const QColor &foreground = QColor());

    QString text() const;
    KrimbleTextStyle style() const;
    void showTab(int index);   // for the bench: 0 Text, 1 Font, 2 Style, 3 Layout

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void build(const QString &text, bool editing);
    void applyStyleToControls(const KrimbleTextStyle &style);
    void setColor(const QColor &color);
    void updatePreview();
    void updateKernDisplay();
    void nudgeKern(qreal direction);
    void applyCase(int mode);
    void fillFontList();
    void toggleFavorite();
    QStringList favorites() const;
    void rememberColor(const QColor &color);
    void rebuildColorMenu();

    KrimbleTextStyle m_initial;
    QColor m_color;
    QColor m_foreground;
    QVector<qreal> m_kern;
    bool m_landscape = false;
    bool m_changingCase = false;
    bool m_settingKern = false;

    QTabWidget *m_tabs = nullptr;
    QWidget *m_textPage = nullptr;
    QPlainTextEdit *m_edit = nullptr;
    QComboBox *m_fontBox = nullptr;
    QToolButton *m_starButton = nullptr;
    QDoubleSpinBox *m_sizeBox = nullptr;
    QToolButton *m_colorButton = nullptr;
    QToolButton *m_dropperButton = nullptr;
    QToolButton *m_boldButton = nullptr;
    QToolButton *m_italicButton = nullptr;
    QToolButton *m_underlineButton = nullptr;
    QToolButton *m_strikeButton = nullptr;
    QToolButton *m_caseButton = nullptr;
    QButtonGroup *m_alignGroup = nullptr;
    QDoubleSpinBox *m_trackBox = nullptr;
    QSpinBox *m_lineBox = nullptr;
    QDoubleSpinBox *m_kernBox = nullptr;
    QWidget *m_preview = nullptr;   // a KrimblePreviewWidget (KrimbleTextDialog.cpp)
    QLineEdit *m_fontSearch = nullptr;
    QCheckBox *m_favoritesOnly = nullptr;
    QListWidget *m_fontList = nullptr;
    QDoubleSpinBox *m_boxWidthBox = nullptr;
};

#endif
