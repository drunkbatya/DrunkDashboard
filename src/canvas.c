#include "canvas.h"
#include <stdbool.h>
#include <malloc.h>

Canvas* canvas_init(u8x8_msg_cb u8g2_comm_cb, u8x8_msg_cb u8g2_gpio_and_delay_cb) {
    Canvas* canvas = malloc(sizeof(Canvas));

    u8g2_Setup_ssd1306_128x64_noname_f(&canvas->fb, U8G2_R0, u8g2_comm_cb, u8g2_gpio_and_delay_cb);

    uint8_t* display_fb = malloc(u8g2_GetBufferSize(&canvas->fb));
    u8g2_SetBufferPtr(&canvas->fb, display_fb);

    u8g2_InitDisplay(&canvas->fb);
    u8g2_SetPowerSave(&canvas->fb, 0);

    canvas_set_font(canvas, CanvasFontSecondary);
    canvas->width = u8g2_GetDisplayWidth(&canvas->fb);
    canvas->height = u8g2_GetDisplayHeight(&canvas->fb);
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

static void canvas_draw_u8g2_bitmap_int(
    u8g2_t* u8g2,
    u8g2_uint_t x,
    u8g2_uint_t y,
    u8g2_uint_t w,
    u8g2_uint_t h,
    bool mirror,
    bool rotation,
    const uint8_t* bitmap) {
    u8g2_uint_t blen;
    blen = w;
    blen += 7;
    blen >>= 3;

    if(rotation && !mirror) {
        x += w + 1;
    } else if(mirror && !rotation) {
        y += h - 1;
    }

    while(h > 0) {
        const uint8_t* b = bitmap;
        uint16_t len = w;
        uint16_t x0 = x;
        uint16_t y0 = y;
        uint8_t mask;
        uint8_t color = u8g2->draw_color;
        uint8_t ncolor = (color == 0 ? 1 : 0);
        mask = 1;

        while(len > 0) {
            if(u8x8_pgm_read(b) & mask) {
                u8g2->draw_color = color;
                u8g2_DrawHVLine(u8g2, x0, y0, 1, 0);
            } else if(u8g2->bitmap_transparency == 0) {
                u8g2->draw_color = ncolor;
                u8g2_DrawHVLine(u8g2, x0, y0, 1, 0);
            }

            if(rotation) {
                y0++;
            } else {
                x0++;
            }

            mask <<= 1;
            if(mask == 0) {
                mask = 1;
                b++;
            }
            len--;
        }

        u8g2->draw_color = color;
        bitmap += blen;

        if(mirror) {
            if(rotation) {
                x++;
            } else {
                y--;
            }
        } else {
            if(rotation) {
                x--;
            } else {
                y++;
            }
        }
        h--;
    }
}

static void canvas_draw_u8g2_bitmap(
    u8g2_t* u8g2,
    u8g2_uint_t x,
    u8g2_uint_t y,
    u8g2_uint_t w,
    u8g2_uint_t h,
    const uint8_t* bitmap,
    CanvasIconRotation rotation) {
    u8g2_uint_t blen;
    blen = w;
    blen += 7;
    blen >>= 3;
#ifdef U8G2_WITH_INTERSECTION
    if(u8g2_IsIntersection(u8g2, x, y, x + w, y + h) == 0) return;
#endif /* U8G2_WITH_INTERSECTION */

    switch(rotation) {
    case CanvasIconRotation0:
        canvas_draw_u8g2_bitmap_int(u8g2, x, y, w, h, 0, 0, bitmap);
        break;
    case CanvasIconRotation90:
        canvas_draw_u8g2_bitmap_int(u8g2, x, y, w, h, 0, 1, bitmap);
        break;
    case CanvasIconRotation180:
        canvas_draw_u8g2_bitmap_int(u8g2, x, y, w, h, 1, 0, bitmap);
        break;
    case CanvasIconRotation270:
        canvas_draw_u8g2_bitmap_int(u8g2, x, y, w, h, 1, 1, bitmap);
        break;
    default:
        break;
    }
}

void canvas_draw_icon(Canvas* canvas, uint8_t x, uint8_t y, const Icon* icon) {
    const uint8_t* bitmap = icon_get_data(icon);
    bitmap++; // TODO: skipping first 0 byte, what the hell is this?
    canvas_draw_u8g2_bitmap(
        &canvas->fb,
        x,
        y,
        icon_get_width(icon),
        icon_get_height(icon),
        bitmap,
        CanvasIconRotation0);
}

void canvas_draw_box(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    u8g2_DrawBox(&canvas->fb, x, y, width, height);
}

void canvas_draw_rbox(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    uint8_t radius) {
    u8g2_DrawRBox(&canvas->fb, x, y, width, height, radius);
}

void canvas_draw_frame(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    u8g2_DrawFrame(&canvas->fb, x, y, width, height);
}

void canvas_draw_rframe(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t width,
    uint8_t height,
    uint8_t radius) {
    u8g2_DrawRFrame(&canvas->fb, x, y, width, height, radius);
}

void canvas_draw_line(Canvas* canvas, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    u8g2_DrawLine(&canvas->fb, x1, y1, x2, y2);
}

void canvas_draw_circle(Canvas* canvas, uint8_t x, uint8_t y, uint8_t radius) {
    u8g2_DrawCircle(&canvas->fb, x, y, radius, U8G2_DRAW_ALL);
}

void canvas_draw_disc(Canvas* canvas, uint8_t x, uint8_t y, uint8_t radius) {
    u8g2_DrawDisc(&canvas->fb, x, y, radius, U8G2_DRAW_ALL);
}

void canvas_draw_triangle(Canvas* canvas, uint8_t x, uint8_t y, uint8_t base, uint8_t height) {
    canvas_draw_line(canvas, x - base / 2, y, x + base / 2, y);
    canvas_draw_line(canvas, x - base / 2, y, x, y + height - 1);
    canvas_draw_line(canvas, x, y + height - 1, x + base / 2, y);
}

uint8_t canvas_get_width(Canvas* canvas) {
    return canvas->width;
}

uint8_t canvas_get_height(Canvas* canvas) {
    return canvas->height;
}

void canvas_set_color(Canvas* canvas, CanvasColor color) {
    u8g2_SetDrawColor(&canvas->fb, color);
}

void canvas_invert_color(Canvas* canvas) {
    u8g2_SetDrawColor(&canvas->fb, !u8g2_GetDrawColor(&canvas->fb));
}

void canvas_set_font(Canvas* canvas, CanvasFont font) {
    u8g2_SetFontMode(&canvas->fb, 1);
    if(font == CanvasFontPrimary) {
        u8g2_SetFont(&canvas->fb, u8g2_font_helvB08_tr);
    } else if(font == CanvasFontSecondary) {
        u8g2_SetFont(&canvas->fb, u8g2_font_haxrcorp4089_tr);
    } else if(font == CanvasFontKeyboard) {
        u8g2_SetFont(&canvas->fb, u8g2_font_profont11_mr);
    } else if(font == CanvasFontBigNumbers) {
        u8g2_SetFont(&canvas->fb, u8g2_font_profont22_tn);
    } else {
        return;
    }
}
