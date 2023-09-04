#pragma once

#include <stm32f4xx_ll_gpio.h>

typedef struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} GPIOPin;

void gpio_write(GPIOPin* pin, uint8_t state);
