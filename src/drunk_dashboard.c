#include "drunk_dashboard.h"
#include <canvas.h>
#include <stdbool.h>
#include <halk/halk_delay.h>
#include <u8g2/u8g2_glue.h>

#include <stdio.h>
#include <stdlib.h>

void draw_turbo_test(Canvas* canvas, char* str, uint32_t bar) {
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_set_font(canvas, CanvasFontPrimary);
    canvas_draw_str(canvas, 38, 11, "Pressure");
    canvas_draw_circle(canvas, 30, 35, 18);
    canvas_draw_line(canvas, 26, 17, 63, 17);
    canvas_draw_line(canvas, 46, 27, 63, 27);
    canvas_draw_line(canvas, 64, 16, 69, 16);
    canvas_draw_line(canvas, 69, 16, 69, 29);
    canvas_draw_line(canvas, 69, 29, 64, 29);
    canvas_draw_line(canvas, 64, 29, 64, 28);
    canvas_draw_circle(canvas, 30, 35, 9);
    canvas_draw_disc(canvas, 30, 35, 2);

    canvas_draw_line(canvas, 33, 37, 37, 38);
    canvas_draw_line(canvas, 31, 37, 32, 43);
    canvas_draw_line(canvas, 29, 38, 26, 42);
    canvas_draw_line(canvas, 29, 36, 22, 37);
    canvas_draw_line(canvas, 30, 36, 23, 31);
    canvas_draw_line(canvas, 31, 36, 27, 27);
    canvas_draw_line(canvas, 31, 36, 32, 28);
    canvas_draw_line(canvas, 31, 36, 37, 29);
    canvas_draw_line(canvas, 32, 37, 39, 33);

    canvas_set_font(canvas, CanvasFontBigNumbers);
    sprintf(str, "%lu.%.2lu", bar / 100, bar % 100);
    canvas_draw_str(canvas, 57, 51, str);
    canvas_set_font(canvas, CanvasFontSecondary);
    canvas_draw_str(canvas, 106, 51, "bar");
}

void drunk_dashboard() {
    Canvas* canvas0 = canvas_init(u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32_display0);
    Canvas* canvas1 = canvas_init(u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32_display1);
    Canvas* canvas2 = canvas_init(u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32_display2);
    Canvas* canvas3 = canvas_init(u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32_display3);
    uint32_t counter = 0;
    uint32_t bar = 0;
    char* str = malloc(100 * sizeof(char));
    while(true) {
        canvas_clear(canvas0);
        canvas_clear(canvas1);
        canvas_clear(canvas2);
        canvas_clear(canvas3);
        canvas_draw_str(canvas0, 20, 20, "Hi from display0!");
        canvas_draw_box(canvas0, 3, 3, 10, 10);
        canvas_draw_frame(canvas0, 0, 0, canvas_get_width(canvas0), canvas_get_height(canvas0));
        canvas_draw_str(canvas1, 20, 40, "Hi from display1!");
        canvas_draw_icon(canvas1, 0, 0, &I_BatteryBody_52x28);
        draw_turbo_test(canvas2, str, bar);
        canvas_draw_str(canvas3, 20, 20, "Hi from display3!");
        sprintf(str, "Count: %lu", counter);
        canvas_draw_str(canvas3, 20, 40, str);
        canvas_commit(canvas0);
        canvas_commit(canvas1);
        canvas_commit(canvas2);
        canvas_commit(canvas3);
        halk_delay_ms(500);
        counter++;
        bar += 3;
        if(bar >= 350) bar = 0;
    }
    free(str);
    canvas_free(canvas0);
    canvas_free(canvas1);
    canvas_free(canvas2);
    canvas_free(canvas3);
}
