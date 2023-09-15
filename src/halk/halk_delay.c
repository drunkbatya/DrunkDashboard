#include "halk_delay.h"
#include <stm32h7xx_ll_utils.h>

void halk_delay_ms(uint32_t time) {
    LL_mDelay(time);
}
