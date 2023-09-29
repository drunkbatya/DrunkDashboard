#include "system_delay.h"
#include <cmsis_os.h>

void system_delay_ms(uint32_t time) {
    osDelay(time);
}
