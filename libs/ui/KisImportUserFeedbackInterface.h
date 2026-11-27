/*
 *  SPDX-FileCopyrightText: 2023 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef KISIMPORTUSERFEEDBACKINTERFACE_H
#define KISIMPORTUSERFEEDBACKINTERFACE_H

#include <QtGlobal>
#include <functional>

class QWidget;

class KisImportUserFeedbackInterface
{
public:
    using AskCallback = std::function<bool(QWidget*)>;

    enum Result {
        Success = 0,
        UserCancelled,
        SuppressedByBatchMode
    };

public:
    KisImportUserFeedbackInterface() = default;

    virtual ~KisImportUserFeedbackInterface();

    virtual Result askUser(AskCallback callback) = 0;

private:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    Q_DISABLE_COPY_MOVE(KisImportUserFeedbackInterface);
#else
    KisImportUserFeedbackInterface(const KisImportUserFeedbackInterface&) = delete;
    KisImportUserFeedbackInterface(KisImportUserFeedbackInterface&&) = delete;
#endif
};

#endif
