#include <furi_hal_i2c_config.h>
#include <furi_hal_bus.h>
#include <furi_hal_resources.h>

#include <stm32u5xx_ll_i2c.h>
#include <stm32u5xx_ll_rcc.h>

#include <furi.h>

#define FURI_HAL_I2C1_TIMING 0x30909DEC

static FuriMutex* furi_hal_i2c1_mutex = NULL;

static void furi_hal_i2c_bus_i2c1_event(FuriHalI2cBus* bus, FuriHalI2cBusEvent event) {
    if(event == FuriHalI2cBusEventInit) {
        furi_hal_i2c1_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
        bus->current_handle = NULL;
    } else if(event == FuriHalI2cBusEventDeinit) {
        furi_mutex_free(furi_hal_i2c1_mutex);
        furi_hal_i2c1_mutex = NULL;
    } else if(event == FuriHalI2cBusEventLock) {
        furi_check(furi_mutex_acquire(furi_hal_i2c1_mutex, FuriWaitForever) == FuriStatusOk);
    } else if(event == FuriHalI2cBusEventUnlock) {
        furi_check(furi_mutex_release(furi_hal_i2c1_mutex) == FuriStatusOk);
    } else if(event == FuriHalI2cBusEventActivate) {
        FURI_CRITICAL_ENTER();
        LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);
        furi_hal_bus_enable(FuriHalBusI2C1);
        FURI_CRITICAL_EXIT();
    } else if(event == FuriHalI2cBusEventDeactivate) {
        furi_hal_bus_disable(FuriHalBusI2C1);
    }
}

FuriHalI2cBus furi_hal_i2c_bus_i2c1 = {
    .i2c = I2C1,
    .callback = furi_hal_i2c_bus_i2c1_event,
};

static void furi_hal_i2c_bus_handle_i2c1_event(
    const FuriHalI2cBusHandle* handle,
    FuriHalI2cBusHandleEvent event) {
    if(event == FuriHalI2cBusHandleEventActivate) {
        furi_hal_gpio_init_ex(
            &gpio_i2c1_scl, GpioModeAltFunctionOpenDrain, GpioPullNo, GpioSpeedLow, GpioAltFn4I2C1);
        furi_hal_gpio_init_ex(
            &gpio_i2c1_sda, GpioModeAltFunctionOpenDrain, GpioPullNo, GpioSpeedLow, GpioAltFn4I2C1);

        LL_I2C_InitTypeDef i2c = {0};
        i2c.PeripheralMode = LL_I2C_MODE_I2C;
        i2c.Timing = FURI_HAL_I2C1_TIMING;
        i2c.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
        i2c.DigitalFilter = 0;
        i2c.OwnAddress1 = 0;
        i2c.TypeAcknowledge = LL_I2C_ACK;
        i2c.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
        LL_I2C_Init(handle->bus->i2c, &i2c);
        LL_I2C_EnableAutoEndMode(handle->bus->i2c);
        LL_I2C_SetOwnAddress2(handle->bus->i2c, 0, LL_I2C_OWNADDRESS2_NOMASK);
        LL_I2C_DisableOwnAddress2(handle->bus->i2c);
        LL_I2C_DisableGeneralCall(handle->bus->i2c);
        LL_I2C_EnableClockStretching(handle->bus->i2c);
    } else if(event == FuriHalI2cBusHandleEventDeactivate) {
        LL_I2C_Disable(handle->bus->i2c);
        furi_hal_gpio_init(&gpio_i2c1_scl, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_i2c1_sda, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

const FuriHalI2cBusHandle furi_hal_i2c_handle_i2c1 = {
    .bus = &furi_hal_i2c_bus_i2c1,
    .callback = furi_hal_i2c_bus_handle_i2c1_event,
};
