#pragma once

#include <lvgl.h>

void init_log();
void init_tick();
lv_display_t *init_display();
lv_indev_t *init_indev();