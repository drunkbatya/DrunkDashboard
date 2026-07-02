#include <furi_hal_spi.h>
#include <furi.h>

#include <stm32u5xx_ll_spi.h>

void furi_hal_spi_dma_init(void) {
}

void furi_hal_spi_bus_init(FuriHalSpiBus* bus) {
    furi_check(bus);
    if(bus->callback) {
        bus->callback(bus, FuriHalSpiBusEventInit);
    }
}

void furi_hal_spi_bus_deinit(FuriHalSpiBus* bus) {
    furi_check(bus);
    if(bus->callback) {
        bus->callback(bus, FuriHalSpiBusEventDeinit);
    }
}

void furi_hal_spi_bus_handle_init(const FuriHalSpiBusHandle* handle) {
    furi_check(handle);
    if(handle->callback) {
        handle->callback(handle, FuriHalSpiBusHandleEventInit);
    }
}

void furi_hal_spi_bus_handle_deinit(const FuriHalSpiBusHandle* handle) {
    furi_check(handle);
    if(handle->callback) {
        handle->callback(handle, FuriHalSpiBusHandleEventDeinit);
    }
}

void furi_hal_spi_acquire(const FuriHalSpiBusHandle* handle) {
    furi_check(handle);
    furi_check(handle->bus);

    if(handle->bus->callback) {
        handle->bus->callback(handle->bus, FuriHalSpiBusEventLock);
        handle->bus->callback(handle->bus, FuriHalSpiBusEventActivate);
    }

    furi_check(handle->bus->current_handle == NULL);
    handle->bus->current_handle = handle;

    if(handle->callback) {
        handle->callback(handle, FuriHalSpiBusHandleEventActivate);
    }
}

void furi_hal_spi_release(const FuriHalSpiBusHandle* handle) {
    furi_check(handle);
    furi_check(handle->bus);
    furi_check(handle->bus->current_handle == handle);

    if(handle->callback) {
        handle->callback(handle, FuriHalSpiBusHandleEventDeactivate);
    }

    handle->bus->current_handle = NULL;

    if(handle->bus->callback) {
        handle->bus->callback(handle->bus, FuriHalSpiBusEventDeactivate);
        handle->bus->callback(handle->bus, FuriHalSpiBusEventUnlock);
    }
}

bool furi_hal_spi_bus_rx(
    const FuriHalSpiBusHandle* handle,
    uint8_t* buffer,
    size_t size,
    uint32_t timeout) {
    return furi_hal_spi_bus_trx(handle, NULL, buffer, size, timeout);
}

bool furi_hal_spi_bus_tx(
    const FuriHalSpiBusHandle* handle,
    const uint8_t* buffer,
    size_t size,
    uint32_t timeout) {
    return furi_hal_spi_bus_trx(handle, buffer, NULL, size, timeout);
}

bool furi_hal_spi_bus_trx(
    const FuriHalSpiBusHandle* handle,
    const uint8_t* tx_buffer,
    uint8_t* rx_buffer,
    size_t size,
    uint32_t timeout) {
    UNUSED(timeout);
    furi_check(handle);
    furi_check(handle->bus);
    furi_check(handle->bus->current_handle == handle);
    furi_check(size > 0);

    SPI_TypeDef* spi = handle->bus->spi;
    furi_check(spi);

    while(size > 0) {
        while(!LL_SPI_IsActiveFlag_TXP(spi)) {
        }

        LL_SPI_TransmitData8(spi, tx_buffer ? *tx_buffer++ : 0xFF);

        while(!LL_SPI_IsActiveFlag_RXP(spi)) {
        }

        uint8_t data = LL_SPI_ReceiveData8(spi);
        if(rx_buffer) {
            *rx_buffer++ = data;
        }

        size--;
    }

    while(!LL_SPI_IsActiveFlag_TXC(spi)) {
    }

    return true;
}

bool furi_hal_spi_bus_trx_dma(
    const FuriHalSpiBusHandle* handle,
    uint8_t* tx_buffer,
    uint8_t* rx_buffer,
    size_t size,
    uint32_t timeout_ms) {
    return furi_hal_spi_bus_trx(handle, tx_buffer, rx_buffer, size, timeout_ms);
}
