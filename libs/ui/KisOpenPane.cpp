/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Peter Simonsson <psn@linux.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QApplication>
#include "KisOpenPane.h"

#include <QLayout>
#include <QImage>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStyledItemDelegate>
#include <QLinearGradient>
#include <QStandardPaths>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QScrollArea>
#include <QMimeData>

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kis_debug.h>
#include <QUrl>


#include <KisKineticScroller.h>
#include <KoFileDialog.h>
#include <KoIcon.h>
#include "KisTemplateTree.h"
#include "KisTemplateGroup.h"
#include "KisTemplate.h"
#include "KisDetailsPane.h"
#include "KisTemplatesPane.h"
#include "ui_KisOpenPaneBase.h"

#include <limits.h>
#include <kconfiggroup.h>

#include <kis_icon.h>

class KoSectionListItem : public QTreeWidgetItem
{
public:
    KoSectionListItem(QTreeWidget* treeWidget, const QString& name, QString untranslatedName, int sortWeight, int widgetIndex = -1)
        : QTreeWidgetItem(treeWidget, QStringList() << name)
        , m_sortWeight(sortWeight)
        , m_widgetIndex(widgetIndex)
        , m_untranslatedName(untranslatedName)
    {
        Qt::ItemFlags newFlags = Qt::NoItemFlags;

        if(m_widgetIndex >= 0)
            newFlags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        setFlags(newFlags);
    }

    bool operator<(const QTreeWidgetItem & other) const override {
        const KoSectionListItem* item = dynamic_cast<const KoSectionListItem*>(&other);

        if (!item)
            return 0;

        return ((item->sortWeight() - sortWeight()) < 0);
    }

    int sortWeight() const {
        return m_sortWeight;
    }

    int widgetIndex() const {
        return m_widgetIndex;
    }

    QString untranslatedName() const {
        return m_untranslatedName;
    }

private:
    int m_sortWeight {0};
    int m_widgetIndex {-1};
    QString m_untranslatedName;
};

class KisOpenPanePrivate : public Ui_KisOpenPaneBase
{
public:
    KisOpenPanePrivate() :
        Ui_KisOpenPaneBase()
    {
        m_itemMap = new QMap<QString, QTreeWidgetItem *>();
    }

    ~KisOpenPanePrivate()
    {
        delete m_itemMap;
    }

    int m_freeCustomWidgetIndex {0};
    KoSectionListItem* m_templatesSeparator {nullptr};

    //Stores pointers to items in the pane, accessible by their untranslated name
    QMap<QString, QTreeWidgetItem *> *m_itemMap;
};

// Krimble 2026-10-12: a delegate that puts the icon above the text (Android only; elsewhere it behaves like the plain one).
class KrimbleIconAboveTextDelegate : public QStyledItemDelegate
{
public:
    explicit KrimbleIconAboveTextDelegate(QObject *parent) : QStyledItemDelegate(parent) {}
protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);
#ifdef Q_OS_ANDROID
        option->decorationPosition = QStyleOptionViewItem::Top;
        option->displayAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        option->decorationAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
        option->features |= QStyleOptionViewItem::WrapText;
#endif
    }
};

