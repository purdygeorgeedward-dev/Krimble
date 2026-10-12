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
