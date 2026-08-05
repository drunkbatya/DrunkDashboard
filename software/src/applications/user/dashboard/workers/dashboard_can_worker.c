#include "dashboard_can_worker_i.h"

#include <core/check.h>

#include <furi_hal_interrupt.h>
#include <furi_hal_fdcan.h>
#include <stm32u5xx_ll_fdcan.h>

#include <stdlib.h>
#include <string.h>

#define TAG "DashboardCanWorker"

static void dashboard_can_worker_lock(DashboardCanWorker* worker) {
    furi_check(furi_mutex_acquire(worker->can_messages_mutex, FuriWaitForever) == FuriStatusOk);
}

static void dashboard_can_worker_unlock(DashboardCanWorker* worker) {
    furi_check(furi_mutex_release(worker->can_messages_mutex) == FuriStatusOk);
}

static void dashboard_can_worker_execute_data_updated_callback(DashboardCanWorker* worker) {
    if(worker->data_updated_callback) {
        worker->data_updated_callback(worker->data_updated_callback_context);
    }
}

static bool dashboard_can_worker_update_insert_message(
    DashboardCanWorker* worker,
    const FuriHalFdcanMessage* message) {
    if(message->data_size > FURI_HAL_FDCAN_MAX_FRAME_SIZE) {
        return false;
    }

    bool data_updated = false;
    CanMsg* msg = can_msgs_dict_get(worker->can_messages, message->id);
    if(msg == NULL) {
        // dict is full
        if(can_msgs_dict_size(worker->can_messages) >= DASHBOARD_CAN_MAX_IDS) {
            return false;
        }

        // this will create a new element with id key
        msg = can_msgs_dict_safe_get(worker->can_messages, message->id);
        data_updated = true;
    }

    msg->rx_count++;
    if((msg->last_len != message->data_size) ||
       (memcmp(msg->last_data, message->data, message->data_size) != 0)) {
        memcpy(msg->last_data, message->data, message->data_size);
        msg->last_len = message->data_size;
        data_updated = true;
    }

    return data_updated;
}

static bool dashboard_can_worker_process_rx_queue(DashboardCanWorker* worker) {
    bool data_updated = false;
    FuriHalFdcanMessage message;

    dashboard_can_worker_lock(worker);
    while(furi_message_queue_get(worker->can_rx_queue, &message, 0) == FuriStatusOk) {
        data_updated |= dashboard_can_worker_update_insert_message(worker, &message);
    }
    dashboard_can_worker_unlock(worker);

    return data_updated;
}

static int32_t dashboard_can_worker_thread(void* arg) {
    DashboardCanWorker* worker = arg;
    furi_assert(worker);
    uint32_t events;
    while(1) {
        events =
            furi_thread_flags_wait(DashboardCanWorkerEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(events & DashboardCanWorkerEventStop) {
            break;
        }
        if(events & DashboardCanWorkerEventMsgRecieved) {
            if(dashboard_can_worker_process_rx_queue(worker)) {
                dashboard_can_worker_execute_data_updated_callback(worker);
            }
        }
    }

    return 0;
}

DashboardCanWorker* dashboard_can_worker_alloc(void) {
    DashboardCanWorker* worker = malloc(sizeof(DashboardCanWorker));
    worker->can_rx_queue =
        furi_message_queue_alloc(CAN_RX_QUEUE_SIZE, sizeof(FuriHalFdcanMessage));
    worker->can_messages_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    worker->thread = furi_thread_alloc_ex(TAG, 2048, dashboard_can_worker_thread, worker);
    furi_hal_fdcan_init();
    can_msgs_dict_init(worker->can_messages);
    return worker;
}

static void dashboard_can_worker_can_isr(void* context) {
    DashboardCanWorker* worker = context;
    bool rx_fifo0_new = LL_FDCAN_IsActiveFlag(FDCAN1, LL_FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
    bool rx_fifo0_full = LL_FDCAN_IsActiveFlag(FDCAN1, LL_FDCAN_IT_RX_FIFO0_FULL);

    if(rx_fifo0_new || rx_fifo0_full) {
        if(rx_fifo0_new) {
            LL_FDCAN_ClearFlag(FDCAN1, LL_FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
        }
        if(rx_fifo0_full) {
            LL_FDCAN_ClearFlag(FDCAN1, LL_FDCAN_IT_RX_FIFO0_FULL);
        }

        bool message_received = false;
        while(LL_FDCAN_GetRxFifoFillLevel(FDCAN1, LL_FDCAN_RX_FIFO0) > 0U) {
            FuriHalFdcanMessage message;
            if(!furi_hal_fdcan_get_message(&message)) {
                break;
            }

            message_received = true;
            if(furi_message_queue_put(worker->can_rx_queue, &message, 0) != FuriStatusOk) {
                worker->can_rx_dropped++;
            }
        }

        if(message_received) {
            furi_thread_flags_set(
                furi_thread_get_id(worker->thread), DashboardCanWorkerEventMsgRecieved);
        }
    }
    if(LL_FDCAN_IsActiveFlag(FDCAN1, LL_FDCAN_IT_RX_FIFO0_MESSAGE_LOST)) {
        LL_FDCAN_ClearFlag(FDCAN1, LL_FDCAN_IT_RX_FIFO0_MESSAGE_LOST);
    }
}

void dashboard_can_worker_start(DashboardCanWorker* worker) {
    furi_thread_start(worker->thread);
    furi_delay_ms(10);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdFdcan1It0, dashboard_can_worker_can_isr, worker);
}

void dashboard_can_worker_free(DashboardCanWorker* worker) {
    furi_hal_fdcan_deinit();
    can_msgs_dict_clear(worker->can_messages);
    furi_mutex_free(worker->can_messages_mutex);
    furi_message_queue_free(worker->can_rx_queue);
    furi_thread_free(worker->thread);
    free(worker);
}

void dashboard_can_worker_send_stop(DashboardCanWorker* worker) {
    furi_hal_interrupt_set_isr(FuriHalInterruptIdFdcan1It0, NULL, NULL);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), DashboardCanWorkerEventStop);
}

