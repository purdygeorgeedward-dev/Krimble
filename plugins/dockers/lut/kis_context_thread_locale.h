// SPDX-FileCopyrightText: 2013-2018 INRIA
// SPDX-License-Identifier: GPL-2.0-or-later

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <windows.h>
#endif
#include <locale.h>
#ifdef __APPLE__
#include <xlocale.h>
#endif

#ifndef KIS_CONTEXT_THREAD_LOCALE_H
#define KIS_CONTEXT_THREAD_LOCALE_H

class AutoSetAndRestoreThreadLocale
{
public:
    AutoSetAndRestoreThreadLocale()
    {
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        previousThreadConfig = _configthreadlocale(_ENABLE_PER_THREAD_LOCALE);

        ssaLocale.assign(setlocale(LC_ALL, NULL));

        setlocale(LC_ALL, "C");
#else
        currentLocale = newlocale(LC_ALL_MASK, "C", NULL);
        oldLocale = uselocale(currentLocale);
#endif
    }

    ~AutoSetAndRestoreThreadLocale()
    {
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
        setlocale(LC_ALL, ssaLocale.c_str());

        _configthreadlocale(previousThreadConfig);
#else
        uselocale(oldLocale);
        freelocale(currentLocale);
#endif
    }

private:
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    std::string ssaLocale;
    int previousThreadConfig;
#else
    locale_t oldLocale;
    locale_t currentLocale;
#endif
};

#endif
