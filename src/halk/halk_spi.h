#pragma once

#include <stm32h7xx_ll_spi.h>

void halk_spi_transmit(SPI_TypeDef* spi, uint8_t* data, uint16_t size);
