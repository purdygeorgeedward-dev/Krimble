#ifndef KRIMBLE_BATCH_DIALOG_H
#define KRIMBLE_BATCH_DIALOG_H

#include "KrimbleBatch.h"

#include <QDialog>
#include <QSharedPointer>
#include <functional>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QProgressBar;
class QPushButton;
class QSlider;
class QSpinBox;

class KrimbleBatchDialog : public QDialog
{
public:
    typedef std::function<QStringList()> PickFunction;
    typedef std::function<bool(const QString &tempPath, const QString &mime, const QString &name, const QString &folder)> SaveFunction;

    KrimbleBatchDialog(QWidget *parent, PickFunction pick, SaveFunction save, const QString &whereText);

    KrimbleBatch::Recipe currentRecipe() const;
    void setScript(const QString &text);
    bool buildProgram(KrimbleBatch::Program *program, QString *error) const;
    void runOn(const QStringList &files);
    bool processAnimation(const QString &path, KrimbleBatch::Animation animation, const KrimbleBatch::Program &program, int format, const QString &dirPath);

private:
    void fillRecipeCombo(const QString &select = QString());
    void loadRecipeIntoFields(const KrimbleBatch::Recipe &recipe);
    void updateFieldStates();
    void checkScript();
    void saveRecipeAs();
    void deleteRecipe();

    QList<KrimbleBatch::Recipe> m_recipes;
    PickFunction m_pick;
    SaveFunction m_save;
    QString m_where;
    QSharedPointer<bool> m_cancelRequested;

    QComboBox *m_recipeCombo = nullptr;
    QComboBox *m_resizeCombo = nullptr;
    QSpinBox *m_sizeBox = nullptr;
    QCheckBox *m_noEnlarge = nullptr;
    QComboBox *m_formatCombo = nullptr;
    QSlider *m_qualitySlider = nullptr;
    QLabel *m_qualityValue = nullptr;
    QLabel *m_qualityLabel = nullptr;
    QLineEdit *m_suffixEdit = nullptr;
    QLabel *m_hint = nullptr;
    QCheckBox *m_useScript = nullptr;
    QPlainTextEdit *m_scriptEdit = nullptr;
    QLabel *m_scriptError = nullptr;
    QPushButton *m_scriptFromSettings = nullptr;
    QPushButton *m_scriptHelpButton = nullptr;
    QPushButton *m_saveRecipeButton = nullptr;
    QPushButton *m_deleteRecipeButton = nullptr;
    QPushButton *m_startButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QPushButton *m_closeButton = nullptr;
    QProgressBar *m_bar = nullptr;
    QLabel *m_status = nullptr;
};

#endif