KisOpenPane::KisOpenPane(QWidget *parent, const QStringList& mimeFilter, const QString& templatesResourcePath)
    : QDialog(parent)
    , d(new KisOpenPanePrivate)
{
    d->setupUi(this);

#ifdef Q_OS_ANDROID
    // KRIMBLE 2026-10-02: the File > New pane was wider than a portrait screen.
    // Cap the left list (Custom Document, templates...) to 22% of the screen
    // width; longer names are elided. The cap is larger than the list's natural
    // width on a landscape screen, so landscape is unaffected.
    if (QScreen *scr = QGuiApplication::primaryScreen()) {
        // d->m_sectionList->setMaximumWidth(int(scr->availableGeometry().width() * 0.22));
        // d->m_sectionList->setMaximumWidth(int(scr->availableGeometry().width() * 0.20));   // Krimble 2026-10-12: thinner
        // Krimble 2026-10-12 (2nd): George's 1.0.34 screenshots: "That column of colored icons in the dialog is still too wide" (it
        // was about half the screen). In portrait the list is exactly 22% of the screen's width, whatever it would like to be; in
        // landscape the old cap stays.
        const int screenWidth = scr->availableGeometry().width();
        if (screenWidth < 450) {
            d->m_sectionList->setMinimumWidth(0);
            // d->m_sectionList->setFixedWidth(int(screenWidth * 0.22));
            // d->m_sectionList->setFixedWidth(int(screenWidth * 0.34));   // Krimble 2026-10-14: portrait (the screenshots): 34% too, not 22%
            // d->m_sectionList->setFixedWidth(int(screenWidth * 0.42));      // Krimble 2026-10-18: 42%, so that the 96 px icons and their names fit
            d->m_sectionList->setFixedWidth(int(screenWidth * 0.46));         // Krimble 2026-10-18 (2nd): 46%, for the 112 px icons
        } else {
            // d->m_sectionList->setMaximumWidth(int(screenWidth * 0.20));
            // Krimble 2026-10-14: George (1.0.36): "The new icons are way too small on the new file ... dialogs. Column area for them is too narrow
            // on one dialog." and "Need big icons. Small screen." The list is 34% of the screen wide, so the icons and their names are not cut.
            d->m_sectionList->setMaximumWidth(int(screenWidth * 0.34));
            d->m_sectionList->setMinimumWidth(int(screenWidth * 0.20));   // Krimble 2026-10-18: wide enough for the 96 px icons
        }
    }
    // Krimble 2026-10-12: George: the New Document window wastes a lot of space. Small margins and spacing around and between
    // its parts.
    if (layout()) {
        layout()->setContentsMargins(4, 4, 4, 4);
        layout()->setSpacing(4);
    }
#endif

    m_mimeFilter = mimeFilter;

    // QStyledItemDelegate* delegate = new QStyledItemDelegate(d->m_sectionList);
    // Krimble 2026-10-12: on Android the left list shows the text under the icon, centered and wrapped, so the list can be thin.
    QStyledItemDelegate* delegate = new KrimbleIconAboveTextDelegate(d->m_sectionList);
    d->m_sectionList->setItemDelegate(delegate);
#ifdef Q_OS_ANDROID
    // Krimble 2026-10-18 (3rd): George (1.0.37, c666301): "1. File - New dialog still has tiny icons. It must be hard coded somewhere." It was: KisOpenPaneBase.ui gives the
    // list (m_sectionList) an iconSize of 48 x 48, and a list draws its icons at that size whatever the size of the picture (so the 64, 96 and 112 px pictures of the 150th
    // entry were drawn at 48 logical px). The list's icon size is now 112 x 112, the size of the pictures.
    d->m_sectionList->setIconSize(QSize(112, 112));
#endif
    QScroller *scroller = KisKineticScroller::createPreconfiguredScroller(d->m_sectionList);
    if (scroller) {
        connect(scroller, &QScroller::stateChanged, this, [&](QScroller::State state) {
            KisKineticScroller::updateCursor(this, state);
        });
    }

    connect(d->m_sectionList, SIGNAL(itemSelectionChanged()),
            this, SLOT(updateSelectedWidget()));
    connect(d->m_sectionList, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(itemClicked(QTreeWidgetItem*)));
    connect(d->m_sectionList, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(itemClicked(QTreeWidgetItem*)));

    initTemplates(templatesResourcePath);

    d->m_freeCustomWidgetIndex = 4;

    if (!d->m_sectionList->selectedItems().isEmpty())
    {
        KoSectionListItem* selectedItem = static_cast<KoSectionListItem*>(d->m_sectionList->selectedItems().first());

        if (selectedItem) {
            d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
        }
    }

    QList<int> sizes;

    // Set the sizes of the details pane splitters
    KConfigGroup cfgGrp( KSharedConfig::openConfig(), "TemplateChooserDialog"); sizes = cfgGrp.readEntry("DetailsPaneSplitterSizes", sizes);

    if (!sizes.isEmpty())
        Q_EMIT splitterResized(0, sizes);

    connect(this, SIGNAL(splitterResized(KisDetailsPane*,QList<int>)),
            this, SLOT(saveSplitterSizes(KisDetailsPane*,QList<int>)));

    setAcceptDrops(true);
}

