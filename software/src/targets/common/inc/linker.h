/**
 * @file linker.h
 *
 * Linker defined symbols. Used in various part of firmware to understand
 * hardware boundaries.
 *
 */
#pragma once

#ifdef __cplusplus
extern "C" {
typedef const char linker_symbol_t;
#else
typedef const void linker_symbol_t;
#endif

extern linker_symbol_t __heap_start__; /**< FreeRTOS heap start */
extern linker_symbol_t __heap_end__; /**< FreeRTOS heap end */

#ifdef __cplusplus
}
#endif
