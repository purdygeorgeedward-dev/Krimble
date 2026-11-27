/*
 *  SPDX-FileCopyrightText: 2013 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __KIS_ASSERT_H
#define __KIS_ASSERT_H

#include <QtGlobal>
#include <kritaglobal_export.h>

KRITAGLOBAL_EXPORT void kis_assert_exception(const char *assertion, const char *file, int line);
KRITAGLOBAL_EXPORT void kis_assert_recoverable(const char *assertion, const char *file, int line);
KRITAGLOBAL_EXPORT void kis_assert_x_exception(const char *assertion, const char *where, const char *what, const char *file, int line);

KRITAGLOBAL_EXPORT void kis_safe_assert_recoverable(const char *assertion, const char *file, int line);

#define KIS_ASSERT(cond) ((!(cond)) ? kis_assert_exception(#cond,__FILE__,__LINE__) : qt_noop())

#define KIS_ASSERT_X(cond, where, what) ((!(cond)) ? kis_assert_x_exception(#cond,where, what,__FILE__,__LINE__) : qt_noop())

#define KIS_ASSERT_RECOVER(cond) if (!(cond) && (kis_assert_recoverable(#cond,__FILE__,__LINE__), true))

#define KIS_ASSERT_RECOVER_BREAK(cond) KIS_ASSERT_RECOVER(cond) { break; }

#define KIS_ASSERT_RECOVER_RETURN(cond) do { KIS_ASSERT_RECOVER(cond) { return; } } while (0)

#define KIS_ASSERT_RECOVER_RETURN_VALUE(cond, val) do { KIS_ASSERT_RECOVER(cond) { return (val); } } while (0)

#define KIS_ASSERT_RECOVER_NOOP(cond) do { KIS_ASSERT_RECOVER(cond) { qt_noop(); } } while (0)

#define KIS_SAFE_ASSERT_RECOVER(cond) if (!(cond) && (kis_safe_assert_recoverable(#cond,__FILE__,__LINE__), true))
#define KIS_SAFE_ASSERT_RECOVER_BREAK(cond) KIS_SAFE_ASSERT_RECOVER(cond) { break; }
#define KIS_SAFE_ASSERT_RECOVER_RETURN(cond) do { KIS_SAFE_ASSERT_RECOVER(cond) { return; } } while (0)
#define KIS_SAFE_ASSERT_RECOVER_RETURN_VALUE(cond, val) do { KIS_SAFE_ASSERT_RECOVER(cond) { return (val); } } while (0)
#define KIS_SAFE_ASSERT_RECOVER_NOOP(cond) do { KIS_SAFE_ASSERT_RECOVER(cond) { qt_noop(); } } while (0)

#endif