KisOpenPane::~KisOpenPane()
{
    if (!d->m_sectionList->selectedItems().isEmpty()) {
        KoSectionListItem* item = dynamic_cast<KoSectionListItem*>(d->m_sectionList->selectedItems().first());

        if (item) {
            if (!qobject_cast<KisDetailsPane*>(d->m_widgetStack->widget(item->widgetIndex()))) {
                KConfigGroup cfgGrp( KSharedConfig::openConfig(), "TemplateChooserDialog");
                cfgGrp.writeEntry("LastReturnType", item->untranslatedName());
            }
        }
    }

    delete d;
}



void KisOpenPane::openFileDialog()
{

    KoFileDialog dialog(this, KoFileDialog::OpenFiles, "OpenDocument");
    dialog.setCaption(i18n("Open Existing Document"));
    dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    dialog.setMimeTypeFilters(m_mimeFilter);
    Q_FOREACH (const QString &filename, dialog.filenames()) {
        Q_EMIT openExistingFile(QUrl::fromUserInput(filename));
    }
}

void KisOpenPane::slotOpenTemplate(const QUrl &url)
{
    Q_EMIT openTemplate(url);
    accept();
}

void KisOpenPane::initTemplates(const QString& templatesResourcePath)
{
    QTreeWidgetItem* selectItem = 0;
    QTreeWidgetItem* firstItem = 0;
    const int templateOffset = 1000;

    if (!templatesResourcePath.isEmpty()) {
        KisTemplateTree templateTree(templatesResourcePath, true);

        Q_FOREACH (KisTemplateGroup *group, templateTree.groups()) {
            if (group->isHidden()) {
                continue;
            }

            if (!d->m_templatesSeparator) {
                d->m_templatesSeparator = new KoSectionListItem(d->m_sectionList, "", "", 999);
            }

            KisTemplatesPane* pane = new KisTemplatesPane(this, group->name(),
                                                          group, templateTree.defaultTemplate());
            connect(pane, SIGNAL(openUrl(QUrl)), this, SLOT(slotOpenTemplate(QUrl)));
            connect(pane, SIGNAL(alwaysUseChanged(KisTemplatesPane*,QString)),
                    this, SIGNAL(alwaysUseChanged(KisTemplatesPane*,QString)));
            connect(this, SIGNAL(alwaysUseChanged(KisTemplatesPane*,QString)),
                    pane, SLOT(changeAlwaysUseTemplate(KisTemplatesPane*,QString)));
            connect(pane, SIGNAL(splitterResized(KisDetailsPane*,QList<int>)),
                    this, SIGNAL(splitterResized(KisDetailsPane*,QList<int>)));
            connect(this, SIGNAL(splitterResized(KisDetailsPane*,QList<int>)),
                    pane, SLOT(resizeSplitter(KisDetailsPane*,QList<int>)));
            QTreeWidgetItem* item = addPane(group->name(), "Template", group->templates().first()->loadPicture(),
                                            pane, group->sortingWeight() + templateOffset);



            if (!firstItem) {
                firstItem = item;
            }

            if (group == templateTree.defaultGroup()) {
                firstItem = item;
            }

            if (pane->isSelected()) {
                selectItem = item;
            }
        }
    } else {
        firstItem = d->m_sectionList->topLevelItem(0);
    }

    KConfigGroup cfgGrp( KSharedConfig::openConfig(), "TemplateChooserDialog");

    if (selectItem && (cfgGrp.readEntry("LastReturnType") == "Template")) {
        d->m_sectionList->setCurrentItem(selectItem, 0, QItemSelectionModel::ClearAndSelect);
    }
    else if (d->m_sectionList->selectedItems().isEmpty() && firstItem) {
        d->m_sectionList->setCurrentItem(firstItem, 0, QItemSelectionModel::ClearAndSelect);
    }
}

void KisOpenPane::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->accept();
    }
}

