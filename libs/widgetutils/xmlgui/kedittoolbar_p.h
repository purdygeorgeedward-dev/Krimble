/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 2000 Kurt Granroth <granroth@kde.org>
   SPDX-FileCopyrightText: 2006 Hamish Rodda <rodda@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef KEDITTOOLBARP_H
#define KEDITTOOLBARP_H

#include "kxmlguiclient.h"
#include <QDialog>
#include <QListWidget>

class QDialogButtonBox;
class QLineEdit;
class QCheckBox;

namespace KDEPrivate
{

class ToolBarItem;
class KisKEditToolBarWidgetPrivate;

class ToolBarListWidget : public QListWidget
{
    Q_OBJECT
public:
    ToolBarListWidget(QWidget *parent = 0);

    void makeVisible(QListWidgetItem *item)
    {
        scrollTo(indexFromItem(item));
    }

    ToolBarItem *currentItem() const;

    void setActiveList(bool isActiveList)
    {
        m_activeList = isActiveList;
    }

Q_SIGNALS:
    void dropped(ToolBarListWidget *list, int index, ToolBarItem *item, bool sourceIsActiveList);

protected:
    Qt::DropActions supportedDropActions() const override
    {
        return Qt::MoveAction;
    }
    QStringList mimeTypes() const override
    {
        return QStringList() << QStringLiteral("application/x-kde-action-list");
    }

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QMimeData *mimeData(const QList<QListWidgetItem *> items) const override;
#else
    QMimeData *mimeData(const QList<QListWidgetItem *> &items) const override;
#endif
    bool dropMimeData(int index, const QMimeData *data, Qt::DropAction action) override;

    void dropEvent(QDropEvent *ev) override
    {
        QAbstractItemView::dropEvent(ev);
    }

private:
    bool m_activeList;
};

class IconTextEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit IconTextEditDialog(QWidget *parent = 0);

public:
    void setIconText(const QString &text);
    QString iconText() const;

    void setTextAlongsideIconHidden(bool hidden);
    bool textAlongsideIconHidden() const;

private Q_SLOTS:
    void slotTextChanged(const QString &text);

private:
    QLineEdit *m_lineEdit;
    QCheckBox *m_cbHidden;
    QDialogButtonBox *m_buttonBox;
};

class KisKEditToolBarWidget : public QWidget, virtual public KisKXMLGUIClient
{
    Q_OBJECT
public:
    explicit KisKEditToolBarWidget(KisKActionCollection *collection,
                                QWidget *parent = 0L);

    explicit KisKEditToolBarWidget(QWidget *parent = 0L);

    ~KisKEditToolBarWidget() override;

    void load(const QString &resourceFile,
              bool global = true,
              const QString &defaultToolBar = QString());

    void load(KisKXMLGUIFactory *factory,
              const QString &defaultToolBar = QString());

    KisKActionCollection *actionCollection() const override;

    void save();

    void rebuildKisKXMLGUIClients();

Q_SIGNALS:
    void enableOk(bool);

private:
    Q_PRIVATE_SLOT(d, void slotToolBarSelected(int index))

    Q_PRIVATE_SLOT(d, void slotInactiveSelectionChanged())
    Q_PRIVATE_SLOT(d, void slotActiveSelectionChanged())

    Q_PRIVATE_SLOT(d, void slotInsertButton())
    Q_PRIVATE_SLOT(d, void slotRemoveButton())
    Q_PRIVATE_SLOT(d, void slotUpButton())
    Q_PRIVATE_SLOT(d, void slotDownButton())

    Q_PRIVATE_SLOT(d, void slotDropped(ToolBarListWidget *, int, ToolBarItem *, bool))

    Q_PRIVATE_SLOT(d, void slotChangeIconButton())

private:
    friend class KisKEditToolBarWidgetPrivate;
    KisKEditToolBarWidgetPrivate *const d;

    Q_DISABLE_COPY(KisKEditToolBarWidget)
};

}

#endif
