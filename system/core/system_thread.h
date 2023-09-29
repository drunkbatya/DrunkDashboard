#pragma once

#include <cmsis_os2.h>

typedef osThreadId_t SystemThread;

SystemThread system_thread_start(const char* name, void* function, void* context, uint32_t stack_size);
