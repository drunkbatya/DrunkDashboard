#pragma once

#include <stdint.h>

#include <furi_hal_fdcan.h>

typedef struct {
    uint32_t id;
    uint32_t rx_count;
    uint32_t last_len;
    uint8_t last_data[FURI_HAL_FDCAN_MAX_FRAME_SIZE];
} DashboardCanFrame;
