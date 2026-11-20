
static void krimbleMirrorLayout(QMainWindow *win)
{
    struct Group { QDockWidget *first; QList<QDockWidget*> others; int y; };
    auto collect = [win](Qt::DockWidgetArea area) {
        QList<Group> groups;
        QList<QDockWidget*> seen;
        QList<QDockWidget*> docks = win->findChildren<QDockWidget*>();
        std::sort(docks.begin(), docks.end(), [](QDockWidget *a, QDockWidget *b) { return a->geometry().y() < b->geometry().y(); });
        Q_FOREACH (QDockWidget *dock, docks) {
            if (dock->isFloating() || win->dockWidgetArea(dock) != area || seen.contains(dock)) continue;
            Group g;
            g.first = dock;
            g.y = dock->geometry().y();
            seen.append(dock);
            Q_FOREACH (QDockWidget *tab, win->tabifiedDockWidgets(dock)) {
                g.others.append(tab);
                seen.append(tab);
            }
            groups.append(g);
        }
        return groups;
    };
    const QList<Group> leftGroups = collect(Qt::LeftDockWidgetArea);
    const QList<Group> rightGroups = collect(Qt::RightDockWidgetArea);
    auto place = [win](const QList<Group> &groups, Qt::DockWidgetArea to) {
        Q_FOREACH (const Group &g, groups) {
            win->addDockWidget(to, g.first);
            Q_FOREACH (QDockWidget *other, g.others) {
                win->addDockWidget(to, other);
                win->tabifyDockWidget(g.first, other);
            }
            g.first->show();
            g.first->raise();
        }
    };
    place(leftGroups, Qt::RightDockWidgetArea);
    place(rightGroups, Qt::LeftDockWidgetArea);
}


// "Where is...?": every menu entry with its path, searchable; tapping one opens it (Claude's idea).
static void krimbleCollectMenuActions(QMenu *menu, const QString &path, QList<QPair<QString, QPointer<QAction> > > &out, int depth)
{
    if (!menu || depth > 6) return;
    Q_EMIT menu->aboutToShow();    // some menus (Window, recent files) fill themselves when they are about to be shown
    Q_FOREACH (QAction *action, menu->actions()) {
        if (action->isSeparator()) continue;
        QString text = action->text();
        text.remove(QLatin1Char('&'));
        text = text.section(QLatin1Char('\t'), 0, 0).trimmed();
        if (action->menu()) {
            krimbleCollectMenuActions(action->menu(), path + text + QStringLiteral("  >  "), out, depth + 1);
        } else if (!text.isEmpty()) {
            out.append(qMakePair(path + text, QPointer<QAction>(action)));
        }
    }
}

static void krimbleShowWhereIs(QMainWindow *win)
{
    QSharedPointer<QList<QPair<QString, QPointer<QAction> > > > items(new QList<QPair<QString, QPointer<QAction> > >());
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        if (!top->menu()) continue;
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        krimbleCollectMenuActions(top->menu(), title + QStringLiteral("  >  "), *items, 0);
    }

    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Where is...?"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(6, 6, 6, 6);
    QLineEdit *edit = new QLineEdit(dialog);
    edit->setPlaceholderText(i18n("Type a feature name, for example Crop"));
    QListWidget *list = new QListWidget(dialog);
    layout->addWidget(edit);
    layout->addWidget(list, 1);

    auto refill = [items, edit, list]() {
        list->clear();
        const QString needle = edit->text().trimmed();
        int shown = 0;
        for (int i = 0; i < items->size() && shown < 80; ++i) {
            if (needle.isEmpty() || items->at(i).first.contains(needle, Qt::CaseInsensitive)) {
                QListWidgetItem *item = new QListWidgetItem(items->at(i).first, list);
                item->setData(Qt::UserRole, i);
                ++shown;
            }
        }
    };
    QObject::connect(edit, &QLineEdit::textChanged, dialog, refill);
    QObject::connect(list, &QListWidget::itemClicked, dialog, [items, dialog](QListWidgetItem *item) {
        QPointer<QAction> action = items->at(item->data(Qt::UserRole).toInt()).second;
        dialog->close();
        if (action && action->isEnabled()) {
            QTimer::singleShot(0, action.data(), [action]() { if (action) action->trigger(); });
        }
    });
    refill();
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        dialog->resize(int(avail.width() * 0.92), int(avail.height() * 0.80));
    }
    dialog->show();
    edit->setFocus();
}

