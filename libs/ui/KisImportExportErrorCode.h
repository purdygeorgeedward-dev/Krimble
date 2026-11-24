/*
 *  SPDX-FileCopyrightText: 2019 Agata Cacko <cacko.azh@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KIS_IMPORT_EXPORT_ERROR_CODES_H
#define KIS_IMPORT_EXPORT_ERROR_CODES_H

#include <QFile>
#include <QFileDevice>
#include <QString>
#include <kritaui_export.h>
#include <QDebug>

namespace ImportExportCodes
{

    enum ErrorCodeID
    {
        InternalError,

        FileNotExist,
        NoAccessToRead,
        ErrorWhileReading,
        FileFormatIncorrect,
        FormatFeaturesUnsupported,
        FormatColorSpaceUnsupported,

        CannotCreateFile,
        NoAccessToWrite,
        ErrorWhileWriting,
        InsufficientMemory,
        FileFormatNotSupported,

        Cancelled,

        Failure,

        Busy,

        OK,

    };

};

class KisImportExportErrorCode;

struct KRITAUI_EXPORT KisImportExportComplexError
{
    virtual QString errorMessage() const = 0;
    KisImportExportComplexError(QFileDevice::FileError error);

    friend inline QDebug &operator<<(QDebug &d,
                                     const KisImportExportErrorCode &errorCode);
    virtual ~KisImportExportComplexError() = default;

protected:
    QString qtErrorMessage() const;
    QFileDevice::FileError m_error;
};

struct KRITAUI_EXPORT KisImportExportErrorCannotWrite : KisImportExportComplexError
{

    KisImportExportErrorCannotWrite(QFileDevice::FileError error);
    QString errorMessage() const override;

    ~KisImportExportErrorCannotWrite() override = default;

    bool operator==(KisImportExportErrorCannotWrite other);

private:
    KisImportExportErrorCannotWrite();

    friend class KisImportExportErrorCode;

};

struct KRITAUI_EXPORT KisImportExportErrorCannotRead : KisImportExportComplexError
{

    KisImportExportErrorCannotRead(QFileDevice::FileError error);
    QString errorMessage() const override;

    ~KisImportExportErrorCannotRead() override = default;

    bool operator==(KisImportExportErrorCannotRead other);

private:
    KisImportExportErrorCannotRead();

    friend class KisImportExportErrorCode;

};

class KRITAUI_EXPORT KisImportExportErrorCode
{
public:
    KisImportExportErrorCode();

    KisImportExportErrorCode(ImportExportCodes::ErrorCodeID code);
    KisImportExportErrorCode(KisImportExportErrorCannotRead code);
    KisImportExportErrorCode(KisImportExportErrorCannotWrite code);
    explicit KisImportExportErrorCode(const QString &message);

    QString errorMessage() const;
    bool isOk() const;
    bool isCancelled() const;
    bool isInternalError() const;

    friend inline QDebug &operator<<(QDebug &d,
                                     const KisImportExportErrorCode &errorCode);

    bool operator==(KisImportExportErrorCode errorCode);

private:
    enum ErrorFieldUsed
    {
        None,
        CodeId,
        CannotRead,
        CannotWrite
    };

    ErrorFieldUsed errorFieldUsed;

    ImportExportCodes::ErrorCodeID codeId {ImportExportCodes::OK};
    KisImportExportErrorCannotRead cannotRead;
    KisImportExportErrorCannotWrite cannotWrite;
    QString failureMessage;
};

inline QDebug &operator<<(QDebug &d, const KisImportExportErrorCode &errorCode)
{
    switch (errorCode.errorFieldUsed) {
    case KisImportExportErrorCode::None:
        d << "None of the error fields is in use.";
        break;
    case KisImportExportErrorCode::CannotRead:
        d << "Cannot read: " << errorCode.cannotRead.m_error;
        break;
    case KisImportExportErrorCode::CannotWrite:
        d << "Cannot write: " << errorCode.cannotRead.m_error;
        break;
    case KisImportExportErrorCode::CodeId:
        d << "Error code = " << errorCode.codeId;
    }
    d << " " << errorCode.errorMessage();
    return d;
}

#endif
