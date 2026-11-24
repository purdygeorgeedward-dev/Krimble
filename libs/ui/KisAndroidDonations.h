/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef __KISANDROIDDONATIONS_H_
#define __KISANDROIDDONATIONS_H_

#include <QObject>
#include <QVector>

#include "KisSupporterProduct.h"
#include "kritaui_export.h"

class KRITAUI_EXPORT KisAndroidDonations : public QObject
{
    Q_OBJECT
    friend class KisApplication;

public:
    enum class State {
        Unknown = 0,
        Checking = 1,
        Unavailable = 2,
        NoSupport = 3,
        Supporter = 4,
    };

    static KisAndroidDonations *instance();

    State state() const { return m_state; }

    bool shouldShowSupporterBadge() const;

    bool isShowDonationManagementDialogPending() const
    {
        return m_showDonationManagementDialogPending;
    }

    void setShowDonationManagementDialogPending(bool pending);

    QVector<KisSupporterProduct> getCurrentProducts() const;

    void startBillingFlowFor(const QString &productId, const QString &offerToken);

    static void setLoaded(bool loaded);
    static void setLoadingText(const QString &text);
    static void showDonationDialog(bool splash);

public Q_SLOTS:
    void slotStartDonationFlow();
    void slotManageSubscriptions();
    void slotManageSubscription(const QString &productId);

private Q_SLOTS:
    void slotUpdateState(int state, long long ownedProductFlags);

Q_SIGNALS:
    void sigStateUpdateReceived(int state, long long ownedProductFlags);
    void sigStateChanged();
    void sigShowDonationManagementDialogRequested();
    void sigSplashDialogDismissed();

private:
    explicit KisAndroidDonations(QObject *parent = nullptr);

    bool isProductOwned(const QString &productId) const;

    void syncState();

    State m_state{State::Unknown};
    long long m_ownedProductFlags{0L};
    bool m_showDonationManagementDialogPending{false};
};

#endif
