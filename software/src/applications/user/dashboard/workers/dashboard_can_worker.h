#pragma once

#include <stddef.h>
#include <stdint.h>

#include <furi_hal_fdcan.h>

#define DASHBOARD_CAN_MAX_IDS 256U

typedef void (*DashboardCanWorkerDataUpdatedCallback)(void* context);

typedef struct DashboardCanWorker DashboardCanWorker;

typedef struct {
    uint32_t id;
    uint32_t count;
    uint32_t last_len;
    uint8_t last_data[FURI_HAL_FDCAN_MAX_FRAME_SIZE];
} DashboardCanFrame;

DashboardCanWorker* dashboard_can_worker_alloc(void);
void dashboard_can_worker_start(DashboardCanWorker* worker);
void dashboard_can_worker_free(DashboardCanWorker* worker);
void dashboard_can_worker_send_stop(DashboardCanWorker* worker);
void dashboard_can_worker_await_stop(DashboardCanWorker* worker);
void dashboard_can_worker_set_data_updated_callback(
    DashboardCanWorker* worker,
    DashboardCanWorkerDataUpdatedCallback cb,
    void* ctx);
uint32_t dashboard_can_worker_get_count(DashboardCanWorker* worker);
size_t dashboard_can_worker_copy_snapshot(
    DashboardCanWorker* worker,
    DashboardCanFrame* frames,
    size_t frames_count);
bool dashboard_can_worker_get_frame(
    DashboardCanWorker* worker,
    uint32_t id,
    DashboardCanFrame* frame);
