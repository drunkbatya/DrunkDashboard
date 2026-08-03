#include <furi_hal_encoder.h>
#include <furi_hal_bus.h>
#include <furi_hal_gpio.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>

#include <furi.h>

#include <stm32u5xx_ll_exti.h>
#include <stm32u5xx_ll_gpio.h>
#include <stm32u5xx_ll_lptim.h>
#include <stm32u5xx_ll_rcc.h>
#include <stm32u5xx_ll_tim.h>

#define FURI_HAL_ENCODER_TIM_COUNTS_PER_STEP   2
#define FURI_HAL_ENCODER_LPTIM_COUNTS_PER_STEP 1

typedef struct {
    const uint8_t counts_per_step;
    const bool inverted;
    volatile uint32_t last_counter;
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
} FuriHalEncoderTim;

typedef struct {
    FuriHalEncoderCounter counter;
    LPTIM_TypeDef* const lptim;
    const FuriHalBus bus;
} FuriHalEncoderLpTim;

typedef struct {
    FuriHalEncoderTim vertical;
    FuriHalEncoderLpTim horizontal;
    FuriHalEncoderCallback callback;
    void* callback_context;
} FuriHalEncoderState;

static FuriHalEncoderState furi_hal_encoder = {
    .vertical =
        {
            .counter = {.counts_per_step = FURI_HAL_ENCODER_TIM_COUNTS_PER_STEP},
            .timer = TIM8,
            .bus = FuriHalBusTIM8,
            .interrupt = FuriHalInterruptIdTim8Cc,
            .pin_a = &gpio_encoder_1_a,
            .pin_b = &gpio_encoder_1_b,
            .alt_fn = LL_GPIO_AF_3,
        },
    .horizontal =
        {
            .counter =
                {.counts_per_step = FURI_HAL_ENCODER_LPTIM_COUNTS_PER_STEP, .inverted = true},
            .lptim = LPTIM2,
            .bus = FuriHalBusLPTIM2,
        },
};

