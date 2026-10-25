/*
 * Krimble 2026-10-25: the window of Batch Process: a list of recipes (the built-in ones and the ones saved by the user), the fields of the recipe (resize, format, quality, name),
 * "Save recipe...", and the run over the pictures chosen. It uses Qt widgets only: the choosing of pictures and the saving to the phone's Pictures folder (Android code) are
 * given to it as functions (KisMainWindow.cpp), so it can be tried on any machine (tools/batch-bench).
 */
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
    // pick: asks the user for the pictures (returns their paths); save: puts one finished file (a path in the cache, its mime type and name) where it belongs, true if it worked
    typedef std::function<QStringList()> PickFunction;
    // folder: the folder in Pictures/Krimble (a script can name it with "folder")
    typedef std::function<bool(const QString &tempPath, const QString &mime, const QString &name, const QString &folder)> SaveFunction;

    KrimbleBatchDialog(QWidget *parent, PickFunction pick, SaveFunction save, const QString &whereText);

    KrimbleBatch::Recipe currentRecipe() const;      // what the fields (or the script) say now
    void setScript(const QString &text);             // switches to "use a script" with this text (for the bench)
    bool buildProgram(KrimbleBatch::Program *program, QString *error) const;   // the script, or the settings, as what is to be done
    void runOn(const QStringList &files);            // the run (used by the button, and by the bench)

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
