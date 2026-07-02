#pragma once

#include <stdint.h>
#include <mlib/m-dict.h>

#include "dashboard_can_worker.h"

#include <furi.h>
#include <furi_hal_fdcan.h>

typedef enum {
    DashboardCanWorkerModeDashboard,
    DashboardCanWorkerModeMonitor
} DashboardCanWorkerMode;

#define CAN_RX_QUEUE_SIZE 32

typedef struct {
    uint32_t count;
    uint32_t last_len;
    uint8_t last_data[FURI_HAL_FDCAN_MAX_FRAME_SIZE];
} CanMsg;

DICT_DEF2(can_msgs_dict, uint32_t, M_DEFAULT_OPLIST, CanMsg, M_POD_OPLIST)

struct DashboardCanWorker {
    FuriThread* thread;
    DashboardCanWorkerMode mode;
    FuriMessageQueue* can_rx_queue;
    FuriMutex* can_messages_mutex;
    can_msgs_dict_t can_messages;
    DashboardCanWorkerDataUpdatedCallback data_updated_callback;
    void* data_updated_callback_context;
    uint32_t can_rx_dropped;
};

typedef enum {
    DashboardCanWorkerEventStop = (1 << 0),
    DashboardCanWorkerEventMsgRecieved = (1 << 1),
    DashboardCanWorkerEventDataUpdated = (1 << 2),
    DashboardCanWorkerEventAll = DashboardCanWorkerEventStop | DashboardCanWorkerEventMsgRecieved |
                                 DashboardCanWorkerEventDataUpdated
} DashboardCanWorkerEvent;
