/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2013-2014 Yue Liu <yue.liu@mail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoFileDialog.h"
#include <QDebug>
#include <QFileDialog>
#include <KisPreviewFileDialog.h>
#include <QApplication>
#include <QScreen>
#include <QPointer>
#include <QImageReader>
#include <QClipboard>
#include <QInputDialog>
#include <QScrollBar>
#include <QListView>
#include <QTimer>
#include <QMessageBox>

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>
#include <kstandardguiitem.h>

#include <KisMimeDatabase.h>
#include <KoJsonTrader.h>
#include "WidgetUtilsDebug.h"

#include <kis_assert.h>

#ifdef Q_OS_MACOS
#include "KisMacosSecurityBookmarkManager.h"
#endif

class Q_DECL_HIDDEN KoFileDialog::Private
{
public:
    Private(QWidget *parent_,
            KoFileDialog::DialogType dialogType_,
            const QString caption_,
            const QString defaultDir_,
            const QString dialogName_)
        : parent(parent_)
        , type(dialogType_)
        , dialogName(dialogName_)
        , caption(caption_)
        , defaultDirectory(defaultDir_)
        , filterList(QStringList())
        , defaultFilter(QString())
    {
    }

    ~Private()
    {
    }

    QWidget *parent;
    KoFileDialog::DialogType type;
    QString dialogName;
    QString caption;
    QString defaultDirectory;
    QString proposedFileName;
    QUrl defaultUri;
    QStringList filterList;
    QMap<QString, QString> suffixes;
    QString defaultFilter;
    QScopedPointer<KisPreviewFileDialog> fileDialog;
    QString mimeType;
};

KoFileDialog::KoFileDialog(QWidget *parent,
                           KoFileDialog::DialogType type,
                           const QString &dialogName)
    : d(new Private(parent, type, "", getUsedDir(dialogName), dialogName))
{
}

KoFileDialog::~KoFileDialog()
{
    delete d;
}

void KoFileDialog::setCaption(const QString &caption)
{
    d->caption = caption;
}

void KoFileDialog::setDefaultDir(const QString &defaultDir, bool force)
{
    if (!defaultDir.isEmpty()) {
        if (d->defaultDirectory.isEmpty() || force) {
            QFileInfo f(defaultDir);
            if (f.isDir()) {
                d->defaultDirectory = defaultDir;
            }
            else {
                d->defaultDirectory = f.absolutePath();
            }
        }
        if (!QFileInfo(defaultDir).isDir()) {
            d->proposedFileName = QFileInfo(defaultDir).fileName();
        }
    }
}

void KoFileDialog::setDirectoryUrl(const QUrl &defaultUri)
{
    d->defaultUri = defaultUri;
}

void KoFileDialog::setImageFilters()
{
    QStringList imageFilters;
    Q_FOREACH (const QByteArray &format, QImageReader::supportedImageFormats()) {
        imageFilters << QLatin1String("image/") + format;
    }
    setMimeTypeFilters(imageFilters);
}

void KoFileDialog::setNameFilter(const QString &filter)
{
    d->filterList = filter.split(";;");
}

void KoFileDialog::selectNameFilter(const QString &filter)
{
    d->defaultFilter = filter;
}

QString KoFileDialog::selectedNameFilter() const
{
    return d->fileDialog->selectedNameFilter();
}

QString KoFileDialog::selectedMimeType() const
{
    return d->mimeType;
}

#ifdef Q_OS_ANDROID
class KrimbleFitToScreenFilter : public QObject
{
public:
    explicit KrimbleFitToScreenFilter(QWidget *dialog, qreal heightFraction = 0.90) : QObject(dialog), m_dialog(dialog), m_heightFraction(heightFraction) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dialog && event->type() == QEvent::Show) {
            if (QScreen *screen = QApplication::primaryScreen()) {
                const QRect avail = screen->availableGeometry();
                const QSize s(qMin(m_dialog->width(), int(avail.width() * 0.95)), qMin(m_dialog->height(), int(avail.height() * m_heightFraction)));
                m_dialog->resize(s);
                m_dialog->move(avail.left() + (avail.width() - s.width()) / 2, avail.top() + (avail.height() - s.height()) / 2);
            }
        }
        return false;
    }
