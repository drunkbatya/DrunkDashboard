#include <furi_hal_encoder.h>
#include <furi_hal_bus.h>
#include <furi_hal_gpio.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>

#include <furi.h>

#include <stm32u5xx_ll_gpio.h>
#include <stm32u5xx_ll_tim.h>

#define FURI_HAL_ENCODER_TIMER_COUNTS_PER_STEP 2
#define FURI_HAL_ENCODER_GPIO_COUNTS_PER_STEP  4

typedef struct {
    const uint8_t counts_per_step;
    volatile int32_t accumulator;
    volatile int32_t steps;
} FuriHalEncoderCounter;

typedef struct {
    FuriHalEncoderCounter counter;
    TIM_TypeDef* const timer;
    const FuriHalBus bus;
    const FuriHalInterruptId interrupt;
    const GpioPin* const pin_a;
    const GpioPin* const pin_b;
    const uint32_t alt_fn;
    volatile uint32_t last_counter;
} FuriHalEncoderTimer;

typedef struct {
    FuriHalEncoderCounter counter;
    const GpioPin* const pin_a;
    const GpioPin* const pin_b;
    volatile uint8_t last_state;
} FuriHalEncoderGpio;

typedef struct {
    FuriHalEncoderTimer vertical;
    FuriHalEncoderGpio horizontal;
    FuriHalEncoderCallback callback;
    void* callback_context;
} FuriHalEncoderState;

static FuriHalEncoderState furi_hal_encoder = {
    .vertical =
        {
            .counter = {.counts_per_step = FURI_HAL_ENCODER_TIMER_COUNTS_PER_STEP},
            .timer = TIM8,
            .bus = FuriHalBusTIM8,
            .interrupt = FuriHalInterruptIdTim8Cc,
            .pin_a = &gpio_encoder_1_a,
            .pin_b = &gpio_encoder_1_b,
            .alt_fn = LL_GPIO_AF_3,
        },
    .horizontal =
        {
            .counter = {.counts_per_step = FURI_HAL_ENCODER_GPIO_COUNTS_PER_STEP},
            .pin_a = &gpio_encoder_2_a,
            .pin_b = &gpio_encoder_2_b,
        },
};

static const int8_t furi_hal_encoder_quadrature_lut[16] =
    {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};

static void furi_hal_encoder_apply_delta(FuriHalEncoderCounter* counter, int32_t delta) {
    const int32_t previous_steps = counter->steps;

    counter->accumulator += delta;

    while(counter->accumulator >= counter->counts_per_step) {
        counter->steps++;
        counter->accumulator -= counter->counts_per_step;
    }

    while(counter->accumulator <= -counter->counts_per_step) {
        counter->steps--;
        counter->accumulator += counter->counts_per_step;
    }

    if((counter->steps != previous_steps) && furi_hal_encoder.callback) {
        furi_hal_encoder.callback(furi_hal_encoder.callback_context);
    }
}

static void furi_hal_encoder_timer_isr(void* context) {
    FuriHalEncoderTimer* encoder = context;
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

    const uint32_t counter = LL_TIM_GetCounter(timer);
    const int32_t delta = (int16_t)((uint16_t)counter - (uint16_t)encoder->last_counter);
    encoder->last_counter = counter;

    furi_hal_encoder_apply_delta(&encoder->counter, delta);
}

static void furi_hal_encoder_timer_gpio_init(FuriHalEncoderTimer* encoder) {
    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = encoder->pin_a->pin | encoder->pin_b->pin;
    gpio.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;
    gpio.Alternate = encoder->alt_fn;
    LL_GPIO_Init(encoder->pin_a->port, &gpio);
}

static void furi_hal_encoder_timer_init(FuriHalEncoderTimer* encoder) {
    TIM_TypeDef* timer = encoder->timer;

    furi_hal_bus_enable(encoder->bus);
    furi_hal_encoder_timer_gpio_init(encoder);

    LL_TIM_InitTypeDef tim = {0};
    tim.Prescaler = 0;
    tim.CounterMode = LL_TIM_COUNTERMODE_UP;
    tim.Autoreload = 0xFFFFU;
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
    encoder->counter.accumulator = 0;
    encoder->counter.steps = 0;

    LL_TIM_ClearFlag_CC1(timer);
    LL_TIM_ClearFlag_CC2(timer);
    LL_TIM_ClearFlag_CC1OVR(timer);
    LL_TIM_ClearFlag_CC2OVR(timer);
    LL_TIM_EnableIT_CC1(timer);
    LL_TIM_EnableIT_CC2(timer);
    LL_TIM_EnableCounter(timer);

    furi_hal_interrupt_set_isr(encoder->interrupt, furi_hal_encoder_timer_isr, encoder);
}

static uint8_t furi_hal_encoder_gpio_state(FuriHalEncoderGpio* encoder) {
    return (furi_hal_gpio_read(encoder->pin_a) ? 2U : 0U) |
           (furi_hal_gpio_read(encoder->pin_b) ? 1U : 0U);
}

static void furi_hal_encoder_gpio_isr(void* context) {
    FuriHalEncoderGpio* encoder = context;

    const uint8_t state = furi_hal_encoder_gpio_state(encoder);
    const int8_t delta = furi_hal_encoder_quadrature_lut[(encoder->last_state << 2) | state];
    encoder->last_state = state;

    if(delta != 0) {
        furi_hal_encoder_apply_delta(&encoder->counter, delta);
    }
}

static void furi_hal_encoder_gpio_init(FuriHalEncoderGpio* encoder) {
    furi_hal_gpio_init(encoder->pin_a, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedLow);
    furi_hal_gpio_init(encoder->pin_b, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedLow);

    encoder->last_state = furi_hal_encoder_gpio_state(encoder);
    encoder->counter.accumulator = 0;
    encoder->counter.steps = 0;

    furi_hal_gpio_add_int_callback(encoder->pin_a, furi_hal_encoder_gpio_isr, encoder);
    furi_hal_gpio_add_int_callback(encoder->pin_b, furi_hal_encoder_gpio_isr, encoder);

    NVIC_SetPriority(EXTI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_SetPriority(EXTI9_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI9_IRQn);
}

void furi_hal_encoder_init(void) {
    furi_hal_encoder_timer_init(&furi_hal_encoder.vertical);
    furi_hal_encoder_gpio_init(&furi_hal_encoder.horizontal);
}

void furi_hal_encoder_set_callback(FuriHalEncoderCallback callback, void* context) {
    bool pending;

    FURI_CRITICAL_ENTER();
    furi_hal_encoder.callback = callback;
    furi_hal_encoder.callback_context = context;
    pending = furi_hal_encoder.vertical.counter.steps || furi_hal_encoder.horizontal.counter.steps;
    FURI_CRITICAL_EXIT();

    if(pending && callback) {
        callback(context);
    }
}

void furi_hal_encoder_get_deltas(int32_t* vertical, int32_t* horizontal) {
    furi_check(vertical);
    furi_check(horizontal);

    FURI_CRITICAL_ENTER();
    *vertical = furi_hal_encoder.vertical.counter.steps;
    *horizontal = furi_hal_encoder.horizontal.counter.steps;
    furi_hal_encoder.vertical.counter.steps = 0;
    furi_hal_encoder.horizontal.counter.steps = 0;
    FURI_CRITICAL_EXIT();
}
