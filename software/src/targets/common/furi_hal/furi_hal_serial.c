#include <furi_hal_serial.h>
#include "furi_hal_serial_types_i.h"

#include <stm32u5xx_ll_rcc.h>
#include <stm32u5xx_ll_usart.h>

#include <furi_hal_bus.h>
#include <furi_hal_gpio.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>

#include <furi.h>

#define FURI_HAL_SERIAL_USART_OVERSAMPLING LL_USART_OVERSAMPLING_16

typedef struct {
    bool enabled;
    FuriHalSerialHandle* handle;
    FuriHalSerialAsyncRxCallback rx_byte_callback;
    void* context;
} FuriHalSerial;

typedef void (*FuriHalSerialControlFunc)(USART_TypeDef*);

typedef struct {
    USART_TypeDef* periph;
    GpioAltFn alt_fn;
    const GpioPin* gpio[FuriHalSerialDirectionMax];
    FuriHalSerialControlFunc enable[FuriHalSerialDirectionMax];
    FuriHalSerialControlFunc disable[FuriHalSerialDirectionMax];
} FuriHalSerialConfig;

static const FuriHalSerialConfig furi_hal_serial_config[FuriHalSerialIdMax] = {
    [FuriHalSerialIdUsart] =
        {
            .periph = USART1,
            .alt_fn = GpioAltFn7USART1,
            .gpio =
                {
                    [FuriHalSerialDirectionTx] = &gpio_usart_tx,
                    [FuriHalSerialDirectionRx] = &gpio_usart_rx,
                },
            .enable =
                {
                    [FuriHalSerialDirectionTx] = LL_USART_EnableDirectionTx,
                    [FuriHalSerialDirectionRx] = LL_USART_EnableDirectionRx,
                },
            .disable =
                {
                    [FuriHalSerialDirectionTx] = LL_USART_DisableDirectionTx,
                    [FuriHalSerialDirectionRx] = LL_USART_DisableDirectionRx,
                },
        },
};

static FuriHalSerial furi_hal_serial[FuriHalSerialIdMax] = {0};

static void furi_hal_serial_usart_irq_callback(void* context) {
    UNUSED(context);

    FuriHalSerialRxEvent event = 0;

    if(USART1->ISR & USART_ISR_RXNE_RXFNE) {
        event |= FuriHalSerialRxEventData;
    }
    if(USART1->ISR & USART_ISR_IDLE) {
        USART1->ICR = USART_ICR_IDLECF;
        event |= FuriHalSerialRxEventIdle;
    }
    if(USART1->ISR & USART_ISR_ORE) {
        USART1->ICR = USART_ICR_ORECF;
        event |= FuriHalSerialRxEventOverrunError;
    }
    if(USART1->ISR & USART_ISR_NE) {
        USART1->ICR = USART_ICR_NECF;
        event |= FuriHalSerialRxEventNoiseError;
    }
    if(USART1->ISR & USART_ISR_FE) {
        USART1->ICR = USART_ICR_FECF;
        event |= FuriHalSerialRxEventFrameError;
    }
    if(USART1->ISR & USART_ISR_PE) {
        USART1->ICR = USART_ICR_PECF;
        event |= FuriHalSerialRxEventParityError;
    }

    if(furi_hal_serial[FuriHalSerialIdUsart].rx_byte_callback) {
        furi_hal_serial[FuriHalSerialIdUsart].rx_byte_callback(
            furi_hal_serial[FuriHalSerialIdUsart].handle,
            event,
            furi_hal_serial[FuriHalSerialIdUsart].context);
    }
}

static void furi_hal_serial_usart_pins_init(void) {
    furi_hal_gpio_init_ex(
        &gpio_usart_tx,
        GpioModeAltFunctionPushPull,
        GpioPullUp,
        GpioSpeedVeryHigh,
        GpioAltFn7USART1);
    furi_hal_gpio_init_ex(
        &gpio_usart_rx,
        GpioModeAltFunctionPushPull,
        GpioPullUp,
        GpioSpeedVeryHigh,
        GpioAltFn7USART1);
}

void furi_hal_serial_init(FuriHalSerialHandle* handle, uint32_t baud) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);

    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1) {
    }

    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_HSI);
    furi_hal_bus_enable(FuriHalBusUSART1);
    furi_hal_serial_usart_pins_init();

    LL_USART_InitTypeDef usart = {0};
    usart.PrescalerValue = LL_USART_PRESCALER_DIV1;
    usart.BaudRate = baud;
    usart.DataWidth = LL_USART_DATAWIDTH_8B;
    usart.StopBits = LL_USART_STOPBITS_1;
    usart.Parity = LL_USART_PARITY_NONE;
    usart.TransferDirection = LL_USART_DIRECTION_TX_RX;
    usart.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usart.OverSampling = FURI_HAL_SERIAL_USART_OVERSAMPLING;
    LL_USART_Init(USART1, &usart);
    LL_USART_EnableFIFO(USART1);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_DisableIT_ERROR(USART1);

    LL_USART_Enable(USART1);
    while(!LL_USART_IsActiveFlag_TEACK(USART1) || !LL_USART_IsActiveFlag_REACK(USART1)) {
    }

    furi_hal_serial[handle->id].enabled = true;
}

