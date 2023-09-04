#include "gpio.h"

void gpio_write(GPIOPin* pin, uint8_t state) {
    if(state)
        LL_GPIO_SetOutputPin(pin->port, pin->pin);
    else
        LL_GPIO_ResetOutputPin(pin->port, pin->pin);
}
