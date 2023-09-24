#include "drunk_dashboard_system.h"
#include <cmsis_os2.h>

void drunk_dashboard_system_init() {
    osKernelInitialize();
}

void drunk_dashboard_system_start_kernel() {
    osKernelStart();
}
