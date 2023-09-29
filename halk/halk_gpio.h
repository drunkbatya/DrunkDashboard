#pragma once

#include <stm32h7xx_ll_gpio.h>

typedef struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} GPIOPin;

void halk_gpio_pin_write(const GPIOPin* pin, uint8_t state);
