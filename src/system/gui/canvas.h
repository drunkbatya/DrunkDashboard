#pragma once

#include <u8g2/u8g2.h>
#include <assets_icons.h>
#include "icon.h"

typedef enum {
    CanvasColorWhite = 0x00,
    CanvasColorBlack = 0x01,
    CanvasColorXOR = 0x02,
} CanvasColor;

typedef enum {
    CanvasFontPrimary,
    CanvasFontSecondary,
    CanvasFontKeyboard,
    CanvasFontBigNumbers,
} CanvasFont;

typedef enum {
    CanvasIconFlipNone,
    CanvasIconFlipHorizontal,
    CanvasIconFlipVertical,
    CanvasIconFlipBoth,
} CanvasIconFlip;

typedef enum {
    CanvasIconRotation0,
    CanvasIconRotation90,
    CanvasIconRotation180,
    CanvasIconRotation270,
} CanvasIconRotation;

typedef struct Canvas Canvas;

Canvas* canvas_init(u8x8_msg_cb u8g2_comm_cb, u8x8_msg_cb u8g2_gpio_and_delay_cb);
void canvas_free(Canvas* canvas);
void canvas_commit(Canvas* canvas);
void canvas_clear(Canvas* canvas);
void canvas_reset(Canvas* canvas);
void canvas_draw_str(Canvas* canvas, uint8_t x, uint8_t y, const char* str);
void canvas_draw_icon(Canvas* canvas, uint8_t x, uint8_t y, const Icon* icon);
void canvas_draw_box(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void canvas_draw_box(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void canvas_draw_frame(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void canvas_draw_line(Canvas* canvas, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void canvas_draw_circle(Canvas* canvas, uint8_t x, uint8_t y, uint8_t r);
void canvas_draw_disc(Canvas* canvas, uint8_t x, uint8_t y, uint8_t r);
void canvas_draw_triangle(Canvas* canvas, uint8_t x, uint8_t y, uint8_t base, uint8_t height);
uint8_t canvas_get_width(Canvas* canvas);
uint8_t canvas_get_height(Canvas* canvas);
void canvas_set_color(Canvas* canvas, CanvasColor color);
void canvas_invert_color(Canvas* canvas);
void canvas_set_font(Canvas* canvas, CanvasFont font);
uint16_t canvas_get_string_width(Canvas* canvas, const char* str);