private:
    QWidget *m_dialog;
    qreal m_heightFraction;
};
#endif

#ifdef Q_OS_ANDROID
class KrimbleTypeListFit : public QObject
{
public:
    explicit KrimbleTypeListFit(QDialog *dialog) : QObject(dialog), m_dialog(dialog) {}
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_dialog && event->type() == QEvent::Show) {
            QPointer<QDialog> guard(m_dialog);
            QTimer::singleShot(0, m_dialog, [guard]() { fit(guard); });
            QTimer::singleShot(150, m_dialog, [guard]() { fit(guard); });
        }
        return false;
    }
    static void fit(QDialog *dialog)
    {
        if (!dialog) return;
        QListView *list = dialog->findChild<QListView*>();
        QScreen *screen = QApplication::primaryScreen();
        if (!list || !list->model() || !screen) return;
        const QRect avail = screen->availableGeometry();
        const int rows = list->model()->rowCount();
        if (rows <= 0) return;
        const int frame = 2 * list->frameWidth();
        const int wantListH = list->sizeHintForRow(0) * rows + frame;
        int widest = 0;
        for (int row = 0; row < rows; ++row) {
            const QString text = list->model()->index(row, 0).data().toString();
            if (text.startsWith(QLatin1String("All supported"))) continue;
            widest = qMax(widest, list->fontMetrics().horizontalAdvance(text) + 24);
        }
        const int wantListW = widest + frame + list->verticalScrollBar()->sizeHint().width() + 8;
        list->setTextElideMode(Qt::ElideRight);
        list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        list->setWordWrap(false);
        const int extraH = qMax(0, dialog->height() - list->height());
        const int extraW = qMax(0, dialog->width() - list->width());
        const int h = qMin(wantListH + extraH, int(avail.height() * 0.98));
        const int w = qMin(qMax(240, wantListW + extraW), int(avail.width() * 0.60));
        if (qAbs(h - dialog->height()) > 2 || qAbs(w - dialog->width()) > 2) {
            dialog->resize(w, h);
            dialog->move(avail.left() + (avail.width() - w) / 2, avail.top() + (avail.height() - h) / 2);
        }
    }
private:
    QDialog *m_dialog;
};
#endif

