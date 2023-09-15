#include "canvas.h"
#include <malloc.h>

Canvas* canvas_init(u8x8_msg_cb u8g2_comm_cb, u8x8_msg_cb u8g2_gpio_and_delay_cb) {
    Canvas* canvas = malloc(sizeof(Canvas));
    u8g2_Setup_ssd1306_128x64_noname_f(&canvas->fb, U8G2_R0, u8g2_comm_cb, u8g2_gpio_and_delay_cb);
    uint8_t* display_fb = malloc(u8g2_GetBufferSize(&canvas->fb));
    u8g2_SetBufferPtr(&canvas->fb, display_fb);
    u8g2_InitDisplay(&canvas->fb);
    u8g2_SetContrast(&canvas->fb, 0xFF);
    u8g2_SetPowerSave(&canvas->fb, 0);
    u8g2_SetFont(&canvas->fb, u8g2_font_haxrcorp4089_tr);
    canvas_clear(canvas);
    canvas_commit(canvas);
    return canvas;
}

void canvas_free(Canvas* canvas) {
    uint8_t* display_fb = u8g2_GetBufferPtr(&canvas->fb);
    free(display_fb);
    free(canvas);
}

void canvas_commit(Canvas* canvas) {
    u8g2_SendBuffer(&canvas->fb);
}

void canvas_clear(Canvas* canvas) {
    u8g2_ClearBuffer(&canvas->fb);
}

void canvas_draw_str(Canvas* canvas, uint8_t x, uint8_t y, const char* str) {
    if(!str) return;
    u8g2_DrawStr(&canvas->fb, x, y, str);
}
