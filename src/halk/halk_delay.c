#include "halk_delay.h"
#include <cmsis_os.h>

void halk_delay_ms(uint32_t time) {
    osDelay(time);
}