void KoFileDialog::createFileDialog()
{
    d->fileDialog.reset(new KisPreviewFileDialog(d->parent, d->caption, d->defaultDirectory + "/" + d->proposedFileName));
    if (!d->defaultUri.isEmpty()) {
        d->fileDialog->setDirectoryUrl(d->defaultUri);
    }
    connect(d->fileDialog.get(), SIGNAL(filterSelected(const QString&)), this, SLOT(onFilterSelected(const QString&)));

#ifdef Q_OS_MACOS
    KisMacosSecurityBookmarkManager *bookmarkmngr = KisMacosSecurityBookmarkManager::instance();
    if(bookmarkmngr->isSandboxed()) {
        connect(d->fileDialog.get(), SIGNAL(urlSelected  (const QUrl&)), bookmarkmngr, SLOT(addBookmarkAndCheckParentDir(const QUrl&)));
    }
#endif

    KConfigGroup group = KSharedConfig::openConfig()->group("File Dialogs");

    bool dontUseNative = true;
#ifdef Q_OS_ANDROID
    dontUseNative = false;
#endif
#ifdef Q_OS_UNIX
    if (qgetenv("XDG_CURRENT_DESKTOP") == "KDE") {
        dontUseNative = false;
    }
#endif
#ifdef Q_OS_MACOS
    dontUseNative = false;
#endif
#ifdef Q_OS_WIN
    dontUseNative = false;
#endif

    bool optionDontUseNative;
    if (!qEnvironmentVariable("APPIMAGE").isEmpty()) {
        optionDontUseNative = false;
    } else {
        optionDontUseNative = group.readEntry("DontUseNativeFileDialog", dontUseNative);
    }

    d->fileDialog->setOption(QFileDialog::DontUseNativeDialog, optionDontUseNative);
    d->fileDialog->setOption(QFileDialog::DontConfirmOverwrite, false);
    d->fileDialog->setOption(QFileDialog::HideNameFilterDetails, dontUseNative ? true : false);

#ifdef Q_OS_MACOS
    QList<QUrl> urls = d->fileDialog->sidebarUrls();
    QUrl volumes = QUrl::fromLocalFile("/Volumes");
    if (!urls.contains(volumes)) {
        urls.append(volumes);
    }

    d->fileDialog->setSidebarUrls(urls);
#endif

    if (d->type == SaveFile) {
        d->fileDialog->setAcceptMode(QFileDialog::AcceptSave);
        d->fileDialog->setFileMode(QFileDialog::AnyFile);
    }
    else {

        d->fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

        if (d->type == ImportDirectory || d->type == OpenDirectory) {
            d->fileDialog->setFileMode(QFileDialog::Directory);
            d->fileDialog->setOption(QFileDialog::ShowDirsOnly, true);
        }
        else {
            if (d->type == OpenFile || d->type == ImportFile)
            {
                d->fileDialog->setFileMode(QFileDialog::ExistingFile);
            }
            else {
                d->fileDialog->setFileMode(QFileDialog::ExistingFiles);
            }
        }
    }

#ifndef Q_OS_ANDROID
    d->fileDialog->setNameFilters(d->filterList);

    if (!d->proposedFileName.isEmpty()) {
        QString mime = KisMimeDatabase::mimeTypeForFile(d->proposedFileName, d->type == KoFileDialog::SaveFile ? false : true);

        QString description = KisMimeDatabase::descriptionForMimeType(mime);
        Q_FOREACH(const QString &filter, d->filterList) {
            if (filter.startsWith(description)) {
                d->fileDialog->selectNameFilter(filter);
                break;
            }
        }
    }
    else if (!d->defaultFilter.isEmpty()) {
        d->fileDialog->selectNameFilter(d->defaultFilter);
    }
#endif

    if (d->type == ImportDirectory ||
            d->type == ImportFile || d->type == ImportFiles ||
            d->type == SaveFile) {

        bool allowModal = true;
#ifdef Q_OS_MACOS
        allowModal = optionDontUseNative;
#endif
        if (allowModal) {
            d->fileDialog->setWindowModality(Qt::WindowModal);
        }
    }
    d->fileDialog->resetIconProvider();

    onFilterSelected(d->fileDialog->selectedNameFilter());

    if (!d->dialogName.isEmpty()) {
        KConfigGroup group = KSharedConfig::openConfig()->group("File Dialogs");
        const QByteArray geom = QByteArray::fromBase64(group.readEntry(d->dialogName + "_geometry", QByteArray()));
        if (!geom.isEmpty()) {
            d->fileDialog->restoreGeometry(geom);
        }

        const QString dialogName = d->dialogName;
        QPointer<QFileDialog> dialogPtr = d->fileDialog.get();
        connect(d->fileDialog.get(), &QDialog::finished, this, [dialogName, dialogPtr](int) {
            if (dialogPtr) {
                KConfigGroup group = KSharedConfig::openConfig()->group("File Dialogs");
                group.writeEntry(dialogName + "_geometry", dialogPtr->saveGeometry().toBase64());
            }
        });
    }
#ifdef Q_OS_ANDROID
    d->fileDialog->installEventFilter(new KrimbleFitToScreenFilter(d->fileDialog.get()));
#endif
}

