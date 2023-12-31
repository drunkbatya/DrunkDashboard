#pragma once

#include <stdint.h>

typedef struct Icon Icon;

uint8_t icon_get_width(const Icon* instance);
uint8_t icon_get_height(const Icon* instance);
const uint8_t* icon_get_data(const Icon* instance);
