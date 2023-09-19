#pragma once

#include "gui.h"
#include "canvas.h"

struct Gui {
    Canvas** canvases;
    View** views;
    uint8_t display_count;
};