static void furi_hal_encoder_apply_counter(FuriHalEncoderCounter* counter, uint32_t value) {
    const int32_t delta = (int16_t)((uint16_t)value - (uint16_t)counter->last_counter);
    counter->last_counter = value;

    if(delta == 0) {
        return;
    }

    const int32_t previous_steps = counter->steps;

    counter->accumulator += counter->inverted ? -delta : delta;

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

static void furi_hal_encoder_tim_isr(void* context) {
    FuriHalEncoderTim* encoder = context;
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

    furi_hal_encoder_apply_counter(&encoder->counter, LL_TIM_GetCounter(timer));
}

static void furi_hal_encoder_tim_init(FuriHalEncoderTim* encoder) {
    TIM_TypeDef* timer = encoder->timer;

    furi_hal_bus_enable(encoder->bus);

    LL_GPIO_InitTypeDef gpio = {0};
    gpio.Pin = encoder->pin_a->pin | encoder->pin_b->pin;
    gpio.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull = LL_GPIO_PULL_NO;
    gpio.Alternate = encoder->alt_fn;
    LL_GPIO_Init(encoder->pin_a->port, &gpio);

    LL_TIM_InitTypeDef tim = {0};
    tim.Prescaler = 0;
    tim.CounterMode = LL_TIM_COUNTERMODE_UP;
    tim.Autoreload = 0xFFFFU;
    tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(timer, &tim);
    LL_TIM_DisableARRPreload(timer);

    LL_TIM_SetEncoderMode(timer, LL_TIM_ENCODERMODE_X4_TI12);
    LL_TIM_IC_SetActiveInput(timer, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(timer, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(timer, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV32_N8);
    LL_TIM_IC_SetPolarity(timer, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetActiveInput(timer, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(timer, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(timer, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV32_N8);
    LL_TIM_IC_SetPolarity(timer, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_SetTriggerOutput(timer, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(timer);
    LL_TIM_CC_EnableChannel(timer, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);

    LL_TIM_SetCounter(timer, 0);
    encoder->counter.last_counter = 0;
    encoder->counter.accumulator = 0;
    encoder->counter.steps = 0;

    LL_TIM_ClearFlag_CC1(timer);
    LL_TIM_ClearFlag_CC2(timer);
    LL_TIM_ClearFlag_CC1OVR(timer);
    LL_TIM_ClearFlag_CC2OVR(timer);
    LL_TIM_EnableIT_CC1(timer);
    LL_TIM_EnableIT_CC2(timer);
    LL_TIM_EnableCounter(timer);

    furi_hal_interrupt_set_isr(encoder->interrupt, furi_hal_encoder_tim_isr, encoder);
}

static uint32_t furi_hal_encoder_lptim_counter(LPTIM_TypeDef* lptim) {
    uint32_t previous = LL_LPTIM_GetCounter(lptim);
    uint32_t current = LL_LPTIM_GetCounter(lptim);

    while(previous != current) {
        previous = current;
        current = LL_LPTIM_GetCounter(lptim);
    }

    return current;
}

static void furi_hal_encoder_lptim_isr(void* context) {
    FuriHalEncoderLpTim* encoder = context;
    furi_hal_encoder_apply_counter(
        &encoder->counter, furi_hal_encoder_lptim_counter(encoder->lptim));
}

static void furi_hal_encoder_lptim_pin_init(
    const GpioPin* gpio,
    uint32_t alt_fn,
    uint32_t exti_port,
    uint32_t exti_source_line,
    uint32_t exti_line,
    FuriHalEncoderLpTim* encoder) {
    LL_GPIO_InitTypeDef init = {0};
    init.Pin = gpio->pin;
    init.Mode = LL_GPIO_MODE_ALTERNATE;
    init.Speed = LL_GPIO_SPEED_FREQ_LOW;
    init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    init.Pull = LL_GPIO_PULL_NO;
    init.Alternate = alt_fn;
    LL_GPIO_Init(gpio->port, &init);

    LL_EXTI_SetEXTISource(exti_port, exti_source_line);
    LL_EXTI_EnableRisingTrig_0_31(exti_line);
    LL_EXTI_EnableFallingTrig_0_31(exti_line);

    furi_hal_gpio_add_int_callback(gpio, furi_hal_encoder_lptim_isr, encoder);

    const IRQn_Type irqn = (IRQn_Type)(EXTI0_IRQn + __builtin_ctz(gpio->pin));
    NVIC_SetPriority(irqn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(irqn);
}

static void furi_hal_encoder_lptim_init(FuriHalEncoderLpTim* encoder) {
    LPTIM_TypeDef* lptim = encoder->lptim;

    LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM2_CLKSOURCE_PCLK1);
    furi_hal_bus_enable(encoder->bus);

    LL_LPTIM_Disable(lptim);
    LL_LPTIM_SetClockSource(lptim, LL_LPTIM_CLK_SOURCE_INTERNAL);
    LL_LPTIM_SetPrescaler(lptim, LL_LPTIM_PRESCALER_DIV1);
    LL_LPTIM_SetEncoderMode(lptim, LL_LPTIM_ENCODER_MODE_RISING_FALLING);
    LL_LPTIM_EnableEncoderMode(lptim);

    LL_LPTIM_Enable(lptim);
    LL_LPTIM_SetAutoReload(lptim, 0xFFFFU);
    while(!LL_LPTIM_IsActiveFlag_ARROK(lptim)) {
    }
    LL_LPTIM_ClearFlag_ARROK(lptim);

    LL_LPTIM_StartCounter(lptim, LL_LPTIM_OPERATING_MODE_CONTINUOUS);

    encoder->counter.last_counter = furi_hal_encoder_lptim_counter(lptim);
    encoder->counter.accumulator = 0;
    encoder->counter.steps = 0;

    furi_hal_encoder_lptim_pin_init(
        &gpio_encoder_2_a,
        LL_GPIO_AF_14,
        LL_EXTI_EXTI_PORTB,
        LL_EXTI_EXTI_LINE1,
        LL_EXTI_LINE_1,
        encoder);
    furi_hal_encoder_lptim_pin_init(
        &gpio_encoder_2_b,
        LL_GPIO_AF_2,
        LL_EXTI_EXTI_PORTD,
        LL_EXTI_EXTI_LINE9,
        LL_EXTI_LINE_9,
        encoder);
}

void furi_hal_encoder_init(void) {
    furi_hal_encoder_tim_init(&furi_hal_encoder.vertical);
    furi_hal_encoder_lptim_init(&furi_hal_encoder.horizontal);
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
