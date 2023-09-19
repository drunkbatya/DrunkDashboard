#pragma once

#include <gui/canvas.h>
#include <stdbool.h>

typedef struct View View;

typedef void (*ViewDrawCallback)(Canvas* canvas, void* context);
// typedef bool (*ViewInputCallback)(InputEvent* event, void* context);
typedef bool (*ViewCustomCallback)(uint32_t event, void* context);

View* view_alloc();
void view_free(View* view);
void view_set_draw_callback(View* view, ViewDrawCallback callback);
void view_set_custom_callback(View* view, ViewCustomCallback callback);
void view_set_context(View* view, void* context);
void view_draw(View* view, Canvas* canvas);
