#include <lvgl.h>

void app(void)
{
    lv_obj_t *button = lv_button_create(lv_screen_active());
    lv_obj_center(button);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, "Hello, M5Unified!");
    lv_obj_center(label);
}