void dashboard_can_worker_await_stop(DashboardCanWorker* worker) {
    furi_thread_join(worker->thread);
    furi_message_queue_reset(worker->can_rx_queue);
    dashboard_can_worker_lock(worker);
    can_msgs_dict_reset(worker->can_messages);
    dashboard_can_worker_unlock(worker);
}

void dashboard_can_worker_set_data_updated_callback(
    DashboardCanWorker* worker,
    DashboardCanWorkerDataUpdatedCallback cb,
    void* ctx) {
    worker->data_updated_callback = cb;
    worker->data_updated_callback_context = ctx;
}

uint32_t dashboard_can_worker_get_count(DashboardCanWorker* worker) {
    dashboard_can_worker_lock(worker);
    uint32_t count = can_msgs_dict_size(worker->can_messages);
    dashboard_can_worker_unlock(worker);
    return count;
}

size_t dashboard_can_worker_copy_snapshot(
    DashboardCanWorker* worker,
    DashboardCanFrame* frames,
    size_t frames_count) {
    furi_check(worker);
    furi_check(frames);

    dashboard_can_worker_lock(worker);

    size_t written = 0;
    can_msgs_dict_it_t it;
    for(can_msgs_dict_it(it, worker->can_messages); !can_msgs_dict_end_p(it);
        can_msgs_dict_next(it)) {
        if(written >= frames_count) break;

        const can_msgs_dict_itref_t* item = can_msgs_dict_cref(it);
        frames[written].id = item->key;
        frames[written].rx_count = item->value.rx_count;
        frames[written].last_len = item->value.last_len;
        memcpy(frames[written].last_data, item->value.last_data, item->value.last_len);
        written++;
    }

    dashboard_can_worker_unlock(worker);

    for(size_t i = 1; i < written; i++) {
        DashboardCanFrame frame = frames[i];
        size_t j = i;
        while((j > 0) && (frames[j - 1].id > frame.id)) {
            frames[j] = frames[j - 1];
            j--;
        }
        frames[j] = frame;
    }

    return written;
}

bool dashboard_can_worker_get_frame(
    DashboardCanWorker* worker,
    uint32_t id,
    DashboardCanFrame* frame) {
    furi_check(worker);
    furi_check(frame);

    bool found = false;
    dashboard_can_worker_lock(worker);

    CanMsg* msg = can_msgs_dict_get(worker->can_messages, id);
    if(msg) {
        frame->id = id;
        frame->rx_count = msg->rx_count;
        frame->last_len = msg->last_len;
        memcpy(frame->last_data, msg->last_data, msg->last_len);
        found = true;
    }

    dashboard_can_worker_unlock(worker);
    return found;
}

void dashboard_can_worker_get_frames(
    DashboardCanWorker* worker,
    const uint32_t* ids,
    DashboardCanFrame* frames,
    bool* found,
    size_t count) {
    furi_check(worker);
    furi_check(ids);
    furi_check(frames);
    furi_check(found);

    dashboard_can_worker_lock(worker);
    for(size_t i = 0; i < count; i++) {
        CanMsg* msg = can_msgs_dict_get(worker->can_messages, ids[i]);
        if(msg) {
            frames[i].id = ids[i];
            frames[i].rx_count = msg->rx_count;
            frames[i].last_len = msg->last_len;
            memcpy(frames[i].last_data, msg->last_data, msg->last_len);
            found[i] = true;
        } else {
            found[i] = false;
        }
    }
    dashboard_can_worker_unlock(worker);
}