QString KoFileDialog::filename()
{
    QString url;
    createFileDialog();

#ifdef Q_OS_ANDROID
    if (d->type == SaveFile) {
        QString extension = ".kra";
        QInputDialog mimeSelector;
        mimeSelector.setLabelText(i18n("Save As:"));
        mimeSelector.setComboBoxItems(d->filterList);
        mimeSelector.setOkButtonText(KStandardGuiItem::ok().text());
        mimeSelector.setCancelButtonText(KStandardGuiItem::cancel().text());
        mimeSelector.setOption(QInputDialog::UseListViewForComboBoxItems);
        if (QScreen *screen = QApplication::primaryScreen()) {
            const QRect avail = screen->availableGeometry();
            mimeSelector.resize(qMin(int(avail.width() * 0.95), 300), int(avail.height() * 0.98));
        }
#ifdef Q_OS_ANDROID
        mimeSelector.installEventFilter(new KrimbleTypeListFit(&mimeSelector));
#endif

        if (mimeSelector.exec() == QDialog::Accepted) {
            const QString selectedFilter = mimeSelector.textValue();
            int start = selectedFilter.indexOf("*.") + 1;
            int end = selectedFilter.indexOf(" ", start);
            int n = end - start;
            extension = selectedFilter.mid(start, n);
            if (!extension.startsWith(".")) {
                extension = "." + extension;
            }
            if (extension.compare(QStringLiteral(".jpeg"), Qt::CaseInsensitive) == 0) {
                extension = QStringLiteral(".jpg");
            }
            d->fileDialog->selectNameFilter(selectedFilter);

            const QString proposedFileBaseName = QFileInfo(d->proposedFileName).baseName();
            d->fileDialog->setWindowTitle(proposedFileBaseName.isEmpty() ? QString("Untitled" + extension)
                                                                         : proposedFileBaseName + extension);
        } else {
            return url;
        }
    }
#endif

    bool retryNeeded;
    do {
        retryNeeded = false;
        if (d->fileDialog->exec() == QDialog::Accepted) {
            url = d->fileDialog->selectedFiles().first();
        } else {
            url = QString();
            break;
        }

#ifndef Q_OS_ANDROID
        const QString suffix = QFileInfo(url).suffix();
        bool isValidSuffix = true;
        if (KisMimeDatabase::mimeTypeForSuffix(suffix).isEmpty()) {
            warnWidgetUtils << "Selected file name suffix" << suffix << "does not match known MIME types";
            isValidSuffix = false;
        }

        if (d->type == SaveFile && (suffix.isEmpty() || !isValidSuffix)) {
            QString extension;
            if (d->suffixes.contains(d->fileDialog->selectedNameFilter())) {
                extension = d->suffixes[d->fileDialog->selectedNameFilter()];
                if (!extension.isEmpty()) {
                    url.append('.').append(extension);
                    d->fileDialog->selectFile(url);
                }
            }
            if (extension.isEmpty()) {
                QString selectedFilter;
                for (int i = 1; i < d->filterList.size(); ++i) {
                    if (d->filterList[i].startsWith(d->fileDialog->selectedNameFilter())) {
                        selectedFilter = d->filterList[i];
                        break;
                    }
                }
                int start = selectedFilter.indexOf("*.") + 2;
                int end = selectedFilter.indexOf(" ", start);
                if (start != -1 + 2 && end != -1) {
                    extension = selectedFilter.mid(start, end - start);
                }
            }
            QMessageBox::warning(d->parent, d->caption,
                i18n("The selected file name does not have a file extension that Krimble understands.\n"
                     "Make sure the file name ends in '.%1' for example.", extension));
            retryNeeded = true;

#if !(defined(Q_OS_MACOS) || defined(Q_OS_ANDROID))
            url = url + extension;
#endif
        }
#endif
    } while (retryNeeded);

    if (!url.isEmpty()) {
        d->mimeType = KisMimeDatabase::mimeTypeForFile(url, d->type == KoFileDialog::SaveFile ? false : true);
        saveUsedDir(url, d->dialogName);
    }
    return url;
}