void furi_hal_serial_deinit(FuriHalSerialHandle* handle) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);

    furi_hal_serial_async_rx_stop(handle);
    if(LL_USART_IsEnabled(USART1)) {
        LL_USART_Disable(USART1);
    }
    if(furi_hal_bus_is_enabled(FuriHalBusUSART1)) {
        furi_hal_bus_disable(FuriHalBusUSART1);
    }
    furi_hal_gpio_init(&gpio_usart_tx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_usart_rx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_serial[handle->id].enabled = false;
}

void furi_hal_serial_suspend(FuriHalSerialHandle* handle) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);

    if(LL_USART_IsEnabled(USART1)) {
        LL_USART_Disable(USART1);
    }
    furi_hal_serial[handle->id].enabled = false;
}

void furi_hal_serial_resume(FuriHalSerialHandle* handle) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);

    if(!furi_hal_serial[handle->id].enabled) {
        LL_USART_Enable(USART1);
        furi_hal_serial[handle->id].enabled = true;
    }
}

bool furi_hal_serial_is_baud_rate_supported(FuriHalSerialHandle* handle, uint32_t baud) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);
    return baud >= 9600UL && baud <= 4000000UL;
}

void furi_hal_serial_set_br(FuriHalSerialHandle* handle, uint32_t baud) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);

    if(LL_USART_IsEnabled(USART1)) {
        while(!LL_USART_IsActiveFlag_TC(USART1)) {
        }
        LL_USART_Disable(USART1);
        LL_USART_SetBaudRate(
            USART1,
            LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE),
            LL_USART_PRESCALER_DIV1,
            FURI_HAL_SERIAL_USART_OVERSAMPLING,
            baud);
        LL_USART_Enable(USART1);
    }
}

static const uint32_t serial_data_bits_lut[] = {
    [FuriHalSerialDataBits7] = LL_USART_DATAWIDTH_7B,
    [FuriHalSerialDataBits8] = LL_USART_DATAWIDTH_8B,
    [FuriHalSerialDataBits9] = LL_USART_DATAWIDTH_9B,
};

static const uint32_t serial_parity_lut[] = {
    [FuriHalSerialParityNone] = LL_USART_PARITY_NONE,
    [FuriHalSerialParityEven] = LL_USART_PARITY_EVEN,
    [FuriHalSerialParityOdd] = LL_USART_PARITY_ODD,
};

static const uint32_t serial_stop_bits_lut[] = {
    [FuriHalSerialStopBits0_5] = LL_USART_STOPBITS_0_5,
    [FuriHalSerialStopBits1] = LL_USART_STOPBITS_1,
    [FuriHalSerialStopBits1_5] = LL_USART_STOPBITS_1_5,
    [FuriHalSerialStopBits2] = LL_USART_STOPBITS_2,
};

void furi_hal_serial_configure_framing(
    FuriHalSerialHandle* handle,
    FuriHalSerialDataBits data_bits,
    FuriHalSerialParity parity,
    FuriHalSerialStopBits stop_bits) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);
    furi_check(data_bits < FuriHalSerialDataBitsMax);
    furi_check(parity < FuriHalSerialParityMax);
    furi_check(stop_bits < FuriHalSerialStopBits2Max);

    if(data_bits == FuriHalSerialDataBits9) furi_check(parity == FuriHalSerialParityNone);
    if(data_bits == FuriHalSerialDataBits6) furi_check(parity != FuriHalSerialParityNone);
    if(parity != FuriHalSerialParityNone) data_bits++;

    if(LL_USART_IsEnabled(USART1)) {
        while(!LL_USART_IsActiveFlag_TC(USART1)) {
        }
        LL_USART_Disable(USART1);
        LL_USART_SetDataWidth(USART1, serial_data_bits_lut[data_bits]);
        LL_USART_SetParity(USART1, serial_parity_lut[parity]);
        LL_USART_SetStopBitsLength(USART1, serial_stop_bits_lut[stop_bits]);
        LL_USART_Enable(USART1);
    }
}

