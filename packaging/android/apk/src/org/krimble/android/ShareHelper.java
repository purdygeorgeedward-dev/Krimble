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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
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

    // Krimble 2026-10-15: Take Photo, troubleshooting (George, 1.0.36: the camera opened and the photo was taken, then Krimble said "The file
    // content://media/external/images/media/1000356140 does not exist." and nothing opened). Copies what the camera wrote to the address of
    // prepareCameraUri into the app's own cache and returns the path of the copy, which Krimble can open like any file. If it cannot, it returns
    // "EMPTY" (the camera app saved nothing to that address) or "ERROR: ..." with the reason, so the answer is shown instead of a guess.
    public static String copyCameraPhoto(final String uriString) {
        final Activity activity = QtNative.activity();
        if (activity == null) {
            return "ERROR: no activity";
        }
        try {
            ContentResolver resolver = activity.getContentResolver();
            InputStream in = resolver.openInputStream(Uri.parse(uriString));
            if (in == null) {
                return "ERROR: the picture could not be opened (no stream) at " + uriString;
            }
            File dir = new File(activity.getCacheDir(), "camera");
            dir.mkdirs();
            File out = new File(dir, "photo-" + System.currentTimeMillis() + ".jpg");
            OutputStream os = new FileOutputStream(out);
            byte[] buffer = new byte[65536];
            long total = 0;
            int count;
            while ((count = in.read(buffer)) > 0) {
                os.write(buffer, 0, count);
                total += count;
            }
            os.close();
            in.close();
            if (total == 0) {
                out.delete();
                return "EMPTY";
            }
            return out.getAbsolutePath();
        } catch (Exception e) {
            Log.w(TAG, "copy camera photo failed: " + e);
            return "ERROR: " + e;
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
