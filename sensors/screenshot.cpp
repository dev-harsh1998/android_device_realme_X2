#define LOG_TAG "sensors-screenshot-realme-x2"
#define LOG_NDEBUG 1
#include <log/log.h>
#include "screenshot.h"
#include <gui/SurfaceComposerClient.h>
#include <time.h>
#include <cutils/properties.h>

using namespace android;

sp<GraphicBuffer> screen_buffer = NULL;
time_t last_screen_update = 0;

void update_screen_buffer(void **out) {
    if (screen_buffer == NULL) {
        screen_buffer = new GraphicBuffer(10, 10, PIXEL_FORMAT_RGB_888, GraphicBuffer::USAGE_SW_READ_OFTEN | GraphicBuffer::USAGE_SW_WRITE_OFTEN);
    }
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (now.tv_sec - last_screen_update >= SCREENSHOT_INTERVAL) {
        // Update Screenshot at most every second
        ScreenshotClient::capture(SurfaceComposerClient::getInternalDisplayToken(),
                                    ui::Dataspace::V0_SRGB, ui::PixelFormat::RGBA_8888,
                                    Rect(653, 46, 663, 56),
                                    10, 10, true, 0, &screen_buffer);
        last_screen_update = now.tv_sec;
    }
    screen_buffer->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, out);
}

void free_screen_buffer() {
    screen_buffer->unlock();
}
