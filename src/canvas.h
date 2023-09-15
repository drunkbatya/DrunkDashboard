#pragma once

#include <u8g2/u8g2.h>

typedef enum {
    CanvasColorWhite = 0x00,
    CanvasColorBlack = 0x01,
    CanvasColorXOR = 0x02,
} CanvasColor;

typedef struct {
    u8g2_t fb;
} Canvas;

Canvas* canvas_init(u8x8_msg_cb u8g2_comm_cb, u8x8_msg_cb u8g2_gpio_and_delay_cb);
void canvas_free(Canvas* canvas);
void canvas_commit(Canvas* canvas);
void canvas_clear(Canvas* canvas);
void canvas_draw_str(Canvas* canvas, uint8_t x, uint8_t y, const char* str);
