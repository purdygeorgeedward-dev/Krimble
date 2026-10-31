// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-10-30: the two vector tools are one toolbox button with a toggle. George (2026-10-30): "I think you should have left it. Edit shapes can be a toggle
// or something." The toolbox button, with George's arrow picture, is the **Select Shapes** tool (the black arrow: moves, scales and rotates a whole vector
// object); the **Edit Shapes** tool (the white arrow: the points and handles of an object) has no button of its own. Instead the Tool Options of each tool start
// with this row of two buttons, **Whole object** and **Edit points**; pressing the other one switches to the other tool (one tap).
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
        // queued, so this button is not deleted inside its own click
        QTimer::singleShot(0, [otherToolId]() { KoToolManager::instance()->switchToolRequested(otherToolId); });
    });
    return row;
}
