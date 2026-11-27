/*
 *  SPDX-FileCopyrightText: 2024 Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KOSVGTEXTLOADER_H
#define KOSVGTEXTLOADER_H

#include "KoSvgTextShape.h"
#include "kritaflake_export.h"

class KoSvgTextLoader {
public:
    KoSvgTextLoader(KoSvgTextShape *shape);
    ~KoSvgTextLoader();

    void enterNodeSubtree();

    void leaveNodeSubtree();

    void nextNode();

    bool loadSvg(const QDomElement &element, SvgLoadingContext &context, bool root = false);
    bool loadSvgText(const QDomText &text, SvgLoadingContext &context);

    void setStyleInfo(KoShape* s);
    void setTextPathOnCurrentNode(KoShape *s);
private:
    struct Private;
    QScopedPointer<Private> d;
};

#endif
