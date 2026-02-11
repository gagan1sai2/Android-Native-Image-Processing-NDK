#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <cstring>
#include <thread>

#define LOG_TAG "NDK_PROCESS"
#define LOGD(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


// =====================
// MULTI-THREAD GRAYSCALE
// =====================
extern "C"
JNIEXPORT void JNICALL
Java_com_gagansai_imageprocessor_MainActivity_applyGrayFilter(
        JNIEnv *env,
        jobject,
        jobject bitmap) {

    AndroidBitmapInfo info;
    void* pixels;

    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) return;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) return;

    uint32_t* data = (uint32_t*)pixels;

    int width = info.width;
    int height = info.height;

    auto graySection = [&](int startY, int endY) {
        for (int y = startY; y < endY; y++) {
            for (int x = 0; x < width; x++) {

                int index = y * width + x;
                uint32_t pixel = data[index];

                uint8_t r = (pixel >> 16) & 0xff;
                uint8_t g = (pixel >> 8) & 0xff;
                uint8_t b = pixel & 0xff;

                uint8_t gray = (r + g + b) / 3;

                data[index] =
                        (0xff << 24) | (gray << 16) | (gray << 8) | gray;
            }
        }
    };

    int mid = height / 2;

    std::thread t1(graySection, 0, mid);
    std::thread t2(graySection, mid, height);

    t1.join();
    t2.join();

    AndroidBitmap_unlockPixels(env, bitmap);
}


// =====================
// OPTIMIZED MULTI-THREAD BLUR
// =====================
extern "C"
JNIEXPORT void JNICALL
Java_com_gagansai_imageprocessor_MainActivity_applyBlurFilter(
        JNIEnv *env,
        jobject,
        jobject bitmap) {

    AndroidBitmapInfo info;
    void* pixels;

    if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) return;
    if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) return;

    uint32_t* data = (uint32_t*)pixels;

    int width = info.width;
    int height = info.height;

    auto blurSection = [&](int startY, int endY) {

        for (int y = startY; y < endY; y++) {

            // Skip borders
            if (y == 0 || y == height - 1)
                continue;

            for (int x = 1; x < width - 1; x++) {

                int r = 0, g = 0, b = 0;

                for (int ky = -1; ky <= 1; ky++) {
                    uint32_t* row = data + (y + ky) * width;

                    for (int kx = -1; kx <= 1; kx++) {

                        uint32_t pixel = row[x + kx];

                        r += (pixel >> 16) & 0xff;
                        g += (pixel >> 8) & 0xff;
                        b += pixel & 0xff;
                    }
                }

                r /= 9;
                g /= 9;
                b /= 9;

                data[y * width + x] =
                        (0xff << 24) | (r << 16) | (g << 8) | b;
            }
        }
    };

    int mid = height / 2;

    std::thread t1(blurSection, 1, mid);
    std::thread t2(blurSection, mid, height - 1);

    t1.join();
    t2.join();

    AndroidBitmap_unlockPixels(env, bitmap);
}
