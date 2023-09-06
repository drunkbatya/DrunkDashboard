#pragma once

#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>

typedef struct {
    GPIO_TypeDef* port;
    uint32_t pin;
} GPIOPin;

void gpio_pin_write(GPIOPin* pin, uint8_t state);
void gpio_spi_transmit_byte(SPI_TypeDef* spi, uint8_t data);
void gpio_spi_transmit(SPI_TypeDef* spi, const uint8_t* data, uint8_t size);
