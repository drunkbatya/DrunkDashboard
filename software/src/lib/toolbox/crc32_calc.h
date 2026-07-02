#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t crc32_calc_buffer(uint32_t crc, const void* buffer, size_t size);

#ifdef __cplusplus
}
#endif
