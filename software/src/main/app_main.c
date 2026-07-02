//#include <kal.h>
//#include <gui/gui.h>

#include <stdbool.h>
#include <stm32u5xx_ll_gpio.h>
#include <stm32u5xx_ll_utils.h>

//#include <applications/dashboard/dashboard_app.h>

#include <FreeRTOS.h>
#include <task.h>

void app_main(void) {
    while(true) {
    }
}

void app_init(void) {
    //furi_check(!furi_kernel_is_irq_or_masked());
    //furi_check(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED);

    //furi_thread_init();
    //furi_log_init();
    //furi_record_init();
}
