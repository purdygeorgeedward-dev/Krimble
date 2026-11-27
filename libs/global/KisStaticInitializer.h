/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISSTATICINITIALIZER_H
#define KISSTATICINITIALIZER_H

#ifndef CONCAT
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define CONCAT_IMPL(a, b) a ## b
#endif

#define KIS_DECLARE_STATIC_INITIALIZER_IMPL(uniqueId)  \
    static void CONCAT(initializerFunc, uniqueId)(); \
\
    class CONCAT(InitializerStruct, uniqueId) \
    { \
        public: \
        CONCAT(InitializerStruct, uniqueId)(void (*initializer)()) { \
            initializer(); \
        } \
    }; \
\
    static CONCAT(InitializerStruct, uniqueId) CONCAT(__initializerVariable, uniqueId)(&CONCAT(initializerFunc, uniqueId)); \
\
    void CONCAT(initializerFunc, uniqueId)()

#define KIS_DECLARE_STATIC_INITIALIZER KIS_DECLARE_STATIC_INITIALIZER_IMPL(__COUNTER__)

#endif
