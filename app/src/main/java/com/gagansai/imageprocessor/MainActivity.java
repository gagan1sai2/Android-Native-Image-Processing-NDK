package com.gagansai.imageprocessor;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("imageprocessor");
    }

    private native void applyGrayFilter(Bitmap bitmap);
    private native void applyBlurFilter(Bitmap bitmap);

    private Bitmap originalBitmap;
    private Bitmap workingBitmap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ImageView imageView = findViewById(R.id.imageView);
        Button btnGray = findViewById(R.id.btnGray);
        Button btnBlur = findViewById(R.id.btnBlur);

        // Load image and upscale for proper benchmarking
        Bitmap temp = BitmapFactory.decodeResource(getResources(), R.drawable.sample);
        originalBitmap = Bitmap.createScaledBitmap(temp, 1920, 1080, true);

        workingBitmap = originalBitmap.copy(Bitmap.Config.ARGB_8888, true);
        imageView.setImageBitmap(workingBitmap);

        // =========================
        // C++ Grayscale
        // =========================
        btnGray.setOnClickListener(v -> {

            workingBitmap = originalBitmap.copy(Bitmap.Config.ARGB_8888, true);

            long start = System.nanoTime();
            applyGrayFilter(workingBitmap);
            long end = System.nanoTime();

            double timeMs = (end - start) / 1_000_000.0;
            Log.d("CPP_GRAY", "C++ Grayscale Time: " + timeMs + " ms");

            imageView.setImageBitmap(workingBitmap);
        });

        // =========================
        // Multi-thread Blur
        // =========================
        btnBlur.setOnClickListener(v -> {

            workingBitmap = originalBitmap.copy(Bitmap.Config.ARGB_8888, true);

            long start = System.nanoTime();
            applyBlurFilter(workingBitmap);
            long end = System.nanoTime();

            double timeMs = (end - start) / 1_000_000.0;
            Log.d("CPP_BLUR", "Multi-thread Blur Time: " + timeMs + " ms");

            imageView.setImageBitmap(workingBitmap);
        });
    }
}
