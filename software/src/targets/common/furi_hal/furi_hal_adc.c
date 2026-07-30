#include <furi_hal_adc.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <furi.h>

#include <stm32u5xx_ll_adc.h>
#include <stm32u5xx_ll_bus.h>
#include <stm32u5xx_ll_gpio.h>
#include <stm32u5xx_ll_rcc.h>

struct FuriHalAdcHandle {
    ADC_TypeDef* adc;
    FuriMutex* mutex;
    uint32_t resolution;
    uint32_t vref_mv;
};

typedef struct {
    ADC_TypeDef* adc;
    uint32_t channel;
} FuriHalAdcChannelMap;

static FuriHalAdcHandle furi_hal_adc_handle = {0};

static const FuriHalAdcChannelMap furi_hal_adc_channel_map[] = {
    [FuriHalAdcChannel1] = {ADC1, LL_ADC_CHANNEL_1},
    [FuriHalAdcChannel2] = {ADC1, LL_ADC_CHANNEL_2},
    [FuriHalAdcChannel3] = {ADC1, LL_ADC_CHANNEL_3},
    [FuriHalAdcChannel4] = {ADC1, LL_ADC_CHANNEL_4},
    [FuriHalAdcChannel5] = {ADC1, LL_ADC_CHANNEL_5},
    [FuriHalAdcChannel6] = {ADC1, LL_ADC_CHANNEL_6},
    [FuriHalAdcChannel7] = {ADC1, LL_ADC_CHANNEL_7},
    [FuriHalAdcChannel8] = {ADC1, LL_ADC_CHANNEL_8},
    [FuriHalAdcChannel9] = {ADC1, LL_ADC_CHANNEL_9},
};

static void furi_hal_adc_gpio_init(void) {
    for(size_t i = 0; i < gpio_pins_count; i++) {
        if(gpio_pins[i].channel == FuriHalAdcChannelNone) continue;
        furi_hal_gpio_init(gpio_pins[i].pin, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

static void furi_hal_adc1_init(void) {
    LL_RCC_SetADCDACClockSource(LL_RCC_ADCDAC_CLKSOURCE_HSE);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);

    furi_hal_adc_gpio_init();

    LL_ADC_SetTriggerFrequencyMode(ADC1, LL_ADC_TRIGGER_FREQ_HIGH);

    LL_ADC_CommonInitTypeDef adc_common = {0};
    adc_common.CommonClock = LL_ADC_CLOCK_ASYNC_DIV1;
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &adc_common);

    LL_ADC_InitTypeDef adc = {0};
    adc.Resolution = LL_ADC_RESOLUTION_14B;
    adc.LowPowerMode = LL_ADC_LP_MODE_NONE;
    adc.LeftBitShift = LL_ADC_LEFT_BIT_SHIFT_NONE;
    LL_ADC_Init(ADC1, &adc);
    LL_ADC_SetGainCompensation(ADC1, 0);

    LL_ADC_REG_InitTypeDef adc_reg = {0};
    adc_reg.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    adc_reg.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    adc_reg.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    adc_reg.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
    adc_reg.DataTransferMode = LL_ADC_REG_DR_TRANSFER;
    LL_ADC_REG_Init(ADC1, &adc_reg);
    LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

    LL_ADC_DisableDeepPowerDown(ADC1);
    LL_ADC_EnableInternalRegulator(ADC1);

    __IO uint32_t wait_loop_index =
        ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while(wait_loop_index != 0) {
        wait_loop_index--;
    }
}

void furi_hal_adc_init(void) {
    furi_hal_adc1_init();

    furi_hal_adc_handle.adc = ADC1;
    furi_hal_adc_handle.resolution = 14;
    furi_hal_adc_handle.vref_mv = 3300;
    furi_hal_adc_handle.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
}

FuriHalAdcHandle* furi_hal_adc_acquire(void) {
    furi_check(furi_hal_adc_handle.mutex);
    furi_check(furi_mutex_acquire(furi_hal_adc_handle.mutex, FuriWaitForever) == FuriStatusOk);
    return &furi_hal_adc_handle;
}

void furi_hal_adc_release(FuriHalAdcHandle* handle) {
    furi_check(handle == &furi_hal_adc_handle);
    furi_check(furi_mutex_release(furi_hal_adc_handle.mutex) == FuriStatusOk);
}

void furi_hal_adc_configure(FuriHalAdcHandle* handle) {
    furi_check(handle == &furi_hal_adc_handle);
}

void furi_hal_adc_configure_ex(
    FuriHalAdcHandle* handle,
    FuriHalAdcScale scale,
    FuriHalAdcClock clock,
    FuriHalAdcOversample oversample,
    FuriHalAdcSamplingTime sampling_time) {
    UNUSED(clock);
    UNUSED(oversample);
    UNUSED(sampling_time);
    furi_check(handle == &furi_hal_adc_handle);

    switch(scale) {
    case FuriHalAdcScale2048:
        handle->vref_mv = 2048;
        break;
    case FuriHalAdcScale2500:
        handle->vref_mv = 2500;
        break;
    default:
        furi_crash();
    }
}

uint16_t furi_hal_adc_read(FuriHalAdcHandle* handle, FuriHalAdcChannel channel) {
    furi_check(handle == &furi_hal_adc_handle);
    furi_check(channel < COUNT_OF(furi_hal_adc_channel_map));
    furi_check(furi_hal_adc_channel_map[channel].adc == ADC1);

    ADC_TypeDef* adc = handle->adc;
    const uint32_t adc_channel = furi_hal_adc_channel_map[channel].channel;

    if(LL_ADC_IsEnabled(adc) == 0) {
        LL_ADC_Enable(adc);
        while(LL_ADC_IsActiveFlag_ADRDY(adc) == 0) {
        }
    }

    furi_check(LL_ADC_REG_IsConversionOngoing(adc) == 0);
    LL_ADC_REG_SetSequencerRanks(adc, LL_ADC_REG_RANK_1, adc_channel);
    LL_ADC_SetChannelSamplingTime(adc, adc_channel, LL_ADC_SAMPLINGTIME_391CYCLES_5);
    LL_ADC_REG_StartConversion(adc);

    while(LL_ADC_IsActiveFlag_EOC(adc) == 0) {
    }

    return LL_ADC_REG_ReadConversionData14(adc);
}

float furi_hal_adc_convert_to_voltage(FuriHalAdcHandle* handle, uint16_t value) {
    furi_check(handle == &furi_hal_adc_handle);
    const uint32_t max_value = (1UL << handle->resolution) - 1UL;
    return ((float)value * (float)handle->vref_mv) / (float)max_value;
}

float furi_hal_adc_convert_vref(FuriHalAdcHandle* handle, uint16_t value) {
    UNUSED(value);
    furi_check(handle == &furi_hal_adc_handle);
    return (float)handle->vref_mv;
}

float furi_hal_adc_convert_temp(FuriHalAdcHandle* handle, uint16_t value) {
    UNUSED(value);
    furi_check(handle == &furi_hal_adc_handle);
    furi_crash("ADC temp channel is not configured");
    return 0.0f;
}

float furi_hal_adc_convert_vbat(FuriHalAdcHandle* handle, uint16_t value) {
    return furi_hal_adc_convert_to_voltage(handle, value) * 3.0f;
}
