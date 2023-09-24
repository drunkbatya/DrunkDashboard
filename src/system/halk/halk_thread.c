#include <cmsis_os2.h>

osThreadId_t halk_thread_start(const char* name, void* function, uint32_t stack_size) {
    const osThreadAttr_t thread_attributes = {
        .name = name,
        .stack_size = stack_size,
        .priority = (osPriority_t)osPriorityNormal,
    };
    return (osThreadNew(function, NULL, &thread_attributes));
}
