/*
 *  SPDX-FileCopyrightText: 2022 Deif Lou <ginoba@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KISOPTIONCOLLECTIONWIDGET_H
#define KISOPTIONCOLLECTIONWIDGET_H

#include <QWidget>
#include <QScopedPointer>

#include <kritawidgetutils_export.h>

class KRITAWIDGETUTILS_EXPORT KisOptionCollectionWidget : public QWidget
{
    Q_OBJECT

public:
    KisOptionCollectionWidget(QWidget *parent = nullptr);
    ~KisOptionCollectionWidget() override;

    int widgetIndexFromId(const QString &id) const;
    bool containsWidget(const QString &id) const;

    QWidget* widget(int index) const;
    template <typename T>
    T widgetAs(int index) const
    {
        return qobject_cast<T>(widget(index));
    }

    QWidget* widget(const QString &id) const;
    template <typename T>
    T widgetAs(const QString &id) const
    {
        return qobject_cast<T>(widget(id));
    }

    QWidget* findWidget(const QString &path) const;
    template <typename T>
    T findWidgetAs(const QString &path) const
    {
        return qobject_cast<T>(findWidget(path));
    }

    void insertWidget(int index, const QString &id, QWidget *widget);
    void appendWidget(const QString &id, QWidget *widget);

    void removeWidget(int index);
    void removeWidget(const QString &id);

    QWidget* takeWidget(int index);
    QWidget* takeWidget(const QString &id);

    void setWidgetVisible(int index, bool visible);
    void setWidgetVisible(const QString &id, bool visible);

    void setWidgetsMargin(int margin);
    void setSeparatorsVisible(bool visible);
    void setOrientation(Qt::Orientation orientation, bool recursive = false);

    int size() const;
    int numberOfVisibleWidgets() const;

private:
    struct Private;
    QScopedPointer<Private> m_d;
};

class KRITAWIDGETUTILS_EXPORT KisOptionCollectionWidgetWithHeader : public QWidget
{
    Q_OBJECT

public:
    KisOptionCollectionWidgetWithHeader(const QString &title, QWidget *parent = nullptr);
    ~KisOptionCollectionWidgetWithHeader() override;

    QSize minimumSizeHint() const override;
    QWidget* primaryWidget() const;
    template <typename T>
    T primaryWidgetAs() const
    {
        return qobject_cast<T>(primaryWidget());
    }
    void setPrimaryWidget(QWidget *widget);
    void removePrimaryWidget();
    QWidget* takePrimaryWidget();
    void setPrimaryWidgetVisible(bool visible);

    int widgetIndexFromId(const QString &id) const;
    bool containsWidget(const QString &id) const;

    QWidget* widget(int index) const;
    template <typename T>
    T widgetAs(int index) const
    {
        return qobject_cast<T>(widget(index));
    }

    QWidget* widget(const QString &id) const;
    template <typename T>
    T widgetAs(const QString &id) const
    {
        return qobject_cast<T>(widget(id));
    }

    QWidget* findWidget(const QString &path) const;
    template <typename T>
    T findWidgetAs(const QString &path) const
    {
        return qobject_cast<T>(findWidget(path));
    }

    void insertWidget(int index, const QString &id, QWidget *widget);
    void appendWidget(const QString &id, QWidget *widget);

    void removeWidget(int index);
    void removeWidget(const QString &id);

    QWidget* takeWidget(int index);
    QWidget* takeWidget(const QString &id);

    void setWidgetVisible(int index, bool visible);
    void setWidgetVisible(const QString &id, bool visible);

    void setWidgetsMargin(int margin);
    void setSeparatorsVisible(bool visible);
    void setOrientation(Qt::Orientation orientation, bool recursive = false);

    int size() const;
    int numberOfVisibleWidgets() const;

protected:
    void resizeEvent(QResizeEvent*) override;

private:
    struct Private;
    QScopedPointer<Private> m_d;
    friend class KisOptionCollectionWidget;
};

#endif
