// SPDX-License-Identifier: GPL-3.0-or-later
package org.krimble.android;

import android.app.Activity;
import android.util.Log;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.ProductDetails;

import org.krimble.R;

import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Stream;

public final class DonationOffer {
    private static final String TAG = "krita.DonationOffer";
    private static final Pattern PRIORITY_TAG_PATTERN = Pattern.compile("\\Ap(-?[0-9]+)\\z");

    private final ProductDetails mProductDetails;
    private final String mOfferToken;
    private final String mButtonText;

    private DonationOffer(ProductDetails productDetails, String offerToken, String buttonText) {
        this.mProductDetails = productDetails;
        this.mOfferToken = offerToken;
        this.mButtonText = buttonText;
    }

    @Override
    public String toString() {
        return "DonationOffer{" +
                "mProductDetails=" + mProductDetails +
                ", mOfferToken='" + mOfferToken + '\'' +
                ", mButtonText='" + mButtonText + '\'' +
                '}';
    }

    public ProductDetails getProductDetails() {
        return mProductDetails;
    }

    public String getOfferToken() {
        return mOfferToken;
    }

    public String getButtonText() {
        return mButtonText;
    }

    public static DonationOffer fromProductDetails(Activity activity, ProductDetails productDetails) {
        if (productDetails == null) {
            return null;
        }

        if (BillingClient.ProductType.SUBS.equals(productDetails.getProductType())) {
            ProductDetails.SubscriptionOfferDetails offerDetails = Optional.ofNullable(productDetails.getSubscriptionOfferDetails())
                    .map(Collection::stream)
                    .orElseGet(Stream::empty)
                    .min(Comparator.comparing(DonationOffer::getSubscriptionOfferPriority))
                    .orElse(null);
            if (offerDetails == null) {
                Log.d(TAG, "No available offers for subscription product " + productDetails);
                return null;
            } else {
                Log.d(TAG, "Using subscription offer " + offerDetails);
                return new DonationOffer(
                        productDetails, offerDetails.getOfferToken(),
                        formatSubscriptionButtonText(activity, offerDetails));
            }
        } else {
            ProductDetails.OneTimePurchaseOfferDetails offerDetails = Optional.ofNullable(productDetails.getOneTimePurchaseOfferDetailsList())
                    .map(Collection::stream)
                    .orElseGet(Stream::empty)
                    .filter(Objects::nonNull)
                    .min(Comparator.comparing(DonationOffer::getOneTimePurchaseOfferPriority))
                    .orElse(null);
            if (offerDetails == null) {
                Log.d(TAG, "No available offers for one-time product " + productDetails);
                return null;
            } else {
                Log.d(TAG, "Using subscription offer " + offerDetails);
                return new DonationOffer(
                        productDetails, offerDetails.getOfferToken(),
                        formatOneTimeButtonText(activity, offerDetails));
            }
        }
    }

    private static int getOneTimePurchaseOfferPriority(ProductDetails.OneTimePurchaseOfferDetails details) {
        return getOfferPriority(details.getOfferTags());
    }

    private static int getSubscriptionOfferPriority(ProductDetails.SubscriptionOfferDetails details) {
        return getOfferPriority(details.getOfferTags());
    }

    private static int getOfferPriority(List<String> offerTags) {
        return Optional.of(offerTags)
                .orElseGet(Collections::emptyList)
                .stream()
                .map(PRIORITY_TAG_PATTERN::matcher)
                .filter(Matcher::find)
                .map(DonationOffer::parsePriorityTagMatch)
                .filter(Objects::nonNull)
                .sorted()
                .findFirst()
                .orElse(1000);
    }

    private static String formatOneTimeButtonText(
            Activity activity, ProductDetails.OneTimePurchaseOfferDetails offerDetails) {
        String price = offerDetails.getFormattedPrice().trim();
        if(!price.isEmpty()) {
            return activity.getString(R.string.product_button_buy_for, price);
        }
        return activity.getString(R.string.product_button_buy);
    }

    private static String formatSubscriptionButtonText(
            Activity activity, ProductDetails.SubscriptionOfferDetails offerDetails) {
        List<ProductDetails.PricingPhase> pricingPhases = offerDetails.getPricingPhases().getPricingPhaseList();
        if (pricingPhases.size() == 1) {
            ProductDetails.PricingPhase onlyPhase = pricingPhases.get(0);
            if ("P1M".equals(onlyPhase.getBillingPeriod())) {
                String price = onlyPhase.getFormattedPrice().trim();
                if(!price.isEmpty()) {
                    return activity.getString(R.string.product_button_subscribe_for_month, price);
                }
            }
        }
        return activity.getString(R.string.product_button_subscribe);
    }

    private static Integer parsePriorityTagMatch(Matcher matcher) {
        String capture = matcher.group(1);
        if (capture != null) {
            try {
                return Integer.parseInt(capture, 10);
            } catch (NumberFormatException e) {
                Log.w(TAG, "Failed to parse tag priority", e);
            }
        }
        return null;
    }
}
