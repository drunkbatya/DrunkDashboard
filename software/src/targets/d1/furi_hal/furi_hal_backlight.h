/**
 * @file furi_hal_backlight.h
 * Display backlight HAL
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void furi_hal_backlight_set(uint8_t percent);
uint8_t furi_hal_backlight_get(void);

#ifdef __cplusplus
}
#endif
