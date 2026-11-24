// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <QHBoxLayout>
#include <QTimer>
#include <QToolButton>
#include <QWidget>
#include <KLocalizedString>
#include <KoToolManager.h>

inline QWidget *krimbleShapeModeRow(bool editPoints, QWidget *parent = nullptr)
{
    QWidget *row = new QWidget(parent);
    row->setWindowTitle(i18n("Mode"));
    QHBoxLayout *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    QToolButton *whole = new QToolButton(row);
    QToolButton *points = new QToolButton(row);
    whole->setText(i18n("Whole object"));
    points->setText(i18n("Edit points"));
    for (QToolButton *button : {whole, points}) {
        button->setToolButtonStyle(Qt::ToolButtonTextOnly);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setMinimumHeight(38);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        layout->addWidget(button);
    }
    whole->setChecked(!editPoints);
    points->setChecked(editPoints);
    QToolButton *other = editPoints ? whole : points;
    const QString otherToolId = editPoints ? QStringLiteral("InteractionTool") : QStringLiteral("PathTool");
    QObject::connect(other, &QToolButton::clicked, row, [otherToolId]() {
        QTimer::singleShot(0, [otherToolId]() { KoToolManager::instance()->switchToolRequested(otherToolId); });
    });
    return row;
}
