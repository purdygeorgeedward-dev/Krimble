/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

package org.krimble.android;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.content.pm.ServiceInfo;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import org.krimble.R;

// Keeps Krimble running while it is open. Without it, Android counts Krimble as an idle app as soon as another app is
// in front, and its low-memory killer ends it first (the "Last exit: LOW_MEMORY, CACHED" lines in Help > Show Krimble
// Log...). A foreground service makes Android count Krimble as in use, so it is not ended to free memory.
public class KeepAliveService extends Service {

    private static final String TAG = "KeepAliveService";
    private static final String CHANNEL_ID = "org.krimble.android.open";
    private static final int NOTIFICATION_ID = 7;

    // what happened when it was last asked to start (shown in Help > Show Krimble Log...)
    static volatile String status = "never asked to start in this run";

    @Override
    public void onCreate() {
        super.onCreate();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            // NotificationChannel channel = new NotificationChannel(CHANNEL_ID, "Krimble is open", NotificationManager.IMPORTANCE_MIN);
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, "Don'tCrash", NotificationManager.IMPORTANCE_MIN);
            channel.setShowBadge(false);
            getSystemService(NotificationManager.class).createNotificationChannel(channel);
        }
    }

    private Notification notification() {
        Intent open = new Intent(this, MainActivity.class);
        open.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
        PendingIntent openPending = PendingIntent.getActivity(this, 0, open, PendingIntent.FLAG_IMMUTABLE);
        Notification.Builder builder = Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
                ? new Notification.Builder(this, CHANNEL_ID)
                : new Notification.Builder(this);
        // return builder.setContentTitle("Krimble is open")
        return builder.setContentTitle("Don'tCrash")
                .setContentText("Keeps Krimble open in the background")
                .setSmallIcon(R.drawable.ic_save_notification)
                .setContentIntent(openPending)
                .setOngoing(true)
                .build();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
                startForeground(NOTIFICATION_ID, notification(), ServiceInfo.FOREGROUND_SERVICE_TYPE_SPECIAL_USE);
            } else {
                startForeground(NOTIFICATION_ID, notification());
            }
            Log.i(TAG, "Krimble kept in the foreground");
            status = "running since " + new java.text.SimpleDateFormat("HH:mm:ss", java.util.Locale.US).format(new java.util.Date());
        } catch (Exception e) {
            Log.w(TAG, "Could not keep Krimble in the foreground: " + e);
            status = "FAILED to start: " + e;
            stopSelf();
        }
        return START_NOT_STICKY;
    }

    @Override
    public void onTaskRemoved(Intent rootIntent) {
        stopForeground(true);
        stopSelf();
        super.onTaskRemoved(rootIntent);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
