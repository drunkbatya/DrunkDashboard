#pragma once

#include <stdint.h>

typedef struct {
    const uint8_t width;
    const uint8_t height;
    const uint8_t frame_count;
    const uint8_t frame_rate;
    const uint8_t* const* frames;
} Icon;

uint8_t icon_get_width(const Icon* instance);
uint8_t icon_get_height(const Icon* instance);
const uint8_t* icon_get_data(const Icon* instance);
