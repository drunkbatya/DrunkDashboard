#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CompressIcon CompressIcon;

CompressIcon* compress_icon_alloc(size_t decode_buf_size);

void compress_icon_free(CompressIcon* instance);

void compress_icon_decode(CompressIcon* instance, const uint8_t* icon_data, uint8_t** output);

#ifdef __cplusplus
}
#endif