QStringList KoFileDialog::filenames()
{
    QStringList urls;

    createFileDialog();
    if (d->fileDialog->exec() == QDialog::Accepted) {
        urls = d->fileDialog->selectedFiles();
    }
    if (urls.size() > 0) {
        saveUsedDir(urls.first(), d->dialogName);
    }
    return urls;
}

QStringList KoFileDialog::splitNameFilter(const QString &nameFilter, QStringList *mimeList)
{
    Q_ASSERT(mimeList);

    QStringList filters;
    QString description;

    if (nameFilter.contains("(")) {
        description = nameFilter.left(nameFilter.indexOf("(") -1).trimmed();
    }

    QStringList entries = nameFilter.mid(nameFilter.indexOf("(") + 1).split(" ", Qt::SkipEmptyParts);

    entries.sort();
    Q_FOREACH (QString entry, entries) {

        entry = entry.remove("*");
        entry = entry.remove(")");

        QString mimeType = KisMimeDatabase::mimeTypeForSuffix(entry);
        if (mimeType != "application/octet-stream") {
            if (!mimeList->contains(mimeType)) {
                mimeList->append(mimeType);
                filters.append(KisMimeDatabase::descriptionForMimeType(mimeType) + " ( *" + entry + " )");
            }
        }
        else {
            filters.append(entry.remove(".").toUpper() + " " + description + " ( *." + entry + " )");
        }
    }
    return filters;
}

void KoFileDialog::setMimeTypeFilters(const QStringList &mimeTypeList, QString defaultMimeType)
{
    constexpr bool withAllSupportedEntry = true;
    QStringList mimeSeen;

    struct FilterData
    {
        QString descriptionOnly;
        QString fullLine;
        QString defaultSuffix;
    };

    FilterData defaultFilter {};
    QString allSupported;
    FilterData kritaNative {};
    FilterData ora {};
    QVector<FilterData> otherFileTypes;
    bool hasAllFilesFilter = false;

    QStringList mimeList = mimeTypeList;
    mimeList.sort();

    Q_FOREACH(const QString &mimeType, mimeList) {
        if (!mimeSeen.contains(mimeType)) {
            if (mimeType == QLatin1String("application/octet-stream")) {
                hasAllFilesFilter = true;
                mimeSeen << mimeType;
                continue;
            }
            QString description = KisMimeDatabase::descriptionForMimeType(mimeType);
            if (description.isEmpty() && !mimeType.isEmpty()) {
                description = mimeType.split("/")[1];
                if (description.startsWith("x-")) {
                    description = description.remove(0, 2);
                }
            }

            QString oneFilter;
            const QStringList suffixes = KisMimeDatabase::suffixesForMimeType(mimeType);
            KIS_SAFE_ASSERT_RECOVER(!suffixes.isEmpty()) {
                warnWidgetUtils << "KoFileDialog: Found no suffixes for mime type" << mimeType;
                continue;
            }

            Q_FOREACH(const QString &suffix, suffixes) {
                const QString glob = QStringLiteral("*.") + suffix;
                oneFilter.append(glob + " ");
                if (withAllSupportedEntry) {
                    allSupported.append(glob + " ");
                }
#ifdef Q_OS_LINUX
                if (qgetenv("XDG_CURRENT_DESKTOP") == "GNOME") {
                    oneFilter.append(glob.toUpper() + " ");
                    if (withAllSupportedEntry) {
                        allSupported.append(glob.toUpper() + " ");
                    }
                }
#endif
            }

            Q_ASSERT(!description.isEmpty());

            FilterData filterData {};
            filterData.descriptionOnly = description;
            filterData.fullLine = description + " ( " + oneFilter + ")";
            filterData.defaultSuffix = suffixes.first();

            if (mimeType == QLatin1String("application/x-krita")) {
                kritaNative = filterData;
            } else if (mimeType == QLatin1String("image/openraster")) {
                ora = filterData;
            } else {
                otherFileTypes.append(filterData);
            }
            if (defaultMimeType == mimeType) {
                debugWidgetUtils << "KoFileDialog: Matched default MIME type to filter" << filterData.fullLine;
                defaultFilter = filterData;
            }
            mimeSeen << mimeType;
        }
    }

    QStringList retFilterList;
    QMap<QString, QString> retFilterToSuffixMap;
    auto addFilterItem = [&](const FilterData &filterData) {
        if (retFilterList.contains(filterData.fullLine)) {
            debugWidgetUtils << "KoFileDialog: Duplicated filter" << filterData.fullLine;
            return;
        }
        retFilterList.append(filterData.fullLine);
        retFilterToSuffixMap.insert(filterData.descriptionOnly, filterData.defaultSuffix);
        retFilterToSuffixMap.insert(filterData.fullLine, filterData.defaultSuffix);
    };

    if (!allSupported.isEmpty()) {
        FilterData allFilter {};
        if (allSupported.contains("*.kra")) {
            allSupported.remove("*.kra ");
            allSupported.prepend("*.kra ");
            allFilter.defaultSuffix = QStringLiteral("kra");
        } else if (!defaultFilter.fullLine.isEmpty()) {
            const QString suffixToMove = QString("*.") + defaultFilter.defaultSuffix + " ";
            allSupported.remove(suffixToMove);
            allSupported.prepend(suffixToMove);
            allFilter.defaultSuffix = defaultFilter.defaultSuffix;
        } else {
            warnWidgetUtils << "KoFileDialog: No default suffix for 'All supported formats'";
            allFilter.defaultSuffix = QStringLiteral("");
        }
        allFilter.descriptionOnly = i18n("All supported formats");
        allFilter.fullLine = allFilter.descriptionOnly + " ( " + allSupported + ")";
        addFilterItem(allFilter);
    }
    if (!kritaNative.fullLine.isEmpty()) {
        addFilterItem(kritaNative);
    }
    if (!ora.fullLine.isEmpty()) {
        addFilterItem(ora);
    }

    std::sort(otherFileTypes.begin(), otherFileTypes.end(), [](const FilterData &a, const FilterData &b) {
        return a.descriptionOnly < b.descriptionOnly;
    });
    Q_FOREACH(const FilterData &filterData, otherFileTypes) {
        addFilterItem(filterData);
    }

    if (hasAllFilesFilter) {
        retFilterList.append(QFileDialog::tr("All files (*)"));
    }

    d->filterList = retFilterList;
    d->suffixes = retFilterToSuffixMap;
    d->defaultFilter = defaultFilter.fullLine;
}

