#include "compress.h"

#include <furi.h>
#include <stdlib.h>

struct CompressIcon {
    uint8_t unused;
};

CompressIcon* compress_icon_alloc(size_t decode_buf_size) {
    UNUSED(decode_buf_size);

    CompressIcon* instance = malloc(sizeof(CompressIcon));
    furi_check(instance);
    return instance;
}

void compress_icon_free(CompressIcon* instance) {
    furi_check(instance);
    free(instance);
}

void compress_icon_decode(CompressIcon* instance, const uint8_t* icon_data, uint8_t** output) {
    furi_check(instance);
    furi_check(icon_data);
    furi_check(output);

    furi_check(icon_data[0] == 0x00);
    *output = (uint8_t*)&icon_data[1];
}
