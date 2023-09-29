#pragma once
#include <stdbool.h>

typedef void SystemMutex;

typedef enum {
    SystemMutexTypeNormal,
    SystemMutexTypeRecursive,
} SystemMutexType;

SystemMutex* system_mutex_alloc(SystemMutexType type);
void system_mutex_free(SystemMutex* mutex);
bool system_mutex_release(SystemMutex* mutex);
bool system_mutex_acquire(SystemMutex* instance, uint32_t timeout);
