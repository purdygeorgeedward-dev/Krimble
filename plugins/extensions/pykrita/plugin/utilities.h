// SPDX-FileCopyrightText: 2006 Paul Giannaros <paul@giannaros.org>
// SPDX-FileCopyrightText: 2012, 2013 Shaheed Haque <srhaque@theiet.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#ifndef __PYKRITA_UTILITIES_H__
# define  __PYKRITA_UTILITIES_H__

#include <cmath>
#include <Python.h>
#include <QString>

class KConfigBase;

# define PQ(x) x.toUtf8().constData()

class PythonPluginManager;

namespace PyKrita
{
    enum InitResult {
        INIT_UNINITIALIZED,
        INIT_OK,
        INIT_CANNOT_LOAD_PYTHON_LIBRARY,
        INIT_CANNOT_SET_PYTHON_PATHS,
        INIT_CANNOT_LOAD_PYKRITA_MODULE,
    };

    InitResult initialize();

    PythonPluginManager *pluginManager();

    void finalize();

class Python
{
public:
    Python();
    ~Python();

    static bool libraryLoad();

    static bool setPath(const QStringList& scriptPaths);

    static void ensureInitialized();

    static void maybeFinalize();

    static void libraryUnload();

    static PyObject* unicode(const QString& string);

    static QString unicode(PyObject* string);

    static bool isUnicode(PyObject* string);

    bool prependStringToList(PyObject* list, const QString& value);

    void traceback(const QString& description);

    QString lastTraceback(void) const;

    bool functionCall(const char* functionName, const char* moduleName = PYKRITA_ENGINE);
    PyObject* functionCall(const char* functionName, const char* moduleName, PyObject* arguments);

    bool itemStringDel(const char* item, const char* moduleName = PYKRITA_ENGINE);

    PyObject* itemString(const char* item, const char* moduleName = PYKRITA_ENGINE);

    PyObject* itemString(const char* item, PyObject* dict);

    bool itemStringSet(const char* item, PyObject* value, const char* moduleName = PYKRITA_ENGINE);

    PyObject* moduleActions(const char* moduleName);

    PyObject* moduleConfigPages(const char* moduleName);

    PyObject* moduleDict(const char* moduleName = PYKRITA_ENGINE);

    QString moduleHelp(const char* moduleName);

    PyObject* moduleImport(const char* moduleName);

    bool prependPythonPaths(const QString& path);

    bool prependPythonPaths(const QStringList& paths);

    static const char* PYKRITA_ENGINE;

private:
    bool prependPythonPaths(const QString&, PyObject*);
    PyGILState_STATE m_state;
    mutable QString m_traceback;

    PyObject* kritaHandler(const char* moduleName, const char* handler);
};

}
#endif
