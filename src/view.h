#pragma once

#include <canvas.h>
#include <stdbool.h>

typedef struct View View;

typedef void (*ViewDrawCallback)(Canvas* canvas, void* model);
// typedef bool (*ViewInputCallback)(InputEvent* event, void* context);
typedef bool (*ViewCustomCallback)(uint32_t event, void* context);
