#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FuriHalEncoderCallback)(void* context);

void furi_hal_encoder_init(void);
void furi_hal_encoder_set_callback(FuriHalEncoderCallback callback, void* context);
void furi_hal_encoder_get_deltas(int32_t* vertical, int32_t* horizontal);

#ifdef __cplusplus
}
#endif
