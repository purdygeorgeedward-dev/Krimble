/*
 * SPDX-FileCopyrightText: 2018 Boudewijn Rempt <boud@valdyas.org>
 * SPDX-FileCopyrightText: 2020 Agata Cacko <cacko.azh@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TAGACTIONS_H
#define TAGACTIONS_H

#include <QWidgetAction>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <KoResource.h>

#include <KisTag.h>
#include <KisTagModel.h>

#include <kritaresourcewidgets_export.h>

class SimpleExistingTagAction : public QAction
{
    Q_OBJECT
public:
    explicit SimpleExistingTagAction(KoResourceSP resource, KisTagSP tag, QObject* parent = 0);
    ~SimpleExistingTagAction() override;

Q_SIGNALS:
    void triggered(KisTagSP tag, KoResourceSP resource);

protected Q_SLOTS:
    void onTriggered();

private:
    KoResourceSP m_resource;
    KisTagSP m_tag;
};

class KRITARESOURCEWIDGETS_EXPORT LineEditAction : public QWidgetAction
{
    Q_OBJECT
protected:
    LineEditAction(QObject* parent);

public:
    ~LineEditAction() override;
    void setIcon(const QIcon &icon);
    void setCloseParentOnTrigger(bool closeParent);
    bool closeParentOnTrigger();

    void setPlaceholderText(const QString& clickMessage);
    void setText(const QString& text);
    void setVisible(bool showAction);

protected Q_SLOTS:

    void slotActionTriggered();

    virtual void onTriggered() {}

    QString userText();

private:
    bool m_closeParentOnTrigger;
    QLabel *m_label;
    QLineEdit *m_editBox;
    QPushButton *m_AddButton;
};

class KRITARESOURCEWIDGETS_EXPORT UserInputTagAction : public LineEditAction
{
    Q_OBJECT

public:
    explicit UserInputTagAction(QObject* parent);
    ~UserInputTagAction() override;

Q_SIGNALS:
    void triggered(const QString &newTagName);

protected Q_SLOTS:
    void onTriggered() override;
};

class NewTagResourceAction : public LineEditAction
{
    Q_OBJECT

public:
    explicit NewTagResourceAction(KoResourceSP resource, QObject* parent);
    ~NewTagResourceAction() override;

    void setResource(KoResourceSP resource);

Q_SIGNALS:
    void triggered(const QString &newTagName, KoResourceSP resource);

protected Q_SLOTS:

    void onTriggered() override;

private:
    KoResourceSP m_resource;
};

class CompareWithOtherTagFunctor
{
    KisTagSP m_referenceTag;

public:
    CompareWithOtherTagFunctor(KisTagSP referenceTag);

    bool operator()(KisTagSP otherTag);

    void setReferenceTag(KisTagSP referenceTag);

    KisTagSP referenceTag();

};

#endif
