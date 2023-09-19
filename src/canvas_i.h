#pragma once

#include "canvas.h"

struct Canvas {
    u8g2_t fb;
    uint8_t width;
    uint8_t height;
};
