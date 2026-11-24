/*
 * SPDX-FileCopyrightText: 2022 Sharaf Zaman <shzam@sdf.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

package org.krimble.android;

import static android.os.Process.killProcess;
import static android.os.Process.myPid;

import android.app.ForegroundServiceStartNotAllowedException;
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

import androidx.annotation.RequiresApi;

import org.krimble.R;

public class DocumentSaverService extends Service {

    private final String TAG = "DocumentSaverService";
    private final String CHANNEL_ID = "org.krimble.android";
    private final int NOTIFICATION_ID = 3;

    public static final String START_SAVING = "START_SAVING";
    public static final String KILL_PROCESS = "KILL_PROCESS";
    private static final String CANCEL_SAVING = "CANCEL_SAVING";

    private boolean mStartedInForeground = false;
    private boolean mSaveAgain = false;
    private boolean mKillProcess = false;
    private Thread mDocSaverThread = null;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "[onCreate]");

        createNotificationChannel();
    }

    private Notification getNotification() {
        Notification notification;
        Intent intent = new Intent(this, DocumentSaverService.class);
        intent.putExtra(CANCEL_SAVING, true);
        PendingIntent cancelPendingIntent = PendingIntent.getService(this, 0,
                intent, PendingIntent.FLAG_IMMUTABLE);
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            notification = new Notification.Builder(this, CHANNEL_ID)
                    .setContentTitle(getString(R.string.save_notification_text))
                    .setSmallIcon(R.drawable.ic_save_notification)
                    .addAction(new Notification.Action.Builder(null,
                            getString(R.string.cancel_save_notification), cancelPendingIntent).build())
                    .setProgress(0, 0, true)
                    .build();
        } else {
            notification = new Notification.Builder(this)
                    .setContentTitle(getString(R.string.save_notification_text))
                    .setSmallIcon(R.drawable.ic_save_notification)
                    .setPriority(Notification.PRIORITY_LOW)
                    .setProgress(0, 0, true)
                    .build();
        }

        return notification;
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            CharSequence name = "Default";
            int importance = NotificationManager.IMPORTANCE_LOW;
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, name, importance);

            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.S)
    private void tryStartServiceInForegroundS() {
        try {
            tryStartServiceInForegroundQ();
        } catch (ForegroundServiceStartNotAllowedException e) {
            Log.w(TAG, "Could not run the service in foreground: " + e);
            mStartedInForeground = false;
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.Q)
    private void tryStartServiceInForegroundQ() {
        Log.i(TAG, "Starting foreground short service");
        startForeground(
            NOTIFICATION_ID, getNotification(),
            ServiceInfo.FOREGROUND_SERVICE_TYPE_SHORT_SERVICE);
        mStartedInForeground = true;
    }

    private void tryStartServiceInForeground() {
        Log.i(TAG, "Starting foreground service");
        startForeground(NOTIFICATION_ID, getNotification());
        mStartedInForeground = true;
    }

    private synchronized void resetSaveAgain() {
        Log.i(TAG, "Start saving, foreground " + mStartedInForeground + " again " + mSaveAgain + " kill " + mKillProcess);
        mSaveAgain = false;
    }

    private synchronized boolean shouldSaveAgain() {
        Log.i(TAG, "Saving done, foreground " + mStartedInForeground + " again " + mSaveAgain + " kill " + mKillProcess);
        return mSaveAgain && !mKillProcess;
    }

    private synchronized void finishSaverThread() {
        if (mStartedInForeground) {
            mStartedInForeground = false;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                stopForeground(STOP_FOREGROUND_REMOVE);
            } else {
                stopForeground(true);
            }
        }
        mDocSaverThread = null;
    }

    private void runSaverThread() {
        do {
            resetSaveAgain();
            try {
                JNIWrappers.saveState();
            } catch (UnsatisfiedLinkError e) {
                Log.w(TAG, "Unsatisfied link error while saving: " + e);
                break;
            }
        } while (shouldSaveAgain());
        finishSaverThread();
    }

    private synchronized void startSaving() {
        if (mDocSaverThread == null) {
            if (mStartedInForeground) {
                Log.w(TAG, "Already started in foreground, should not happen");
            } else {
                Log.i(TAG, "Attempting to promote to foreground service");
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                    tryStartServiceInForegroundS();
                } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                    tryStartServiceInForegroundQ();
                } else {
                    tryStartServiceInForeground();
                }
            }
            Log.i(TAG, "Starting save thread");
            mDocSaverThread =
                    new Thread(
                            new Runnable() {
                                @Override
                                public void run() {
                                    runSaverThread();
                                }
                            });
            mDocSaverThread.start();
        } else {
            Log.i(TAG, "Save thread already running, setting save again flag");
            mSaveAgain = true;
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "[onStartCommand]");
        if (intent.getBooleanExtra(START_SAVING, false)) {
            startSaving();

        } else if (intent.getBooleanExtra(KILL_PROCESS, false)) {
            Log.i(TAG, "kill process command received");

            boolean needsThread;
            synchronized (this) {
                mKillProcess = true;
                needsThread = mStartedInForeground;
            }

            if (needsThread) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        waitForSaving();
                        stopSelf();
                    }
                }).start();
            } else {
                waitForSaving();
                stopSelf();
            }

        } else if (intent.getBooleanExtra(CANCEL_SAVING, false)) {
            Log.i(TAG, "cancel saving command received");
            stopSelf();

            killProcess(myPid());

        } else {
            Log.i(TAG, "nothing to do");
        }

        return START_NOT_STICKY;
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "Destroying: Kill Process = " + mKillProcess);
        waitForSaving();

        if (mKillProcess) {
            killProcess(myPid());
        }
        super.onDestroy();
    }

    private synchronized Thread getThreadToWaitOn() {
        Log.i(TAG, "[waitForSaving]: mStartedInForeground: " + mStartedInForeground);
        return mDocSaverThread;
    }

    private void waitForSaving() {
        Thread thread = getThreadToWaitOn();
        if (thread != null) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                Log.e(TAG, "Saving Interrupted :" + e.getMessage());
            }
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
