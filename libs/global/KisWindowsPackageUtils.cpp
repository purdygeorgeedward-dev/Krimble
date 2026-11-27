/*
 * SPDX-FileCopyrightText: 2022 Alvin Wong <alvin@alvinhc.com>
 * SPDX-FileCopyrightText: 2022 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KisWindowsPackageUtils.h"

#include <array>

#include <windows.h>

#if defined __has_include
#if __has_include(<appmodel.h>)
#include <appmodel.h>
#define HAS_APPMODEL_H
#endif
#endif

#if defined HAS_APPMODEL_H
using pGetCurrentPackageFamilyName_t = decltype(&GetCurrentPackageFamilyName);

using pGetCurrentPackageFullName_t = decltype(&GetCurrentPackageFullName);
#else
using pGetCurrentPackageFamilyName_t = LONG(WINAPI *)(UINT32 *packageFamilyNameLength, PWSTR packageFamilyName);

using pGetCurrentPackageFullName_t = LONG(WINAPI *)(UINT32 *packageFullNameLength, PWSTR packageFullName);
#endif

#include <shlobj.h>

#include <QDebug>
#include <QLibrary>
#include <QString>

#ifndef PACKAGE_FULL_NAME_MAX_LENGTH
constexpr int PACKAGE_FULL_NAME_MAX_LENGTH = 127;
#endif

#ifndef APPMODEL_ERROR_NO_PACKAGE
constexpr LONG APPMODEL_ERROR_NO_PACKAGE = 15700;
#endif

#ifndef KF_FLAG_RETURN_FILTER_REDIRECTION_TARGET
#define KF_FLAG_RETURN_FILTER_REDIRECTION_TARGET 0x00040000
#endif

struct AppmodelFunctions {
    pGetCurrentPackageFamilyName_t getCurrentPackageFamilyName{};
    pGetCurrentPackageFullName_t getCurrentPackageFullName{};
    QLibrary dllKernel32;

    template<typename T, typename U>
    inline T cast_to_function(U v) noexcept
    {
        return reinterpret_cast<T>(reinterpret_cast<void *>(v));
    }

    static const AppmodelFunctions &instance()
    {
        static const AppmodelFunctions s{};
        return s;
    }

    AppmodelFunctions()
        : dllKernel32("kernel32.dll")
    {
        getCurrentPackageFamilyName =
            cast_to_function<pGetCurrentPackageFamilyName_t>(dllKernel32.resolve("GetCurrentPackageFamilyName"));
        getCurrentPackageFullName =
            cast_to_function<pGetCurrentPackageFullName_t>(dllKernel32.resolve("GetCurrentPackageFullName"));
    }

    ~AppmodelFunctions() = default;
};

namespace KisWindowsPackageUtils
{

bool isRunningInPackage()
{
    return tryGetCurrentPackageFamilyName(nullptr);
}

bool tryGetCurrentPackageFamilyName(QString *outName)
{
    if (!AppmodelFunctions::instance().getCurrentPackageFamilyName) {
        return false;
    }

    std::array<WCHAR, PACKAGE_FULL_NAME_MAX_LENGTH + 1> name{};
    UINT32 nameLength = name.size();
    const LONG result = AppmodelFunctions::instance().getCurrentPackageFamilyName(&nameLength, name.data());
    if (result == APPMODEL_ERROR_NO_PACKAGE) {
        return false;
    }
    if (result == ERROR_INSUFFICIENT_BUFFER) {
        qWarning() << "GetCurrentPackageFamilyName returned "
                      "ERROR_INSUFFICIENT_BUFFER, required length is"
                   << nameLength;
        if (outName) {
            *outName = QString();
        }
        return true;
    }
    if (result != ERROR_SUCCESS) {
        qWarning() << "GetCurrentPackageFamilyName returned unexpected error code:" << result;
        return false;
    }

    if (outName) {
        if (nameLength > name.size()) {
            qWarning() << "GetCurrentPackageFamilyName returned a length "
                          "exceeding the buffer size:"
                       << nameLength;
            nameLength = name.size();
        }
        if (nameLength > 0 && name.at(nameLength - 1) == L'\0') {
            nameLength -= 1;
        }
        *outName = QString::fromWCharArray(name.data(), static_cast<int>(nameLength));
    }
    return true;
}

bool tryGetCurrentPackageFullName(QString *outName)
{
    if (!AppmodelFunctions::instance().getCurrentPackageFullName) {
        return false;
    }

    std::array<WCHAR, PACKAGE_FULL_NAME_MAX_LENGTH + 1> name{};
    UINT32 nameLength = name.size();
    const LONG result = AppmodelFunctions::instance().getCurrentPackageFullName(&nameLength, name.data());
    if (result == APPMODEL_ERROR_NO_PACKAGE) {
        return false;
    }
    if (result == ERROR_INSUFFICIENT_BUFFER) {
        qWarning() << "GetCurrentPackageFullName returned "
                      "ERROR_INSUFFICIENT_BUFFER, required length is"
                   << nameLength;
        if (outName) {
            *outName = QString();
        }
        return true;
    }
    if (result != ERROR_SUCCESS) {
        qWarning() << "GetCurrentPackageFullName returned unexpected error code:" << result;
        return false;
    }

    if (outName) {
        if (nameLength > name.size()) {
            qWarning() << "GetCurrentPackageFullName returned a length "
                          "exceeding the buffer size:"
                       << nameLength;
            nameLength = name.size();
        }
        if (nameLength > 0 && name.at(nameLength - 1) == L'\0') {
            nameLength -= 1;
        }
        *outName = QString::fromWCharArray(name.data(), static_cast<int>(nameLength));
    }
    return true;
}

QString getPackageRoamingAppDataLocation()
{
    PWSTR path = nullptr;
    HRESULT result =
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_RETURN_FILTER_REDIRECTION_TARGET, nullptr, &path);
    if (result != S_OK) {
        qWarning() << "SHGetKnownFolderPath returned error HRESULT:" << result;
        return {};
    }
    if (!path) {
        qWarning() << "SHGetKnownFolderPath did not return a path";
        return {};
    }
    QString appData = QString::fromWCharArray(path);
    CoTaskMemFree(path);
    return appData;
}

}