static void krimbleShowCheatSheet(QMainWindow *win)
{
    QList<QPair<QString, QPointer<QAction> > > all;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        if (!top->menu()) continue;
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        krimbleCollectMenuActions(top->menu(), title + QStringLiteral("  >  "), all, 0);
    }
    // familiar name, a word that is in the menu entry's name
    static const char *const terms[][2] = {
        {"Crop", "Crop"}, {"Image Size", "Scale Image"}, {"Canvas Size", "Canvas Size"}, {"Rotate Image", "Rotate Image"},
        {"Flatten Image", "Flatten Image"}, {"Merge Visible", "Merge Visible"}, {"Merge Down", "Merge Down"},
        {"New Layer", "New Layer"}, {"Duplicate Layer", "Duplicate Layer"}, {"Layer Mask", "Mask"},
        {"Free Transform", "Free Transform"}, {"Warp", "Warp"}, {"Liquify", "Liquify"}, {"Flip", "Flip"},
        {"Select All", "Select All"}, {"Deselect", "Deselect"}, {"Inverse Selection", "Inverse"}, {"Feather", "Feather"},
        {"Color Range", "Color Range"}, {"Brightness/Contrast", "Brightness"}, {"Levels", "Levels"}, {"Curves", "Curves"},
        {"Hue/Saturation", "Hue"}, {"Color Balance", "Color Balance"}, {"Vibrance", "Vibrance"}, {"Invert", "Invert"},
        {"Gaussian Blur", "Gaussian"}, {"Unsharp Mask", "Unsharp"}, {"Noise", "Noise"}, {"Pixelate", "Pixelate"},
        {"Convert Color Mode", "Mode"}, {"Fill", "Fill"}, {"Stroke", "Stroke"}, {"Undo", "Undo"}, {"Copy Merged", "Copy Merged"},
        {"Paste as New Image", "Paste as New"}, {"Preferences", "Preferences"}
    };
    QSharedPointer<QList<QPair<QString, QPointer<QAction> > > > shown(new QList<QPair<QString, QPointer<QAction> > >());
    QStringList lines;
    for (const auto &term : terms) {
        for (const auto &entry : all) {
            if (entry.first.contains(QLatin1String(term[1]), Qt::CaseInsensitive)) {
                lines << QStringLiteral("%1   ->   %2").arg(QLatin1String(term[0]), entry.first);
                shown->append(entry);
                break;
            }
        }
    }
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Cheat Sheet"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(6, 6, 6, 6);
    QLabel *intro = new QLabel(i18n("The names you know from other graphics editors, and where each one is in Krimble. Tap a line to open it."), dialog);
    intro->setWordWrap(true);
    QListWidget *list = new QListWidget(dialog);
    list->addItems(lines);
    layout->addWidget(intro);
    layout->addWidget(list, 1);
    QObject::connect(list, &QListWidget::currentRowChanged, dialog, [shown, dialog](int row) {
        if (row < 0 || row >= shown->size()) return;
        QPointer<QAction> action = shown->at(row).second;
        dialog->close();
        if (action && action->isEnabled()) {
            QTimer::singleShot(0, action.data(), [action]() { if (action) action->trigger(); });
        }
    });
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        const QRect avail = scr->availableGeometry();
        dialog->resize(int(avail.width() * 0.92), int(avail.height() * 0.80));
    }
    dialog->show();
}

