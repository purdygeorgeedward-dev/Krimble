/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGCSSHELPER_H
#define SVGCSSHELPER_H

#include <QStringList>

#include <QDomDocument>

class SvgCssHelper
{
public:
    SvgCssHelper();
    ~SvgCssHelper();

    void parseStylesheet(const QDomElement &);

    QStringList matchStyles(const QDomElement &element) const;

private:
    class Private;
    Private * const d;
};

#endif
