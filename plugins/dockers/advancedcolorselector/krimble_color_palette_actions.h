// SPDX-License-Identifier: GPL-2.0-or-later
// Krimble 2026-11-08: the palette commands of the Color panel (New, Open, Import, Export, Duplicate, Rename, Delete), shared by the Palette page and the palette row of the Picker,
// Sliders and Values pages. Each asks what it needs (a name, a file, a confirmation) and returns true when the palettes changed. They work on the GPL files of krimble_color_palettes.h.
#pragma once
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include "krimble_color_palettes.h"

struct KrimblePaletteActions
{
    static QString ask(QWidget *parent, const QString &title, const QString &label, const QString &value)
    {
        bool ok = false;
        const QString text = QInputDialog::getText(parent, title, label, QLineEdit::Normal, value, &ok).trimmed();
        return ok ? text : QString();
    }
    static bool newPalette(QWidget *parent)
    {
        const QString name = ask(parent, QObject::tr("New Palette"), QObject::tr("Palette name:"), QObject::tr("New palette"));
        if (name.isEmpty()) return false;
        KrimblePalette pal;
        pal.name = KrimblePaletteStore::uniqueName(name);
        pal.columns = 16;
        if (!KrimblePaletteStore::save(pal)) return false;
        KrimblePaletteStore::setActive(pal.name);
        return true;
    }
    static bool openPalette(QWidget *parent)
    {
        const QStringList names = KrimblePaletteStore::names();
        if (names.isEmpty()) return false;
        bool ok = false;
        const QString chosen = QInputDialog::getItem(parent, QObject::tr("Open Palette"), QObject::tr("Palette:"), names, qMax(0, names.indexOf(KrimblePaletteStore::active())), false, &ok);
        if (!ok || chosen.isEmpty()) return false;
        KrimblePaletteStore::setActive(chosen);
        return true;
    }
    static bool importPalette(QWidget *parent)
    {
        const QString path = QFileDialog::getOpenFileName(parent, QObject::tr("Import Palette"), QString(), QObject::tr("GIMP palettes (*.gpl);;All files (*)"));
        if (path.isEmpty()) return false;
        QString name;
        if (!KrimblePaletteStore::importFile(path, &name)) {
            QMessageBox::warning(parent, QObject::tr("Import Palette"), QObject::tr("That file is not a palette (a GIMP palette, .gpl, with colors)."));
            return false;
        }
        KrimblePaletteStore::setActive(name);
        return true;
    }
    static bool exportPalette(QWidget *parent)
    {
        const QString name = KrimblePaletteStore::active();
        const QString path = QFileDialog::getSaveFileName(parent, QObject::tr("Export Palette"), name + QStringLiteral(".gpl"), QObject::tr("GIMP palettes (*.gpl)"));
        return !path.isEmpty() && KrimblePaletteStore::exportTo(name, path) && false;   // exporting changes no palette
    }
    static bool duplicatePalette(QWidget *parent)
    {
        const QString from = KrimblePaletteStore::active();
        const QString name = ask(parent, QObject::tr("Duplicate Palette"), QObject::tr("Name of the copy:"), KrimblePaletteStore::uniqueName(from + QStringLiteral(" copy")));
        if (name.isEmpty()) return false;
        KrimblePalette pal;
        if (!KrimblePaletteStore::load(from, pal)) return false;
        pal.name = KrimblePaletteStore::uniqueName(name);
        if (!KrimblePaletteStore::save(pal)) return false;
        KrimblePaletteStore::setActive(pal.name);
        return true;
    }
    static bool renamePalette(QWidget *parent)
    {
        const QString from = KrimblePaletteStore::active();
        const QString name = ask(parent, QObject::tr("Rename Palette"), QObject::tr("New name:"), from);
        return !name.isEmpty() && name != from && KrimblePaletteStore::rename(from, name);
    }
    static bool deletePalette(QWidget *parent)
    {
        const QString name = KrimblePaletteStore::active();
        if (KrimblePaletteStore::names().size() <= 1) {
            QMessageBox::information(parent, QObject::tr("Delete Palette"), QObject::tr("This is the only palette. Make another one first."));
            return false;
        }
        if (QMessageBox::question(parent, QObject::tr("Delete Palette"), QObject::tr("Delete the palette \"%1\" and its file? This cannot be undone.").arg(name)) != QMessageBox::Yes) return false;
        if (!KrimblePaletteStore::remove(name)) return false;
        const QStringList left = KrimblePaletteStore::names();
        if (!left.isEmpty()) KrimblePaletteStore::setActive(left.first());
        return true;
    }
};
