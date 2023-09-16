#include "drunk_dashboard.h"
#include <canvas.h>
#include <stdbool.h>
#include <halk/halk_delay.h>
#include <u8g2/u8g2_glue.h>

#include <stdio.h>
#include <stdlib.h>

static void draw_display0(Canvas* canvas) {
    canvas_draw_str(canvas, 20, 20, "Hi from display0!");
    canvas_draw_box(canvas, 3, 3, 10, 10);
    // canvas_draw_frame(canvas, 0, 0, canvas_get_width(canvas0), canvas_get_height(canvas0));
}

static void draw_display1(Canvas* canvas) {
    canvas_draw_str(canvas, 20, 40, "Hi from display1!");
    canvas_draw_icon(canvas, 0, 0, &I_BatteryBody_52x28);
}

static void draw_display2(Canvas* canvas, char* str, uint32_t bar) {
    sprintf(str, "%lu.%.2lu", bar / 100, bar % 100);
    // canvas_draw_frame(canvas, 0, 0, canvas_get_width(canvas), canvas_get_height(canvas));
    canvas_draw_icon(canvas, 4, 8, &I_Turbocharger_59_48);
    canvas_set_font(canvas, CanvasFontPrimary);
    canvas_draw_str(canvas, 75, 10, "Pressure");
    canvas_set_font(canvas, CanvasFontBigNumbers);
    canvas_draw_str(canvas, 57, 52, str);
    canvas_set_font(canvas, CanvasFontSecondary);
    canvas_draw_str(canvas, 105, 51, "bar");
}

static void draw_display3(Canvas* canvas, char* str, uint32_t counter) {
    canvas_draw_str(canvas, 20, 20, "Hi from display3!");
    sprintf(str, "Count: %lu", counter);
    canvas_draw_str(canvas, 20, 40, str);
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

        draw_display0(canvas0);
        draw_display1(canvas1);
        draw_display2(canvas2, str, bar);
        draw_display3(canvas3, str, counter);

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
