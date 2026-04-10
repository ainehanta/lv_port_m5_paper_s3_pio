#include <M5Unified.h>
#include <lvgl.h>

#include "./driver.hpp"
#include "./app.hpp"

void setup(void)
{
    M5.begin();
    lv_init();

    init_tick();
    auto display = init_display();
    auto indev = init_indev();
    lv_indev_set_display(indev, display);

    app();
}

void loop(void)
{
    M5.update();
    lv_timer_handler();
    M5.delay(5);
}