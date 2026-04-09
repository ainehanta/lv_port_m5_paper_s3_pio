#include <lvgl.h>
#include <M5Unified.h>

#include "./driver.hpp"

void setup(void)
{
    M5.begin();
    lv_init();

    init_tick();
    auto display = init_display();
    auto indev = init_indev();
    lv_indev_set_display(indev, display);

    lv_obj_t *button = lv_button_create(lv_screen_active());
    lv_obj_center(button);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, "Hello, M5Unified!");
    lv_obj_center(label);
}

void loop(void)
{
    M5.update();
    lv_timer_handler();
    M5.delay(5);
}