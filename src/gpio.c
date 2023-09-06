#include "gpio.h"

void gpio_pin_write(GPIOPin* pin, uint8_t state) {
    if(state)
        LL_GPIO_SetOutputPin(pin->port, pin->pin);
    else
        LL_GPIO_ResetOutputPin(pin->port, pin->pin);
}

void gpio_spi_transmit_byte(SPI_TypeDef* spi, uint8_t data) {
    while(!(spi->SR & SPI_SR_TXE))
        ;
    LL_SPI_TransmitData8(SPI1, data);
    while(spi->SR & SPI_SR_BSY)
        ;
}

void gpio_spi_transmit(SPI_TypeDef* spi, const uint8_t* data, uint8_t size) {
    while(size) {
        gpio_spi_transmit_byte(spi, *data);
        data++;
        size--;
    }
}
