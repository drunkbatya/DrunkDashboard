#include "system.h"
#include <system_thread.h>
#include <system_record.h>
#include <cmsis_os2.h>
#include <dashboard/dashboard.h>
#include <gui/gui.h>

static void system_kernel_init() {
    osKernelInitialize();
}
static void system_kernel_start() {
    osKernelStart();
}
void system_init() {
    system_kernel_init();
    system_record_init();
    system_thread_start("gui", gui_thread, NULL, 2048);
    system_thread_start("Init", init_thread, NULL, 2048);
    system_kernel_start();
}

