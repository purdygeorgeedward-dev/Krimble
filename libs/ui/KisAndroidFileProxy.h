/*
 * SPDX-FileCopyrightText: 2023 Sharaf Zaman <shzam@sdf.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KISANDROIDFILEPROXY_H_
#define __KISANDROIDFILEPROXY_H_

#include <QString>

class KisAndroidFileProxy {
public:

    static QString getFileFromContentUri(QString contentUri);

};

#endif
