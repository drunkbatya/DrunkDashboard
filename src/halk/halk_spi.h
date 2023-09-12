#pragma once

#include <stm32h7xx_ll_spi.h>

void halk_spi_transmit(SPI_TypeDef *spi, uint8_t *data, uint32_t size);
