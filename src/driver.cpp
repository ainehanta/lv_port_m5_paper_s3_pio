#include <lvgl.h>
#include <M5Unified.hpp>

void _print_cb(lv_log_level_t level, const char *buf)
{
    switch (level)
    {
    case LV_LOG_LEVEL_TRACE:
        M5_LOGV("%s", buf);
        break;
    case LV_LOG_LEVEL_INFO:
        M5_LOGD("%s", buf);
        break;
    case LV_LOG_LEVEL_WARN:
        M5_LOGW("%s", buf);
        break;
    case LV_LOG_LEVEL_ERROR:
        M5_LOGE("%s", buf);
        break;
    case LV_LOG_LEVEL_USER:
        M5_LOGI("%s", buf);
        break;
    default:
        M5_LOGD("%s", buf);
        break;
    }
};

void init_log(void)
{
    lv_log_register_print_cb(_print_cb);
}

uint32_t _tick(void)
{
    return M5.millis();
}

void init_tick(void)
{
    lv_tick_set_cb(_tick);
}

// based on https://github.com/m5stack/lv_m5_emulator/blob/32898dbb5497644e753305947e7f3c2a0a2caa20/src/utility/lvgl_port_m5stack.cpp#L190
void _flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    M5_LOGV("flush!");
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    uint32_t pixels = w * h;

    M5.Display.startWrite();
    M5.Display.setAddrWindow(area->x1, area->y1, w, h);

    // Critical fix: Use safe pixel writing method to avoid M5GFX SIMD optimizations
    // Break large transfers into small chunks to avoid problematic copy_rgb_fast function
    const uint32_t SAFE_CHUNK_SIZE = 8192; // 8K pixels per chunk, suitable for small buffer settings

    if (pixels > SAFE_CHUNK_SIZE)
    {
        // Chunked transmission for large data
        const lgfx::grayscale_t *src = (const lgfx::grayscale_t *)px_map;
        uint32_t remaining = pixels;
        uint32_t offset = 0;

        while (remaining > 0)
        {
            uint32_t chunk_size = (remaining > SAFE_CHUNK_SIZE) ? SAFE_CHUNK_SIZE : remaining;
            M5.Display.writePixels(src + offset, chunk_size);
            offset += chunk_size;
            remaining -= chunk_size;
        }
    }
    else
    {
        // Direct transmission for small data
        M5.Display.writePixels((lgfx::grayscale_t *)px_map, pixels);
    }

    M5.Display.endWrite();

    lv_display_flush_ready(disp);
}

lv_display_t *init_display(void)
{
    const auto w = M5.Display.width();
    const auto h = M5.Display.height();
    const auto buf_size = w * h / 10 * 2;

    lv_display_t *disp = lv_display_create(w, h);

#if defined(BOARD_HAS_PSRAM)
    static uint8_t *buf =
        (uint8_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
#else
    static uint8_t *buf = (uint8_t *)malloc(buf_size);
#endif

    lv_display_set_buffers(disp, buf, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(disp, _flush_cb);

    return disp;
}

void _read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;

    bool touched = M5.Display.getTouch(&touchX, &touchY);
    if (!touched)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

lv_indev_t *init_indev(void)
{
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, _read_cb);
    return indev;
}