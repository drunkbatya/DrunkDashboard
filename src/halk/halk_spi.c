#include "halk_spi.h"

void halk_spi_init(SPI_TypeDef* spi) {
    LL_SPI_EnableGPIOControl(spi);
}

static void __halk_spi_await_enough_fifo_space__(SPI_TypeDef* spi) {
    while(!(LL_SPI_IsActiveFlag_TXP(spi)))
        ;
}

static void __halk_spi_await_end_of_transmission__(SPI_TypeDef* spi) {
    while(!(LL_SPI_IsActiveFlag_EOT(spi)))
        ;
}

void halk_spi_transmit(SPI_TypeDef* spi, uint8_t* data, uint16_t size) {
    LL_SPI_SetTransferSize(spi, size);
    LL_SPI_Enable(spi);
    LL_SPI_StartMasterTransfer(spi);
    while(size) {
        __halk_spi_await_enough_fifo_space__(spi);
        LL_SPI_TransmitData8(spi, *data);
        data++;
        size--;
    }
    __halk_spi_await_end_of_transmission__(spi);
    LL_SPI_ClearFlag_EOT(spi);
    LL_SPI_ClearFlag_TXTF(spi);
    LL_SPI_Disable(spi);
}
