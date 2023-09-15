#include "drunk_dashboard.h"
#include <canvas.h>
#include <stdbool.h>
#include <halk/halk_delay.h>
#include <u8g2/u8g2_glue.h>

void drunk_dashboard() {
    Canvas* canvas0 = canvas_init(u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32_display0);
    Canvas* canvas1 = canvas_init(u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32_display1);
    while(true) {
        canvas_clear(canvas0);
        canvas_clear(canvas1);
        canvas_draw_str(canvas0, 10, 10, "Testing new lib!");
        canvas_draw_str(canvas1, 20, 20, "Hi from display1!");
        canvas_commit(canvas0);
        canvas_commit(canvas1);
        halk_delay_ms(50);
    }
    canvas_free(canvas0);
    canvas_free(canvas1);
}
