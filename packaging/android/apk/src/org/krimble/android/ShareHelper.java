// Krimble 2026-10-12: Share (Claude's idea). Saves the picture the app has written into the phone's Pictures/Krimble folder
// (so it also shows up in the Gallery) and opens Android's share sheet for it: messages, email, social apps, cloud storage.
// Needs Android 10 or newer (older phones are told so).
package org.krimble.android;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.provider.MediaStore;
import android.util.Log;
import android.widget.Toast;

import org.qtproject.qt5.android.QtNative;

import java.io.FileInputStream;
import java.io.InputStream;
import java.io.OutputStream;

public class ShareHelper {
    private static final String TAG = "KrimbleShare";

    // Krimble 2026-10-13: Take Photo (Claude's idea; George: "Yes. Go for it."). Makes an empty picture entry in Pictures/Krimble/Camera
    // for the phone's camera app to write the photo into, and returns its address ("" when that is not possible: it needs Android 10).
    public static String prepareCameraUri() {
        final Activity activity = QtNative.activity();
        if (activity == null || Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            return "";
        }
        try {
            ContentValues values = new ContentValues();
            values.put(MediaStore.MediaColumns.DISPLAY_NAME, "Krimble-Photo-" + System.currentTimeMillis() + ".jpg");
            values.put(MediaStore.MediaColumns.MIME_TYPE, "image/jpeg");
            values.put(MediaStore.MediaColumns.RELATIVE_PATH, "Pictures/Krimble/Camera");
            Uri uri = activity.getContentResolver().insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
            return uri == null ? "" : uri.toString();
        } catch (Exception e) {
            Log.w(TAG, "camera uri failed: " + e);
            return "";
        }
    }

    // Removes the empty entry made by prepareCameraUri when the photo was cancelled.
    public static void deleteUri(final String uriString) {
        final Activity activity = QtNative.activity();
        if (activity == null) {
            return;
        }
        try {
            activity.getContentResolver().delete(Uri.parse(uriString), null, null);
        } catch (Exception e) {
            Log.w(TAG, "delete failed: " + e);
        }
    }

    // Krimble 2026-10-12: Batch presets (Claude's idea). Saves one picture into Pictures/Krimble/<folder> through the MediaStore, without
    // opening the share sheet. Needs Android 10 or newer.
    public static boolean saveToPictures(final String path, final String mime, final String displayName, final String folder) {
        final Activity activity = QtNative.activity();
        if (activity == null || Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            return false;
        }
        try {
            ContentResolver resolver = activity.getContentResolver();
            ContentValues values = new ContentValues();
            values.put(MediaStore.MediaColumns.DISPLAY_NAME, displayName);
            values.put(MediaStore.MediaColumns.MIME_TYPE, mime);
            values.put(MediaStore.MediaColumns.RELATIVE_PATH, "Pictures/Krimble/" + folder);
            Uri uri = resolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
            if (uri == null) {
                return false;
            }
            OutputStream out = resolver.openOutputStream(uri);
            InputStream in = new FileInputStream(path);
            try {
                byte[] buffer = new byte[64 * 1024];
                int read;
                while ((read = in.read(buffer)) > 0) {
                    out.write(buffer, 0, read);
                }
            } finally {
                in.close();
                out.close();
            }
            return true;
        } catch (Exception e) {
            Log.w(TAG, "save failed: " + e);
            return false;
        }
    }

    public static boolean shareFile(final String path, final String mime, final String displayName) {
        final Activity activity = QtNative.activity();
        if (activity == null) {
            return false;
        }
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(activity, "Share needs Android 10 or newer", Toast.LENGTH_LONG).show();
                }
            });
            return false;
        }
        try {
            ContentResolver resolver = activity.getContentResolver();
            ContentValues values = new ContentValues();
            values.put(MediaStore.MediaColumns.DISPLAY_NAME, displayName);
            values.put(MediaStore.MediaColumns.MIME_TYPE, mime);
            values.put(MediaStore.MediaColumns.RELATIVE_PATH, "Pictures/Krimble");
            Uri uri = resolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
            if (uri == null) {
                return false;
            }
            OutputStream out = resolver.openOutputStream(uri);
            InputStream in = new FileInputStream(path);
            try {
                byte[] buffer = new byte[64 * 1024];
                int read;
                while ((read = in.read(buffer)) > 0) {
                    out.write(buffer, 0, read);
                }
            } finally {
                in.close();
                out.close();
            }
            final Intent send = new Intent(Intent.ACTION_SEND);
            send.setType(mime);
            send.putExtra(Intent.EXTRA_STREAM, uri);
            send.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            final Intent chooser = Intent.createChooser(send, "Share");
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    activity.startActivity(chooser);
                }
            });
            return true;
        } catch (Exception e) {
            Log.w(TAG, "share failed: " + e);
            return false;
        }
    }
}
