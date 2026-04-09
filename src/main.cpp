#include <lvgl.h>
#include <M5Unified.h>

#include "./driver.hpp"

void setup(void)
{
    M5.begin();
    lv_init();
    init_tick();
    init_display();

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello Arduino, I'm LVGL!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void loop(void)
{
    M5.update();
    lv_timer_handler();
}