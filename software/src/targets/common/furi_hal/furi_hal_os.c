#include <furi_hal_os.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <furi.h>

#include <FreeRTOS.h>
#include <task.h>

#define TAG "FuriHalOs"

#ifndef FURI_HAL_OS_DEBUG_AWAKE_GPIO
#define FURI_HAL_OS_DEBUG_AWAKE_GPIO (&gpio_ext_pa7)
#endif

#ifndef FURI_HAL_OS_DEBUG_TICK_GPIO
#define FURI_HAL_OS_DEBUG_TICK_GPIO (&gpio_ext_pa6)
#endif

#ifndef FURI_HAL_OS_DEBUG_SECOND_GPIO
#define FURI_HAL_OS_DEBUG_SECOND_GPIO (&gpio_ext_pa4)
#endif

#ifdef FURI_HAL_OS_DEBUG
#include <stm32u5xx_ll_gpio.h>

void furi_hal_os_timer_callback(void) {
    furi_hal_gpio_write(
        FURI_HAL_OS_DEBUG_SECOND_GPIO, !furi_hal_gpio_read(FURI_HAL_OS_DEBUG_SECOND_GPIO));
}
#endif

extern void xPortSysTickHandler(void);

void furi_hal_os_init(void) {
#ifdef FURI_HAL_OS_DEBUG
    furi_hal_gpio_init_simple(FURI_HAL_OS_DEBUG_AWAKE_GPIO, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(FURI_HAL_OS_DEBUG_TICK_GPIO, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(FURI_HAL_OS_DEBUG_SECOND_GPIO, GpioModeOutputPushPull);
    furi_hal_gpio_write(FURI_HAL_OS_DEBUG_AWAKE_GPIO, 1);

    FuriTimer* second_timer =
        furi_timer_alloc(furi_hal_os_timer_callback, FuriTimerTypePeriodic, NULL);
    furi_timer_start(second_timer, FURI_HAL_OS_TICK_HZ);
#endif

    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_os_tick(void) {
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
#ifdef FURI_HAL_OS_DEBUG
        furi_hal_gpio_write(
            FURI_HAL_OS_DEBUG_TICK_GPIO, !furi_hal_gpio_read(FURI_HAL_OS_DEBUG_TICK_GPIO));
#endif
        xPortSysTickHandler();
    }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    UNUSED(xTask);
    furi_log_puts("\r\n\r\n stack overflow in ");
    furi_log_puts(pcTaskName);
    furi_log_puts("\r\n\r\n");
    furi_crash("StackOverflow");
}
