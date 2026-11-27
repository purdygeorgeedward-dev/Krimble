/*
 *  SPDX-FileCopyrightText: 2020 Saurabh Kumar <saurabhk660@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __STORYBOARD_DELEGATE_H
#define __STORYBOARD_DELEGATE_H

#include <QStyledItemDelegate>
#include <QTextEdit>

#include "StoryboardView.h"
#include <kis_types.h>
#include <kis_debug.h>
#include <kis_image.h>

class QListView;
class StoryboardModel;
class StoryboardView;

class StoryboardDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    StoryboardDelegate(QObject *parent);
    ~StoryboardDelegate() override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setView(StoryboardView *view);

    void drawSpinBox(QPainter *p, const QStyleOptionViewItem &option, QString data, QString suffix) const;

    QStyleOptionSlider drawCommentHeader(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QRect spinBoxUpButton(const QStyleOptionViewItem &option);

    QRect spinBoxDownButton(const QStyleOptionViewItem &option);

    QRect spinBoxEditField(const QStyleOptionViewItem &option);

    QRect scrollBar(const QStyleOptionViewItem &option, QStyleOptionSlider &scrollBarOption) const;

    QRect scrollDownButton(const QStyleOptionViewItem &option, QStyleOptionSlider &scrollBarOption);

    QRect scrollUpButton(const QStyleOptionViewItem &option, QStyleOptionSlider &scrollBarOption);
    void setImageSize(QSize imageSize);

    bool isOverlappingActionIcons(const QRect& rect, const QMouseEvent *event);

protected:
    bool eventFilter(QObject* editor, QEvent* event) override;

private Q_SLOTS:

    void slotCommentScrolledTo(int value) const;

private:
    StoryboardView *m_view;
    QPoint m_lastDragPos = QPoint(0, 0);
    QSize m_imageSize;
};

class LimitedTextEditor : public QTextEdit {
    Q_OBJECT
public:
    LimitedTextEditor(int limit, QWidget* parent = nullptr)
        : QTextEdit(parent)
        , m_charLimit(limit){
        connect(this, SIGNAL(textChanged()), this, SLOT(restrictText()));
    }

    ~LimitedTextEditor(){}

public Q_SLOTS:
    void restrictText() {
        if (toPlainText().length() > m_charLimit) {
            setText(toPlainText().left(m_charLimit));
            QTextCursor c = textCursor();
            c.setPosition(m_charLimit);
            setTextCursor(c);
        }
    }

private:
    const int m_charLimit;
};

#endif
