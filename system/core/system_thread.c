#include "system_thread.h"

SystemThread
    system_thread_start(const char* name, void* function, void* context, uint32_t stack_size) {
    const osThreadAttr_t thread_attributes = {
        .name = name,
        .stack_size = stack_size,
        .priority = (osPriority_t)osPriorityNormal,
    };
    return (osThreadNew(function, NULL, &thread_attributes));
}
