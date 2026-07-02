#include <furi_hal_spi_config.h>
#include <furi_hal_spi.h>
#include <furi_hal_bus.h>
#include <furi.h>

#include <stm32u5xx_ll_bus.h>
#include <stm32u5xx_ll_gpio.h>
#include <stm32u5xx_ll_rcc.h>
#include <stm32u5xx_ll_spi.h>

static FuriMutex* furi_hal_spi2_mutex = NULL;

static void furi_hal_spi2_init(void) {
    LL_RCC_SetSPIClockSource(LL_RCC_SPI2_CLKSOURCE_SYSCLK);
    furi_hal_bus_enable(FuriHalBusSPI2);
    furi_hal_bus_enable(FuriHalBusGPIOB);

    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = LL_GPIO_PIN_10 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    gpio.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;
    gpio.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOB, &gpio);

    LL_SPI_InitTypeDef spi = {0};
    spi.TransferDirection = LL_SPI_FULL_DUPLEX;
    spi.Mode = LL_SPI_MODE_MASTER;
    spi.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    spi.ClockPolarity = LL_SPI_POLARITY_LOW;
    spi.ClockPhase = LL_SPI_PHASE_1EDGE;
    spi.NSS = LL_SPI_NSS_SOFT;
    spi.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16;
    spi.BitOrder = LL_SPI_MSB_FIRST;
    spi.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    spi.CRCPoly = 0x7;
    LL_SPI_Init(SPI2, &spi);
    LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_EnableNSSPulseMgt(SPI2);
}

static void furi_hal_spi2_bus_callback(FuriHalSpiBus* bus, FuriHalSpiBusEvent event) {
    UNUSED(bus);

    switch(event) {
    case FuriHalSpiBusEventInit:
        furi_hal_spi2_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
        furi_hal_spi2_init();
        LL_SPI_Enable(SPI2);
        break;
    case FuriHalSpiBusEventDeinit:
        LL_SPI_Disable(SPI2);
        furi_hal_bus_disable(FuriHalBusSPI2);
        furi_mutex_free(furi_hal_spi2_mutex);
        furi_hal_spi2_mutex = NULL;
        break;
    case FuriHalSpiBusEventLock:
        furi_check(furi_mutex_acquire(furi_hal_spi2_mutex, FuriWaitForever) == FuriStatusOk);
        break;
    case FuriHalSpiBusEventUnlock:
        furi_check(furi_mutex_release(furi_hal_spi2_mutex) == FuriStatusOk);
        break;
    case FuriHalSpiBusEventActivate:
    case FuriHalSpiBusEventDeactivate:
        break;
    }
}

FuriHalSpiBus furi_hal_spi_bus_spi2 = {
    .spi = SPI2,
    .callback = furi_hal_spi2_bus_callback,
    .current_handle = NULL,
};

void furi_hal_spi_config_init_early(void) {
}

void furi_hal_spi_config_deinit_early(void) {
}

void furi_hal_spi_config_init(void) {
    furi_hal_spi_bus_init(&furi_hal_spi_bus_spi2);
}