void furi_hal_serial_tx(FuriHalSerialHandle* handle, const uint8_t* buffer, size_t buffer_size) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);

    if(LL_USART_IsEnabled(USART1) == 0) return;

    while(buffer_size > 0) {
        while(!LL_USART_IsActiveFlag_TXE(USART1)) {
        }
        LL_USART_TransmitData8(USART1, *buffer);
        buffer++;
        buffer_size--;
    }
}

void furi_hal_serial_tx_wait_complete(FuriHalSerialHandle* handle) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);

    if(LL_USART_IsEnabled(USART1) == 0) return;
    while(!LL_USART_IsActiveFlag_TC(USART1)) {
    }
}

static void furi_hal_serial_event_init(FuriHalSerialHandle* handle, bool report_errors) {
    furi_check(handle->id == FuriHalSerialIdUsart);

    LL_USART_EnableIT_IDLE(USART1);
    if(report_errors) {
        LL_USART_EnableIT_ERROR(USART1);
    }
}

static void furi_hal_serial_event_deinit(FuriHalSerialHandle* handle) {
    furi_check(handle->id == FuriHalSerialIdUsart);

    if(LL_USART_IsEnabledIT_IDLE(USART1)) LL_USART_DisableIT_IDLE(USART1);
    if(LL_USART_IsEnabledIT_ERROR(USART1)) LL_USART_DisableIT_ERROR(USART1);
}

static void furi_hal_serial_async_rx_configure(
    FuriHalSerialHandle* handle,
    FuriHalSerialAsyncRxCallback callback,
    void* context) {
    furi_check(handle->id == FuriHalSerialIdUsart);

    if(!callback) {
        LL_USART_DisableIT_RXNE_RXFNE(USART1);
        furi_hal_interrupt_set_isr(FuriHalInterruptIdUart1, NULL, NULL);
    }

    furi_hal_serial[handle->id].rx_byte_callback = callback;
    furi_hal_serial[handle->id].handle = handle;
    furi_hal_serial[handle->id].context = context;

    if(callback) {
        furi_hal_interrupt_set_isr(
            FuriHalInterruptIdUart1, furi_hal_serial_usart_irq_callback, NULL);
        LL_USART_EnableIT_RXNE_RXFNE(USART1);
    }
}

void furi_hal_serial_async_rx_start(
    FuriHalSerialHandle* handle,
    FuriHalSerialAsyncRxCallback callback,
    void* context,
    bool report_errors) {
    furi_check(handle);
    furi_check(callback);

    furi_hal_serial_event_init(handle, report_errors);
    furi_hal_serial_async_rx_configure(handle, callback, context);
}

void furi_hal_serial_async_rx_stop(FuriHalSerialHandle* handle) {
    furi_check(handle);
    furi_hal_serial_event_deinit(handle);
    furi_hal_serial_async_rx_configure(handle, NULL, NULL);
}

bool furi_hal_serial_async_rx_available(FuriHalSerialHandle* handle) {
    furi_check(FURI_IS_IRQ_MODE());
    furi_check(handle->id == FuriHalSerialIdUsart);
    return LL_USART_IsActiveFlag_RXNE_RXFNE(USART1);
}

uint8_t furi_hal_serial_async_rx(FuriHalSerialHandle* handle) {
    furi_check(FURI_IS_IRQ_MODE());
    furi_check(handle->id == FuriHalSerialIdUsart);
    return LL_USART_ReceiveData8(USART1);
}

void furi_hal_serial_enable_direction(
    FuriHalSerialHandle* handle,
    FuriHalSerialDirection direction) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);
    furi_check(direction < FuriHalSerialDirectionMax);

    USART_TypeDef* periph = furi_hal_serial_config[handle->id].periph;
    furi_hal_serial_config[handle->id].enable[direction](periph);

    const GpioPin* gpio = furi_hal_serial_config[handle->id].gpio[direction];
    const GpioAltFn alt_fn = furi_hal_serial_config[handle->id].alt_fn;

    furi_hal_gpio_init_ex(
        gpio, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedVeryHigh, alt_fn);
}

void furi_hal_serial_disable_direction(
    FuriHalSerialHandle* handle,
    FuriHalSerialDirection direction) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);
    furi_check(direction < FuriHalSerialDirectionMax);

    USART_TypeDef* periph = furi_hal_serial_config[handle->id].periph;
    furi_hal_serial_config[handle->id].disable[direction](periph);

    const GpioPin* gpio = furi_hal_serial_config[handle->id].gpio[direction];

    furi_hal_gpio_init(gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

const GpioPin*
    furi_hal_serial_get_gpio_pin(FuriHalSerialHandle* handle, FuriHalSerialDirection direction) {
    furi_check(handle);
    furi_check(handle->id == FuriHalSerialIdUsart);
    furi_check(direction < FuriHalSerialDirectionMax);

    return furi_hal_serial_config[handle->id].gpio[direction];
}