QString KoFileDialog::getUsedDir(const QString &dialogName)
{
    if (dialogName.isEmpty()) return "";

    KConfigGroup group =  KSharedConfig::openConfig()->group("File Dialogs");
    QString dir = group.readEntry(dialogName, "");
    return dir;
}

void KoFileDialog::saveUsedDir(const QString &fileName,
                               const QString &dialogName)
{

    if (dialogName.isEmpty()) return;

    QFileInfo fileInfo(fileName);
    KConfigGroup group =  KSharedConfig::openConfig()->group("File Dialogs");
    group.writeEntry(dialogName, fileInfo.absolutePath());

}

void KoFileDialog::onFilterSelected(const QString &filter)
{
    debugWidgetUtils << "KoFileDialog::onFilterSelected" << filter;

#ifndef Q_OS_ANDROID
    QFileDialog::FileMode mode = d->fileDialog->fileMode();
    if (mode != QFileDialog::Directory && !d->fileDialog->testOption(QFileDialog::ShowDirsOnly)) {
        if (d->suffixes.contains(filter)) {
            QString suffix = d->suffixes[filter];
            debugWidgetUtils << "  Setting default suffix to" << suffix;
            d->fileDialog->setDefaultSuffix(suffix);
        } else {
            warnWidgetUtils << "KoFileDialog::onFilterSelected: Cannot find suffix for filter" << filter;
            d->fileDialog->setDefaultSuffix("");
        }
    }
#endif
}
