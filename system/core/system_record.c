#include "system_event_flag.h"
#include "system_record.h"
#include "system_mutex.h"
#include "system_check.h"
#include <mlib/m-dict.h>

#define SYSTEM_RECORD_FLAG_READY (0x1)

typedef struct {
    SystemEventFlag* flags;
    void* data;
    size_t holders_count;
} SystemRecordData;

DICT_DEF2(SystemRecordDataDict, const char*, M_CSTR_OPLIST, SystemRecordData, M_POD_OPLIST)

typedef struct {
    SystemMutex* mutex;
    SystemRecordDataDict_t records;
} SystemRecord;

static SystemRecord* system_record = NULL;

static SystemRecordData* system_record_get(const char* name) {
    return SystemRecordDataDict_get(system_record->records, name);
}

static void system_record_put(const char* name, SystemRecordData* record_data) {
    SystemRecordDataDict_set_at(system_record->records, name, *record_data);
}

static void system_record_erase(const char* name, SystemRecordData* record_data) {
    system_event_flag_free(record_data->flags);
    SystemRecordDataDict_erase(system_record->records, name);
}

void system_record_init() {
    system_record = malloc(sizeof(SystemRecord));
    system_record->mutex = system_mutex_alloc(SystemMutexTypeNormal);
    system_check(system_record->mutex);
    SystemRecordDataDict_init(system_record->records);
}

static SystemRecordData* system_record_data_get_or_create(const char* name) {
    system_assert(system_record);
    SystemRecordData* record_data = system_record_get(name);
    if(!record_data) {
        SystemRecordData new_record;
        new_record.flags = system_event_flag_alloc();
        new_record.data = NULL;
        new_record.holders_count = 0;
        system_record_put(name, &new_record);
        record_data = system_record_get(name);
    }
    return record_data;
}

static void system_record_lock() {
    system_check(system_mutex_acquire(system_record->mutex, SystemWaitForever) == SystemStatusOk);
}

static void system_record_unlock() {
    system_check(system_mutex_release(system_record->mutex) == SystemStatusOk);
}

bool system_record_exists(const char* name) {
    system_assert(system_record);
    system_assert(name);

    bool ret = false;

    system_record_lock();
    ret = (system_record_get(name) != NULL);
    system_record_unlock();

    return ret;
}

void system_record_create(const char* name, void* data) {
    system_assert(system_record);

    system_record_lock();

    // Get record data and fill it
    SystemRecordData* record_data = system_record_data_get_or_create(name);
    system_assert(record_data->data == NULL);
    record_data->data = data;
    system_event_flag_set(record_data->flags, SYSTEM_RECORD_FLAG_READY);

    system_record_unlock();
}

bool system_record_destroy(const char* name) {
    system_assert(system_record);

    bool ret = false;

    system_record_lock();

    SystemRecordData* record_data = system_record_get(name);
    system_assert(record_data);
    if(record_data->holders_count == 0) {
        system_record_erase(name, record_data);
        ret = true;
    }

    system_record_unlock();

    return ret;
}

void* system_record_open(const char* name) {
    system_assert(system_record);

    system_record_lock();

    SystemRecordData* record_data = system_record_data_get_or_create(name);
    record_data->holders_count++;

    system_record_unlock();

    // Wait for record to become ready
    system_check(
        system_event_flag_wait(
            record_data->flags,
            SYSTEM_RECORD_FLAG_READY,
            SystemFlagWaitAny | SystemFlagNoClear,
            SystemWaitForever) == SYSTEM_RECORD_FLAG_READY);

    return record_data->data;
}

void system_record_close(const char* name) {
    system_assert(system_record);

    system_record_lock();

    SystemRecordData* record_data = system_record_get(name);
    system_assert(record_data);
    record_data->holders_count--;

    system_record_unlock();
}