// "Share": the picture as it looks flattened, through Android's share sheet (Claude's idea; George: "Share out is great.").
static void krimbleShareImage(KisMainWindow *win, bool asJpg)
{
#ifdef Q_OS_ANDROID
    // KisImageSP image = win->viewManager() ? win->viewManager()->image() : KisImageSP();   (did not compile: weak and shared pointer in one ?:)
    KisImageSP image;
    if (win->viewManager()) {
        image = win->viewManager()->image();
    }
    if (!image) {
        QMessageBox::information(win, i18n("Share"), i18n("Open an image first."));
        return;
    }
    image->waitForDone();
    QImage flat = image->convertToQImage(image->bounds(), image->colorSpace()->profile());
    if (flat.isNull()) {
        QMessageBox::warning(win, i18n("Share"), i18n("The image could not be prepared for sharing."));
        return;
    }
    const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/share");
    QDir().mkpath(dirPath);
    QString base = QStringLiteral("Krimble-%1").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss")));
    const QString path = dirPath + QLatin1Char('/') + base + (asJpg ? QStringLiteral(".jpg") : QStringLiteral(".png"));
    bool saved;
    if (asJpg) {
        QImage onWhite(flat.size(), QImage::Format_RGB32);
        onWhite.fill(Qt::white);
        QPainter painter(&onWhite);
        painter.drawImage(0, 0, flat);
        painter.end();
        saved = onWhite.save(path, "JPG", 92);
    } else {
        saved = flat.save(path, "PNG");
    }
    if (!saved) {
        QMessageBox::warning(win, i18n("Share"), i18n("The image could not be saved for sharing."));
        return;
    }
    QAndroidJniObject jPath = QAndroidJniObject::fromString(path);
    QAndroidJniObject jMime = QAndroidJniObject::fromString(asJpg ? QStringLiteral("image/jpeg") : QStringLiteral("image/png"));
    QAndroidJniObject jName = QAndroidJniObject::fromString(base + (asJpg ? QStringLiteral(".jpg") : QStringLiteral(".png")));
    const jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>("org/krimble/android/ShareHelper", "shareFile",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", jPath.object<jstring>(), jMime.object<jstring>(), jName.object<jstring>());
    if (!ok) {
        QMessageBox::warning(win, i18n("Share"), i18n("Sharing did not work on this phone."));
    }
#else
    Q_UNUSED(win);
    Q_UNUSED(asJpg);
#endif
}

static void krimbleShareWithOptions(KisMainWindow *win, bool asJpg, int level, int maxSide, bool flatten)
{
#ifdef Q_OS_ANDROID
    KisImageSP image;
    if (win->viewManager()) {
        image = win->viewManager()->image();
    }
    if (!image) {
        QMessageBox::information(win, i18n("Share"), i18n("Open an image first."));
        return;
    }
    image->waitForDone();
    QImage flat = image->convertToQImage(image->bounds(), image->colorSpace()->profile());
    if (flat.isNull()) {
        QMessageBox::warning(win, i18n("Share"), i18n("The image could not be prepared for sharing."));
        return;
    }
    if (maxSide > 0 && qMax(flat.width(), flat.height()) > maxSide) {
        flat = flat.scaled(maxSide, maxSide, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/share");
    QDir().mkpath(dirPath);
    const QString base = QStringLiteral("Krimble-%1").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss")));
    const QString extension = asJpg ? QStringLiteral(".jpg") : QStringLiteral(".png");
    const QString path = dirPath + QLatin1Char('/') + base + extension;
    bool saved;
    if (asJpg) {
        QImage toSave = flat;
        if (flatten) {
            toSave = QImage(flat.size(), QImage::Format_RGB32);
            toSave.fill(Qt::white);
            QPainter painter(&toSave);
            painter.drawImage(0, 0, flat);
            painter.end();
        }
        saved = toSave.save(path, "JPG", level);
    } else {
        // Qt's PNG quality: 0 is the smallest file, 100 the largest; the slider's 9 is the smallest file
        saved = flat.save(path, "PNG", (9 - qBound(0, level, 9)) * 100 / 9);
    }
    if (!saved) {
        QMessageBox::warning(win, i18n("Share"), i18n("The image could not be saved for sharing."));
        return;
    }
    QAndroidJniObject jPath = QAndroidJniObject::fromString(path);
    QAndroidJniObject jMime = QAndroidJniObject::fromString(asJpg ? QStringLiteral("image/jpeg") : QStringLiteral("image/png"));
    QAndroidJniObject jName = QAndroidJniObject::fromString(base + extension);
    const jboolean ok = QAndroidJniObject::callStaticMethod<jboolean>("org/krimble/android/ShareHelper", "shareFile",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", jPath.object<jstring>(), jMime.object<jstring>(), jName.object<jstring>());
    if (!ok) {
        QMessageBox::warning(win, i18n("Share"), i18n("Sharing did not work on this phone."));
    }
#else
    Q_UNUSED(win); Q_UNUSED(asJpg); Q_UNUSED(level); Q_UNUSED(maxSide); Q_UNUSED(flatten);
#endif
}

static void krimbleShowShare(KisMainWindow *win)
{
    if (!win->viewManager() || !win->viewManager()->image()) {
        QMessageBox::information(win, i18n("Share"), i18n("Open an image first."));
        return;
    }
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Share"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(8, 8, 8, 8);
    QComboBox *format = new QComboBox(dialog);
    format->addItem(i18n("JPG"));
    format->addItem(i18n("PNG"));
    QLabel *levelLabel = new QLabel(dialog);
    QSlider *level = new QSlider(Qt::Horizontal, dialog);
    QComboBox *size = new QComboBox(dialog);
    size->addItem(i18n("Original size"), 0);
    size->addItem(i18n("Longest side 1920 px"), 1920);
    size->addItem(i18n("Longest side 1280 px"), 1280);
    size->addItem(i18n("Longest side 1080 px"), 1080);
    QCheckBox *flatten = new QCheckBox(i18n("Flatten onto white"), dialog);
    flatten->setChecked(true);
    auto updateLevelLabel = [format, levelLabel, level]() {
        levelLabel->setText(format->currentIndex() == 0 ? i18n("Quality: %1", level->value()) : i18n("Compression: %1", level->value()));
    };
    auto updateFormat = [format, level, flatten, updateLevelLabel]() {
        const bool jpg = format->currentIndex() == 0;
        flatten->setEnabled(jpg);
        if (jpg) {
            level->setRange(10, 100);
            level->setValue(90);
        } else {
            level->setRange(0, 9);
            level->setValue(5);
        }
        updateLevelLabel();
    };
    QObject::connect(format, QOverload<int>::of(&QComboBox::currentIndexChanged), dialog, [updateFormat](int) { updateFormat(); });
    QObject::connect(level, &QSlider::valueChanged, dialog, [updateLevelLabel](int) { updateLevelLabel(); });
    updateFormat();
    QDialogButtonBox *buttons = new QDialogButtonBox(dialog);
    QPushButton *shareButton = buttons->addButton(i18n("Share"), QDialogButtonBox::AcceptRole);
    buttons->addButton(QDialogButtonBox::Cancel);
    Q_UNUSED(shareButton);
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, [win, dialog, format, level, size, flatten]() {
        const bool jpg = format->currentIndex() == 0;
        const int value = level->value();
        const int maxSide = size->currentData().toInt();
        const bool flat = flatten->isChecked();
        dialog->accept();
        krimbleShareWithOptions(win, jpg, value, maxSide, flat);
    });
    layout->addWidget(format);
    layout->addWidget(levelLabel);
    layout->addWidget(level);
    layout->addWidget(size);
    layout->addWidget(flatten);
    layout->addWidget(buttons);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog->resize(int(scr->availableGeometry().width() * 0.9), dialog->sizeHint().height() + 20);
    }
    dialog->show();
}

static void krimbleShowBatch(KisMainWindow *win)
{
#ifdef Q_OS_ANDROID
    auto pickPictures = [win]() -> QStringList {
        KoFileDialog fileDialog(win, KoFileDialog::OpenFiles, "KrimbleBatch");
        fileDialog.setCaption(i18n("Choose pictures"));
        fileDialog.setMimeTypeFilters(QStringList() << QStringLiteral("image/jpeg") << QStringLiteral("image/png")
                                                    << QStringLiteral("image/webp") << QStringLiteral("image/bmp"));
        return fileDialog.filenames();
    };
    auto savePicture = [](const QString &temp, const QString &mime, const QString &name, const QString &folder) -> bool {
        QAndroidJniObject jPath = QAndroidJniObject::fromString(temp);
        QAndroidJniObject jMime = QAndroidJniObject::fromString(mime);
        QAndroidJniObject jName = QAndroidJniObject::fromString(name);
        QAndroidJniObject jFolder = QAndroidJniObject::fromString(folder);   // "Batch", or the folder a script names
        return QAndroidJniObject::callStaticMethod<jboolean>("org/krimble/android/ShareHelper", "saveToPictures",
            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
            jPath.object<jstring>(), jMime.object<jstring>(), jName.object<jstring>(), jFolder.object<jstring>());
    };
    KrimbleBatchDialog *batchDialog = new KrimbleBatchDialog(win, pickPictures, savePicture, QStringLiteral("Pictures/Krimble"));
    batchDialog->show();
#if 0
    struct Preset { const char *label; int width; bool jpg; int quality; const char *suffix; };
    static const Preset presets[] = {
        { "HD 1080p: longest side 1920 px (JPG)",    1920, true,  90, "hd" },
        { "Social: longest side 1080 px (JPG)",      1080, true,  90, "social" },
        { "Messages: longest side 1280 px (JPG 80%)", 1280, true,  80, "msg" },
        { "Thumbnail: longest side 256 px (PNG)",     256, false, 100, "thumb" },
        { "Convert to JPG (same size)",                0, true,  92, "jpg" },
        { "Convert to PNG (same size)",                0, false, 100, "png" }
    };
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Batch Process"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(8, 8, 8, 8);
    QLabel *intro = new QLabel(i18n("Pick a preset, then the pictures. Results go to Pictures/Krimble/Batch; the originals are not changed."), dialog);
    intro->setWordWrap(true);
    QComboBox *combo = new QComboBox(dialog);
    for (const Preset &p : presets) combo->addItem(i18n(p.label));
    QPushButton *start = new QPushButton(i18n("Choose Pictures and Start..."), dialog);
    QProgressBar *bar = new QProgressBar(dialog);
    bar->setRange(0, 100);
    bar->setValue(0);
    QLabel *status = new QLabel(dialog);
    status->setWordWrap(true);
    QPushButton *cancel = new QPushButton(i18n("Cancel"), dialog);
    cancel->setEnabled(false);
    QPushButton *closeButton = new QPushButton(i18n("Close"), dialog);
    QHBoxLayout *buttonRow = new QHBoxLayout;
    buttonRow->addWidget(cancel);
    buttonRow->addWidget(closeButton);
    layout->addWidget(intro);
    layout->addWidget(combo);
    layout->addWidget(start);
    layout->addWidget(bar);
    layout->addWidget(status);
    layout->addLayout(buttonRow);
    layout->addStretch(1);
    QSharedPointer<bool> cancelRequested(new bool(false));
    QPointer<QDialog> guard(dialog);
    QObject::connect(closeButton, &QPushButton::clicked, dialog, [dialog]() { dialog->close(); });
    QObject::connect(cancel, &QPushButton::clicked, dialog, [cancel, cancelRequested]() {
        *cancelRequested = true;
        cancel->setEnabled(false);
    });
    // QObject::connect(start, &QPushButton::clicked, dialog, [win, dialog, combo, start, bar, status]() {   (the first version, without Cancel)
    QObject::connect(start, &QPushButton::clicked, dialog, [win, dialog, combo, start, bar, status, cancel, closeButton, cancelRequested, guard]() {
        KoFileDialog fileDialog(win, KoFileDialog::OpenFiles, "KrimbleBatch");
        fileDialog.setCaption(i18n("Choose pictures"));
        fileDialog.setMimeTypeFilters(QStringList() << QStringLiteral("image/jpeg") << QStringLiteral("image/png")
                                                    << QStringLiteral("image/webp") << QStringLiteral("image/bmp"));
        const QStringList files = fileDialog.filenames();
        if (files.isEmpty()) return;
        const Preset &preset = presets[qBound(0, combo->currentIndex(), 5)];
        start->setEnabled(false);
        closeButton->setEnabled(false);
        cancel->setEnabled(true);
        *cancelRequested = false;
        const QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QStringLiteral("/batch");
        QDir().mkpath(dirPath);
        int done = 0, failed = 0;
        for (int i = 0; i < files.size(); ++i) {
            status->setText(i18n("Picture %1 of %2...", i + 1, files.size()));
            bar->setValue(100 * i / files.size());
            QCoreApplication::processEvents();
            if (!guard) return;                  // the window was closed (Android's back button) while a picture was processed
            if (*cancelRequested) break;
            QImage image(files.at(i));
            if (image.isNull()) { ++failed; continue; }
            // if (preset.width > 0 && image.width() > preset.width) {
            //     image = image.scaledToWidth(preset.width, Qt::SmoothTransformation);
            // }
            if (preset.width > 0 && qMax(image.width(), image.height()) > preset.width) {   // preset.width is the longest side
                image = image.scaled(preset.width, preset.width, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            if (preset.jpg) {
                QImage onWhite(image.size(), QImage::Format_RGB32);
                onWhite.fill(Qt::white);
                QPainter painter(&onWhite);
                painter.drawImage(0, 0, image);
                painter.end();
                image = onWhite;
            }
            QString base = QFileInfo(files.at(i)).completeBaseName();
            if (base.isEmpty()) base = QStringLiteral("picture");
            const QString name = QStringLiteral("%1-%2.%3").arg(base, QLatin1String(preset.suffix), preset.jpg ? QStringLiteral("jpg") : QStringLiteral("png"));
            const QString temp = dirPath + QLatin1Char('/') + name;
            const bool saved = preset.jpg ? image.save(temp, "JPG", preset.quality) : image.save(temp, "PNG");
            bool stored = false;
            if (saved) {
                QAndroidJniObject jPath = QAndroidJniObject::fromString(temp);
                QAndroidJniObject jMime = QAndroidJniObject::fromString(preset.jpg ? QStringLiteral("image/jpeg") : QStringLiteral("image/png"));
                QAndroidJniObject jName = QAndroidJniObject::fromString(name);
                QAndroidJniObject jFolder = QAndroidJniObject::fromString(QStringLiteral("Batch"));
                stored = QAndroidJniObject::callStaticMethod<jboolean>("org/krimble/android/ShareHelper", "saveToPictures",
                    "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
                    jPath.object<jstring>(), jMime.object<jstring>(), jName.object<jstring>(), jFolder.object<jstring>());
                QFile::remove(temp);
            }
            if (stored) ++done; else ++failed;
        }
        if (!guard) return;
        const bool wasCancelled = *cancelRequested;
        bar->setValue(wasCancelled ? bar->value() : 100);
        status->setText(wasCancelled ? i18n("Cancelled: %1 saved in Pictures/Krimble/Batch, %2 skipped, the rest not processed.", done, failed)
                                     : i18n("Done: %1 saved in Pictures/Krimble/Batch, %2 skipped.", done, failed));
        cancel->setEnabled(false);
        closeButton->setEnabled(true);
        start->setEnabled(true);
    });
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog->resize(int(scr->availableGeometry().width() * 0.92), dialog->sizeHint().height() + 30);
    }
    dialog->show();
#endif
#else
    Q_UNUSED(win);
#endif
}

#ifdef Q_OS_ANDROID
class KrimbleCameraReceiver : public QAndroidActivityResultReceiver
{
public:
    KrimbleCameraReceiver(KisMainWindow *win, const QString &uri) : m_win(win), m_uri(uri) {}
    void handleActivityResult(int requestCode, int resultCode, const QAndroidJniObject &data) override
    {
        Q_UNUSED(data);
        const bool taken = (requestCode == 4711 && resultCode == -1);    // -1 is RESULT_OK
        if (taken) {
            KisMainWindow *window = m_win.data();
            const QString uri = m_uri;
            QAndroidJniObject jUriString = QAndroidJniObject::fromString(uri);
            QAndroidJniObject jResult = QAndroidJniObject::callStaticObjectMethod("org/krimble/android/ShareHelper", "copyCameraPhoto",
                "(Ljava/lang/String;)Ljava/lang/String;", jUriString.object<jstring>());
            const QString result = jResult.isValid() ? jResult.toString() : QStringLiteral("ERROR: no answer from the helper");
            if (!result.startsWith(QLatin1String("ERROR")) && result != QLatin1String("EMPTY")) {
                if (window) {
                    QMetaObject::invokeMethod(window, [window, result]() {
                        window->openDocument(result, KisMainWindow::OpenFlags());
                    }, Qt::QueuedConnection);
                }
            } else {
                if (result == QLatin1String("EMPTY")) {
                    QAndroidJniObject::callStaticMethod<void>("org/krimble/android/ShareHelper", "deleteUri", "(Ljava/lang/String;)V", jUriString.object<jstring>());
                }
                if (window) {
                    const QString message = (result == QLatin1String("EMPTY"))
                        ? i18n("The camera app said it took the photo but saved nothing to the place Krimble gave it.")
                        : result;
                    QMetaObject::invokeMethod(window, [window, message]() {
                        QMessageBox::warning(window, i18n("Take Photo"), message);
                    }, Qt::QueuedConnection);
                }
            }
        } else {
            QAndroidJniObject jUri = QAndroidJniObject::fromString(m_uri);
            QAndroidJniObject::callStaticMethod<void>("org/krimble/android/ShareHelper", "deleteUri", "(Ljava/lang/String;)V", jUri.object<jstring>());
        }
        // (the receiver is not deleted here: a few bytes per photo are left; the camera app has finished with it)
    }
private:
    QPointer<KisMainWindow> m_win;
    QString m_uri;
};

static void krimbleTakePhoto(KisMainWindow *win)
{
    QAndroidJniObject jUri = QAndroidJniObject::callStaticObjectMethod("org/krimble/android/ShareHelper", "prepareCameraUri", "()Ljava/lang/String;");
    const QString uri = jUri.isValid() ? jUri.toString() : QString();
    if (uri.isEmpty()) {
        QMessageBox::warning(win, i18n("Take Photo"), i18n("The camera cannot be started on this phone (it needs Android 10 or newer)."));
        return;
    }
    QAndroidJniObject action = QAndroidJniObject::getStaticObjectField<jstring>("android/provider/MediaStore", "ACTION_IMAGE_CAPTURE");
    QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", action.object<jstring>());
    QAndroidJniObject extraOutput = QAndroidJniObject::getStaticObjectField<jstring>("android/provider/MediaStore", "EXTRA_OUTPUT");
    QAndroidJniObject uriObject = QAndroidJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", jUri.object<jstring>());
    intent.callObjectMethod("putExtra", "(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;", extraOutput.object<jstring>(), uriObject.object());
    intent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", 3);      // read and write access to that address
    QtAndroid::startActivity(intent, 4711, new KrimbleCameraReceiver(win, uri));
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {          // no camera app
        env->ExceptionClear();
        QAndroidJniObject::callStaticMethod<void>("org/krimble/android/ShareHelper", "deleteUri", "(Ljava/lang/String;)V", jUri.object<jstring>());
        QMessageBox::warning(win, i18n("Take Photo"), i18n("No camera app was found on this phone."));
    }
}
#endif

// "Upscale...": makes the picture 2, 3 or 4 times larger with the resize method of your choice (Claude's idea; George: "Yes. Go for it.").
// Lanczos3 is the default; the other methods are the ones of Image > Image Size. Optionally a light sharpen layer is put on top
// (a layer, so it can be hidden or removed).
static void krimbleShowUpscale(KisMainWindow *win)
{
    KisViewManager *view = win->viewManager();
    if (!view || !view->image()) {
        QMessageBox::information(win, i18n("Upscale"), i18n("Open a picture first."));
        return;
    }
    KisImageSP image = view->image();
    QDialog *dialog = new QDialog(win);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(i18n("Upscale"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(8, 8, 8, 8);
    QLabel *current = new QLabel(i18n("Now: %1 x %2 px", image->width(), image->height()), dialog);
    QComboBox *factor = new QComboBox(dialog);
    factor->addItem(i18n("2 times larger"), 2);
    factor->addItem(i18n("3 times larger"), 3);
    factor->addItem(i18n("4 times larger"), 4);
    QComboBox *method = new QComboBox(dialog);
    KisFilterStrategyRegistry *strategies = KisFilterStrategyRegistry::instance();
    int lanczosIndex = 0;
    Q_FOREACH (const QString &id, strategies->keys()) {
        if (KisFilterStrategy *strategy = strategies->get(id)) {
            method->addItem(strategy->name(), id);
            if (id == QLatin1String("Lanczos3")) lanczosIndex = method->count() - 1;
        }
    }
    method->setCurrentIndex(lanczosIndex);
    QCheckBox *sharpen = new QCheckBox(i18n("Add a light sharpen layer on top"), dialog);
    sharpen->setChecked(true);
    QLabel *result = new QLabel(dialog);
    auto updateResult = [image, factor, result]() {
        const int f = factor->currentData().toInt();
        result->setText(i18n("Will be: %1 x %2 px", image->width() * f, image->height() * f));
    };
    QObject::connect(factor, QOverload<int>::of(&QComboBox::currentIndexChanged), dialog, updateResult);
    updateResult();
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    // (the lambda is "mutable": it keeps its own copy of the image pointer and calls waitForDone() on it, which a const copy does not allow)
    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, [win, view, image, dialog, factor, method, sharpen]() mutable {
        const int f = factor->currentData().toInt();
        const qint64 newWidth = qint64(image->width()) * f;
        const qint64 newHeight = qint64(image->height()) * f;
        if (newWidth * newHeight > 100LL * 1000 * 1000) {
            QMessageBox::warning(dialog, i18n("Upscale"), i18n("The result would be larger than 100 million pixels, which a phone cannot hold. Choose a smaller factor."));
            return;
        }
        KisFilterStrategy *strategy = KisFilterStrategyRegistry::instance()->get(method->currentData().toString());
        if (!strategy) return;
        const bool addSharpen = sharpen->isChecked();
        dialog->accept();
        view->imageManager()->scaleCurrentImage(QSize(int(newWidth), int(newHeight)), image->xRes(), image->yRes(), strategy);
        if (addSharpen) {
            image->waitForDone();
            KisFilterSP filter = KisFilterRegistry::instance()->value(QStringLiteral("unsharp"));
            if (filter) {
                KisFilterConfigurationSP config = filter->defaultConfiguration(KisGlobalResourcesInterface::instance());
                KisAdjustmentLayerSP sharpenLayer = new KisAdjustmentLayer(image, i18n("Upscale Sharpen"), config, KisSelectionSP());
                KisNodeCommandsAdapter adapter(view);
                adapter.addNode(sharpenLayer, image->root(), image->root()->lastChild());
            }
        }
    });
    layout->addWidget(current);
    layout->addWidget(factor);
    layout->addWidget(method);
    layout->addWidget(sharpen);
    layout->addWidget(result);
    layout->addWidget(buttons);
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        dialog->resize(int(scr->availableGeometry().width() * 0.9), dialog->sizeHint().height() + 20);
    }
    dialog->show();
}

// Adds "Share as JPG" and "Share as PNG" to the File menu, just above Quit (Claude's idea).
static void krimbleInstallFileExtras(KisMainWindow *win)
{
    QMenu *file = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("File")) file = top->menu();
    }
    if (!file || file->property("krimbleFileExtras").toBool()) return;
    file->setProperty("krimbleFileExtras", true);
    QObject::connect(file, &QMenu::aboutToShow, win, [win, file]() {
        if (file->property("krimbleFileExtrasAdded").toBool()) return;
        file->setProperty("krimbleFileExtrasAdded", true);
        QAction *before = nullptr;
        Q_FOREACH (QAction *action, file->actions()) {
            QString text = action->text();
            text.remove(QLatin1Char('&'));
            if (text == QLatin1String("Quit") || text == QLatin1String("Exit")) before = action;
        }
        {
            QAction *shareAction = new QAction(KisIconUtils::loadIcon(QStringLiteral("krimble-share")), i18n("Share..."), file);
            QObject::connect(shareAction, &QAction::triggered, win, [win]() { krimbleShowShare(win); });
            QAction *afterExport = nullptr;
            const QList<QAction*> fileActions = file->actions();
            for (int i = 0; i < fileActions.size(); ++i) {
                QString text = fileActions.at(i)->text();
                text.remove(QLatin1Char('&'));
                if (text.startsWith(QLatin1String("Export Options")) && i + 1 < fileActions.size()) afterExport = fileActions.at(i + 1);
            }
            Q_UNUSED(afterExport);
            Q_UNUSED(shareAction);
        }
        QAction *shareJpg = new QAction(i18n("Share as JPG..."), file);
        QObject::connect(shareJpg, &QAction::triggered, win, [win]() { krimbleShareImage(win, true); });
        QAction *sharePng = new QAction(i18n("Share as PNG..."), file);
        QObject::connect(sharePng, &QAction::triggered, win, [win]() { krimbleShareImage(win, false); });
        QAction *takePhoto = new QAction(i18n("Take Photo..."), file);
        QObject::connect(takePhoto, &QAction::triggered, win, [win]() { krimbleTakePhoto(win); });
        file->insertAction(before, takePhoto);
        QAction *shareLast = new QAction(KisIconUtils::loadIcon(QStringLiteral("krimble-share")), i18n("Share..."), file);
        QObject::connect(shareLast, &QAction::triggered, win, [win]() { krimbleShowShare(win); });
        file->insertAction(before, shareLast);
    });
}

// "Send Feedback": opens a prefilled GitHub issue (Claude's idea).
static void krimbleSendFeedback(QMainWindow *win)
{
    QString device = QSysInfo::prettyProductName();
    if (QScreen *scr = win->screen()) {
        device += QStringLiteral(", screen %1x%2, density %3").arg(scr->size().width()).arg(scr->size().height()).arg(scr->devicePixelRatio());
    }
    device += QStringLiteral(", window %1x%2").arg(win->width()).arg(win->height());
    const QString version = KritaVersionWrapper::versionString();
    const QString body = QStringLiteral("**What happened / what would help:**\n\n\n\n---\nKrimble %1\n%2\n").arg(version, device);
    QUrl url(QStringLiteral("https://github.com/purdygeorgeedward-dev/krimble/issues/new"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("title"), QStringLiteral("Feedback (Krimble %1)").arg(version));
    query.addQueryItem(QStringLiteral("body"), body);
    url.setQuery(query);
    QDesktopServices::openUrl(url);
}

// The QA overlay: live numbers on the screen, for screenshots (Claude's idea).
static QString krimbleQaReport(QMainWindow *win)
{
    QString r;
    const qreal dpr = win->devicePixelRatioF();
    r += QStringLiteral("%1  window %2x%3  density %4  (sizes are logical px; x density = device px)\n")
            .arg(win->width() > win->height() ? QStringLiteral("LANDSCAPE") : QStringLiteral("PORTRAIT"))
            .arg(win->width()).arg(win->height()).arg(dpr);
    if (QScreen *scr = win->screen()) {
        r += QStringLiteral("screen %1x%2  avail %3x%4\n").arg(scr->size().width()).arg(scr->size().height())
                .arg(scr->availableGeometry().width()).arg(scr->availableGeometry().height());
    }
    if (QWidget *central = win->centralWidget()) {
        r += QStringLiteral("central %1x%2  min %3\n").arg(central->width()).arg(central->height()).arg(central->minimumSizeHint().width());
    }
    Q_FOREACH (QDockWidget *dock, win->findChildren<QDockWidget*>()) {
        if (!dock->isVisible()) continue;
        QString where = dock->isFloating() ? QStringLiteral("float") : QString::number(int(win->dockWidgetArea(dock)));
        QWidget *bevelOverlay = dock->findChild<QWidget*>(QStringLiteral("krimbleBevelOverlay"), Qt::FindDirectChildrenOnly);
        const QString bevelState = !bevelOverlay ? QStringLiteral("no bevel") : (bevelOverlay->isVisible() ? QStringLiteral("bevel on") : QStringLiteral("bevel hidden"));
        r += QStringLiteral("%1 %2x%3  min %4  max %5  %6  %7\n").arg(dock->objectName()).arg(dock->width()).arg(dock->height())
                .arg(dock->minimumSizeHint().width()).arg(dock->maximumWidth() > 100000 ? QStringLiteral("-") : QString::number(dock->maximumWidth())).arg(where).arg(bevelState);
    }
    Q_FOREACH (QToolBar *bar, win->findChildren<QToolBar*>()) {
        if (!bar->isVisible()) continue;
        r += QStringLiteral("bar %1 %2x%3\n").arg(bar->objectName()).arg(bar->width()).arg(bar->height());
    }
#ifdef Q_OS_ANDROID
    Q_FOREACH (QWidget *grab, win->findChildren<QWidget*>(QRegularExpression(QStringLiteral("^krimbleEdgeGrab_")), Qt::FindDirectChildrenOnly)) {
        r += static_cast<KrimbleDockEdgeGrab*>(grab)->status() + QLatin1Char('\n');
    }
#endif
    if (QWidget *active = QApplication::activeWindow()) {
        if (QDialog *dlg = qobject_cast<QDialog*>(active)) {
            r += QStringLiteral("DIALOG %1 %2x%3  minHint %4x%5\n").arg(QString::fromLatin1(dlg->metaObject()->className())).arg(dlg->width()).arg(dlg->height())
                    .arg(dlg->minimumSizeHint().width()).arg(dlg->minimumSizeHint().height());
            QList<QPair<int, QString> > widest;
            Q_FOREACH (QWidget *child, dlg->findChildren<QWidget*>()) {
                if (!child->isVisible()) continue;
                widest.append(qMakePair(child->minimumSizeHint().width(),
                    QStringLiteral("%1 '%2'").arg(QString::fromLatin1(child->metaObject()->className()), child->objectName())));
            }
            std::sort(widest.begin(), widest.end(), [](const QPair<int, QString> &a, const QPair<int, QString> &b) { return a.first > b.first; });
            for (int i = 0; i < widest.size() && i < 8; ++i) {
                r += QStringLiteral("  widest %1: %2\n").arg(widest[i].first).arg(widest[i].second);
            }
        }
    }
    return r;
}

static void krimbleSetQaOverlay(QMainWindow *win, bool on)
{
    QLabel *label = win->findChild<QLabel*>(QStringLiteral("krimbleQaOverlay"), Qt::FindDirectChildrenOnly);
    if (!on) {
        if (label) label->deleteLater();
        return;
    }
    if (label) return;
    label = new QLabel(win);
    label->setObjectName(QStringLiteral("krimbleQaOverlay"));
    label->setAttribute(Qt::WA_TransparentForMouseEvents);
    label->setStyleSheet(QStringLiteral("background-color: rgba(0, 0, 0, 190); color: #9CFF9C; padding: 6px; font-family: monospace; font-size: 13px;"));
    label->move(6, 60);
    QTimer *timer = new QTimer(label);
    QObject::connect(timer, &QTimer::timeout, label, [win, label]() {
        label->setText(krimbleQaReport(win));
        label->adjustSize();
        label->raise();
    });
    timer->start(500);
    label->setText(krimbleQaReport(win));
    label->adjustSize();
    label->show();
    label->raise();
}

// Adds the three entries to the end of the Help menu each time it opens (once).
static void krimbleInstallHelpExtras(QMainWindow *win)
{
    QMenu *help = nullptr;
    Q_FOREACH (QAction *top, win->menuBar()->actions()) {
        QString title = top->text();
        title.remove(QLatin1Char('&'));
        if (top->menu() && title == QLatin1String("Help")) help = top->menu();
    }
    if (!help || help->property("krimbleHelpExtras").toBool()) return;
    help->setProperty("krimbleHelpExtras", true);
    QObject::connect(help, &QMenu::aboutToShow, win, [win, help]() {
        if (help->property("krimbleHelpExtrasAdded").toBool()) return;
        help->setProperty("krimbleHelpExtrasAdded", true);
        help->addSeparator();
#if 0
        QAction *menuDiag = help->addAction(i18n("Menu Diagnostics"));
        QObject::connect(menuDiag, &QAction::triggered, win, [win]() {
            QMenu *edit = nullptr;
            Q_FOREACH (QAction *top, win->menuBar()->actions()) {
                QString title = top->text();
                title.remove(QLatin1Char('&'));
                if (top->menu() && title == QLatin1String("Edit")) edit = top->menu();
            }
            if (!edit) return;
            const QFontMetrics fm(edit->font());
            int widest = 0, withTab = 0, shortcutVisible = 0, withShortcut = 0, withIcon = 0;
            Q_FOREACH (QAction *a, edit->actions()) {
                widest = qMax(widest, fm.horizontalAdvance(a->text().remove(QLatin1Char('&'))));
                if (a->text().contains(QLatin1Char('\t'))) ++withTab;
                if (a->isShortcutVisibleInContextMenu()) ++shortcutVisible;
                if (!a->shortcut().isEmpty()) ++withShortcut;
                if (!a->icon().isNull()) ++withIcon;
            }
            const QString report = QStringLiteral(
                "Edit menu\nsizeHint width: %1\nwidest text: %2\nactions: %3\nwith a tab in the text: %4\nshortcut shown: %5\nwith a shortcut: %6\nwith an icon: %7\n"
                "menu style: %8\napp style: %9\napp stylesheet length: %10\nmenu stylesheet length: %11\ndevice pixel ratio: %12\nfont px: %13")
                .arg(edit->sizeHint().width()).arg(widest).arg(edit->actions().size()).arg(withTab).arg(shortcutVisible).arg(withShortcut).arg(withIcon)
                .arg(QString::fromLatin1(edit->style()->metaObject()->className())).arg(QString::fromLatin1(qApp->style()->metaObject()->className()))
                .arg(qApp->styleSheet().length()).arg(edit->styleSheet().length()).arg(qApp->devicePixelRatio()).arg(edit->font().pixelSize());
            QMessageBox::information(win, i18n("Menu Diagnostics"), report);
        });
#endif
        QAction *overlay = help->addAction(i18n("QA Overlay"));
        overlay->setCheckable(true);
        overlay->setChecked(win->findChild<QLabel*>(QStringLiteral("krimbleQaOverlay"), Qt::FindDirectChildrenOnly) != nullptr);
        QObject::connect(overlay, &QAction::toggled, win, [win](bool on) { krimbleSetQaOverlay(win, on); });
    });
}

