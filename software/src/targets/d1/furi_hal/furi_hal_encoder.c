#include <furi_hal_encoder.h>
#include <furi_hal_bus.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>

#include <furi.h>

#include <stm32u5xx_ll_gpio.h>
#include <stm32u5xx_ll_tim.h>

#define FURI_HAL_ENCODER_COUNTS_PER_STEP 2

typedef struct {
    TIM_TypeDef* timer;
    FuriHalBus bus;
    FuriHalInterruptId interrupt;
    const GpioPin* pin_a;
    const GpioPin* pin_b;
    bool counter_16bit;
    volatile uint32_t last_counter;
    volatile int32_t accumulator;
    volatile int32_t steps;
} FuriHalEncoder;

typedef struct {
    FuriHalEncoder vertical;
    FuriHalEncoder horizontal;
    FuriHalEncoderCallback callback;
    void* callback_context;
} FuriHalEncoderState;

static FuriHalEncoderState furi_hal_encoder = {
    .vertical =
        {
            .timer = TIM5,
            .bus = FuriHalBusTIM5,
            .interrupt = FuriHalInterruptIdTIM5,
            .pin_a = &gpio_encoder_1_a,
            .pin_b = &gpio_encoder_1_b,
            .counter_16bit = false,
        },
    .horizontal =
        {
            .timer = TIM3,
            .bus = FuriHalBusTIM3,
            .interrupt = FuriHalInterruptIdTIM3,
            .pin_a = &gpio_encoder_2_a,
            .pin_b = &gpio_encoder_2_b,
            .counter_16bit = true,
        },
};

static int32_t furi_hal_encoder_counter_delta(FuriHalEncoder* encoder, uint32_t counter) {
    if(encoder->counter_16bit) {
        return (int16_t)((uint16_t)counter - (uint16_t)encoder->last_counter);
    } else {
        return (int32_t)(counter - encoder->last_counter);
    }
}

static void furi_hal_encoder_apply_delta(FuriHalEncoder* encoder, int32_t delta) {
    encoder->accumulator += delta;

    while(encoder->accumulator >= FURI_HAL_ENCODER_COUNTS_PER_STEP) {
        encoder->steps++;
        encoder->accumulator -= FURI_HAL_ENCODER_COUNTS_PER_STEP;
    }

    while(encoder->accumulator <= -FURI_HAL_ENCODER_COUNTS_PER_STEP) {
        encoder->steps--;
        encoder->accumulator += FURI_HAL_ENCODER_COUNTS_PER_STEP;
    }
}

static void furi_hal_encoder_timer_isr(void* context) {
    FuriHalEncoder* encoder = context;
    TIM_TypeDef* timer = encoder->timer;

    const bool active = LL_TIM_IsActiveFlag_CC1(timer) || LL_TIM_IsActiveFlag_CC2(timer) ||
                        LL_TIM_IsActiveFlag_CC1OVR(timer) || LL_TIM_IsActiveFlag_CC2OVR(timer);

    if(LL_TIM_IsActiveFlag_CC1(timer)) {
        LL_TIM_ClearFlag_CC1(timer);
    }
    if(LL_TIM_IsActiveFlag_CC2(timer)) {
        LL_TIM_ClearFlag_CC2(timer);
    }
    if(LL_TIM_IsActiveFlag_CC1OVR(timer)) {
        LL_TIM_ClearFlag_CC1OVR(timer);
    }
    if(LL_TIM_IsActiveFlag_CC2OVR(timer)) {
        LL_TIM_ClearFlag_CC2OVR(timer);
    }

    if(!active) {
        return;
    }

    const int32_t previous_steps = encoder->steps;
    const uint32_t counter = LL_TIM_GetCounter(timer);
    const int32_t delta = furi_hal_encoder_counter_delta(encoder, counter);
    encoder->last_counter = counter;

    furi_hal_encoder_apply_delta(encoder, delta);

    if((encoder->steps != previous_steps) && furi_hal_encoder.callback) {
        furi_hal_encoder.callback(furi_hal_encoder.callback_context);
    }
}

static void furi_hal_encoder_gpio_init(FuriHalEncoder* encoder) {
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = encoder->pin_a->pin | encoder->pin_b->pin;
    gpio.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;
    gpio.Alternate = LL_GPIO_AF_2;
    LL_GPIO_Init(encoder->pin_a->port, &gpio);
}

static void furi_hal_encoder_timer_init(FuriHalEncoder* encoder) {
    TIM_TypeDef* timer = encoder->timer;

    furi_hal_bus_enable(FuriHalBusGPIOA);
    furi_hal_bus_enable(encoder->bus);
    furi_hal_encoder_gpio_init(encoder);

    LL_TIM_InitTypeDef tim = {0};
    tim.Prescaler = 0;
    tim.CounterMode = LL_TIM_COUNTERMODE_UP;
    tim.Autoreload = encoder->counter_16bit ? 0xFFFFU : 0xFFFFFFFFU;
    tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(timer, &tim);
    LL_TIM_DisableARRPreload(timer);

    LL_TIM_SetEncoderMode(timer, LL_TIM_ENCODERMODE_X2_TI1);
    LL_TIM_IC_SetActiveInput(timer, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(timer, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(timer, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(timer, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetActiveInput(timer, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(timer, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(timer, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(timer, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_SetTriggerOutput(timer, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(timer);
    LL_TIM_CC_EnableChannel(timer, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);

    LL_TIM_SetCounter(timer, 0);
    encoder->last_counter = 0;
    encoder->accumulator = 0;
    encoder->steps = 0;

    LL_TIM_ClearFlag_CC1(timer);
    LL_TIM_ClearFlag_CC2(timer);
    LL_TIM_ClearFlag_CC1OVR(timer);
    LL_TIM_ClearFlag_CC2OVR(timer);
    LL_TIM_EnableIT_CC1(timer);
    LL_TIM_EnableIT_CC2(timer);
    LL_TIM_EnableCounter(timer);

    furi_hal_interrupt_set_isr(encoder->interrupt, furi_hal_encoder_timer_isr, encoder);
}

void furi_hal_encoder_init(void) {
    furi_hal_encoder_timer_init(&furi_hal_encoder.vertical);
    furi_hal_encoder_timer_init(&furi_hal_encoder.horizontal);
}

void furi_hal_encoder_set_callback(FuriHalEncoderCallback callback, void* context) {
    bool pending;

    FURI_CRITICAL_ENTER();
    furi_hal_encoder.callback = callback;
    furi_hal_encoder.callback_context = context;
    pending = furi_hal_encoder.vertical.steps || furi_hal_encoder.horizontal.steps;
    FURI_CRITICAL_EXIT();

    if(pending && callback) {
        callback(context);
    }
}

void furi_hal_encoder_get_deltas(int32_t* vertical, int32_t* horizontal) {
    furi_check(vertical);
    furi_check(horizontal);

    FURI_CRITICAL_ENTER();
    *vertical = furi_hal_encoder.vertical.steps;
    *horizontal = furi_hal_encoder.horizontal.steps;
    furi_hal_encoder.vertical.steps = 0;
    furi_hal_encoder.horizontal.steps = 0;
    FURI_CRITICAL_EXIT();
}
