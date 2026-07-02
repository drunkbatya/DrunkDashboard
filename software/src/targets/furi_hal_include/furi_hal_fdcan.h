#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FURI_HAL_FDCAN_MAX_FRAME_SIZE 64U

typedef struct {
    uint32_t id;
    uint32_t data_size;
    uint8_t data[FURI_HAL_FDCAN_MAX_FRAME_SIZE];
} FuriHalFdcanMessage;

void furi_hal_fdcan_init(void);
void furi_hal_fdcan_deinit(void);
bool furi_hal_fdcan_get_message(FuriHalFdcanMessage* message);

#ifdef __cplusplus
}
#endif