void KisOpenPane::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() > 0) {
        // XXX: when the MVC refactoring is done, this can open a bunch of
        //      urls, but since the part/document combination is still 1:1
        //      that won't work for now.
        Q_EMIT openExistingFile(event->mimeData()->urls().first());

    }
}

void KisOpenPane::addCustomDocumentWidget(QWidget *widget, const QString& title, const QString &untranslatedName, const QString& icon)
{
    Q_ASSERT(widget);

    // KRIMBLE 2026-10-09: the Custom Document and Create from Clipboard pages need more room than a phone screen has
    // (about 1415 x 1375 px), so Create/Cancel ended up off-screen. On Android each page sits in a scroll area:
    // everything, Create and Cancel included, is reachable by dragging.
    QWidget *paneWidget = widget;
// Krimble 2026-10-12: George: the scroll area made the Create button harder to find. Switched off; the window is made smaller
// instead (narrow left list with the text under the icons, a label that wraps). The version below is kept, not deleted.
#if 0
    {
        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidget(widget);
        KisKineticScroller::createPreconfiguredScroller(scrollArea);
        paneWidget = scrollArea;
    }
#endif
    // QTreeWidgetItem* item = addPane(title, untranslatedName, icon, widget, d->m_freeCustomWidgetIndex);
    QTreeWidgetItem* item = addPane(title, untranslatedName, icon, paneWidget, d->m_freeCustomWidgetIndex);
    ++d->m_freeCustomWidgetIndex;
    KConfigGroup cfgGrp( KSharedConfig::openConfig(), "TemplateChooserDialog");

    QString lastActiveItem = cfgGrp.readEntry("LastReturnType");
    bool showCustomItemByDefault = cfgGrp.readEntry("ShowCustomDocumentWidgetByDefault", false);
    if (lastActiveItem == untranslatedName || (lastActiveItem.isEmpty() && showCustomItemByDefault)) {
        d->m_sectionList->setCurrentItem(item, 0, QItemSelectionModel::ClearAndSelect);
        KoSectionListItem* selectedItem = static_cast<KoSectionListItem*>(item);
        d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
    }
}

bool KisOpenPane::selectItem(const QString &name)
{
    QTreeWidgetItem* item =  d->m_itemMap->value(name, nullptr);
    if (!item) return false;

    d->m_sectionList->setCurrentItem(item);
    return true;
}


QTreeWidgetItem* KisOpenPane::addPane(const QString &title, const QString &untranslatedName, const QString &iconName, QWidget *widget, int sortWeight)
{
    if (!widget) {
        return 0;
    }

    int id = d->m_widgetStack->addWidget(widget);
    KoSectionListItem* listItem = new KoSectionListItem(d->m_sectionList, title, untranslatedName, sortWeight, id);

    d->m_itemMap->insert(untranslatedName,  (QTreeWidgetItem*)listItem);

    // resizes icons so they are a bit smaller
    QIcon icon = KisIconUtils::loadIcon(iconName);
    // QPixmap iconPixmap = icon.pixmap(32, 32);
    // Krimble 2026-10-13: a 32 x 32 pixel picture was stretched to the size it is shown at (about 108 px on a phone), which made the
    // pictures soft. The picture is now made at the screen's real resolution and told its logical size, so it stays sharp.
    const qreal dpr = qMax<qreal>(1.0, qApp->devicePixelRatio());
    // QPixmap iconPixmap = icon.pixmap(QSize(qRound(32 * dpr), qRound(32 * dpr)));
    // Krimble 2026-10-17: George (1.0.37): "6. New full color icons are still too small. Should be 2x larger. Both in Prefs and New dialog." 64 logical px.
    // QPixmap iconPixmap = icon.pixmap(QSize(qRound(64 * dpr), qRound(64 * dpr)));
    // Krimble 2026-10-18: George (1.0.37): "3. New dialog icons need to be bigger still." 64 -> 96 logical px.
    // QPixmap iconPixmap = icon.pixmap(QSize(qRound(96 * dpr), qRound(96 * dpr)));
    // Krimble 2026-10-18 (2nd): George (1.0.37, fc126d8): "1. I still want bigger icons in File - New dialog. They are small." 96 -> 112 logical px.
    // QPixmap iconPixmap = icon.pixmap(QSize(qRound(112 * dpr), qRound(112 * dpr)));
    // Krimble 2026-10-18 (4th): George (1.0.38): "New dialog top 2 icons are just a little too big now." These two (Custom Document, Create from Clipboard) are the icons of the theme, which fill
    // their whole square; the template pictures below them have room around them and look smaller at the same size. 112 -> 88 logical px for these two (the template pictures stay as they are).
    QPixmap iconPixmap = icon.pixmap(QSize(qRound(88 * dpr), qRound(88 * dpr)));
    iconPixmap.setDevicePixelRatio(dpr);

    QIcon finalIcon(iconPixmap);
    listItem->setIcon(0, finalIcon);

    return listItem;
}

