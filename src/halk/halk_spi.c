#include "halk_spi.h"

static void __halk_spi_await_enough_fifo_space__(SPI_TypeDef *spi) {
    while(!(LL_SPI_IsActiveFlag_TXP(spi)))
        ;
}

static void __halk_spi_await_end_of_transmission__(SPI_TypeDef *spi) {
    while(!(LL_SPI_IsActiveFlag_EOT(spi)))
        ;
}

static void __halk_spi_disable_all_interrupts__(SPI_TypeDef *spi) {
    LL_SPI_DisableIT_EOT(spi);
    LL_SPI_DisableIT_TXP(spi);
    LL_SPI_DisableIT_RXP(spi);
    LL_SPI_DisableIT_DXP(spi);
    LL_SPI_DisableIT_UDR(spi);
    LL_SPI_DisableIT_OVR(spi);
    LL_SPI_DisableIT_FRE(spi);
    LL_SPI_DisableIT_MODF(spi);
}

void halk_spi_transmit(SPI_TypeDef *spi, uint8_t *data, uint32_t size) {
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
    __halk_spi_disable_all_interrupts__(spi);
}
