// A stand-in for KDE's i18n(), only so that KrimbleTextDialog.cpp can be built here without KDE.
#pragma once
#include <QString>
inline QString i18n(const char *text) { return QString::fromUtf8(text); }
template<typename... A> QString i18n(const char *text, A... args)
{
    QString s = QString::fromUtf8(text);
    ((s = s.arg(args)), ...);
    return s;
}
