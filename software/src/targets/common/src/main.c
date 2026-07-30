#include <furi.h>
#include <furi_hal.h>
#include <drunk_dashboard.h>

#define TAG "Main"

int32_t init_task(void* context) {
    UNUSED(context);

    // DrunkDashboard FURI HAL
    furi_hal_init();

    // Init DrunkDashboard
    drunk_dashboard_init();

    furi_background();

    return 0;
}

int main(void) {
    // Initialize FURI layer
    furi_init();

    // DrunkDashboard critical FURI HAL
    furi_hal_init_early();

    FuriThread* main_thread = furi_thread_alloc_ex("InitSrv", 1024, init_task, NULL);
    furi_thread_set_priority(main_thread, FuriThreadPriorityInit);

#ifdef FURI_RAM_EXEC
    // Prevent entering sleep mode when executed from RAM
    furi_hal_power_insomnia_enter();
    furi_thread_start(main_thread);
#else
    furi_thread_start(main_thread);
#endif

    // Run Kernel
    furi_run();

    furi_crash("Kernel is Dead");
}

void Error_Handler(void) {
    furi_crash("ErrorHandler");
}

void abort(void) {
    furi_crash("AbortHandler");
}