QTreeWidgetItem* KisOpenPane::addPane(const QString &title, const QString &untranslatedName, const QPixmap& icon, QWidget* widget, int sortWeight)
{
    if (!widget) {
        return 0;
    }

    int id = d->m_widgetStack->addWidget(widget);

    // int iconSize = 32;
    // Krimble 2026-10-13 (2nd): George (1.0.35): "In New dialog the first two icons are nice and big, the rest are too small. Why?" The
    // first two (Custom Document, Create from Clipboard) are made at the screen's real resolution (the other addPane above); these section
    // pictures (Animation, Comic, Design, DSLR, Texture) were cut down to 32 x 32 real pixels, about a third of the size. They are made
    // the same way now: 32 logical px times the screen's pixel ratio, and told their logical size.
    const qreal dpr = qMax<qreal>(1.0, qApp->devicePixelRatio());
    // const int iconSize = qRound(32 * dpr);
    // const int iconSize = qRound(64 * dpr);   // Krimble 2026-10-17: twice as big (see above)
    // const int iconSize = qRound(96 * dpr);      // Krimble 2026-10-18: 96 logical px (see above)
    const int iconSize = qRound(112 * dpr);        // Krimble 2026-10-18 (2nd): 112 logical px (see above)

    KoSectionListItem* listItem = new KoSectionListItem(d->m_sectionList, title, untranslatedName, sortWeight, id);

    d->m_itemMap->insert(untranslatedName,  listItem);

    if (!icon.isNull()) {
        QImage image = icon.toImage();

        // if (!image.isNull() && ((image.width() > iconSize) || (image.height() > iconSize))) {
        // Krimble 2026-10-18 (2nd): the pictures are 256 px; they were only ever made smaller, so a bigger size changed nothing. Scaled to the size either way.
        if (!image.isNull() && ((image.width() != iconSize) || (image.height() != iconSize))) {
            image = image.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        image.convertTo(QImage::Format_ARGB32);
        image = image.copy((image.width() - iconSize) / 2, (image.height() - iconSize) / 2, iconSize, iconSize);
        // listItem->setIcon(0, QIcon(QPixmap::fromImage(image)));
        QPixmap sectionPixmap = QPixmap::fromImage(image);
        sectionPixmap.setDevicePixelRatio(dpr);
        listItem->setIcon(0, QIcon(sectionPixmap));
    }

    return listItem;
}

void KisOpenPane::updateSelectedWidget()
{
    if(!d->m_sectionList->selectedItems().isEmpty())
    {
        KoSectionListItem* section = dynamic_cast<KoSectionListItem*>(d->m_sectionList->selectedItems().first());

        if (!section)
            return;

        d->m_widgetStack->setCurrentIndex(section->widgetIndex());
    }
}

void KisOpenPane::saveSplitterSizes(KisDetailsPane* sender, const QList<int>& sizes)
{
    Q_UNUSED(sender);
    KConfigGroup cfgGrp( KSharedConfig::openConfig(), "TemplateChooserDialog");
    cfgGrp.writeEntry("DetailsPaneSplitterSizes", sizes);
}

void KisOpenPane::itemClicked(QTreeWidgetItem* item)
{
    KoSectionListItem* selectedItem = static_cast<KoSectionListItem*>(item);

    if (selectedItem && selectedItem->widgetIndex() >= 0) {
        d->m_widgetStack->widget(selectedItem->widgetIndex())->setFocus();
    }
}
