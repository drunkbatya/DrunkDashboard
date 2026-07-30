#include <furi_hal_encoder.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <furi.h>

#define FURI_HAL_ENCODER_COUNTS_PER_STEP 2

typedef struct {
    const GpioPin* const pin_a;
    const GpioPin* const pin_b;
    const bool inverted;
    volatile uint8_t last_state;
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
            .pin_a = &gpio_encoder_1_a,
            .pin_b = &gpio_encoder_1_b,
        },
    .horizontal =
        {
            .pin_a = &gpio_encoder_2_a,
            .pin_b = &gpio_encoder_2_b,
            .inverted = true,
        },
};

static const int8_t furi_hal_encoder_quadrature_lut[16] =
    {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};

static uint8_t furi_hal_encoder_read_state(FuriHalEncoder* encoder) {
    return (furi_hal_gpio_read(encoder->pin_a) ? 2U : 0U) |
           (furi_hal_gpio_read(encoder->pin_b) ? 1U : 0U);
}

static void furi_hal_encoder_isr(void* context) {
    FuriHalEncoder* encoder = context;

    const uint8_t state = furi_hal_encoder_read_state(encoder);
    const int8_t delta = furi_hal_encoder_quadrature_lut[(encoder->last_state << 2) | state];
    encoder->last_state = state;

    if(delta == 0) {
        return;
    }

    const int32_t previous_steps = encoder->steps;

    encoder->accumulator += encoder->inverted ? -delta : delta;

    while(encoder->accumulator >= FURI_HAL_ENCODER_COUNTS_PER_STEP) {
        encoder->steps++;
        encoder->accumulator -= FURI_HAL_ENCODER_COUNTS_PER_STEP;
    }

    while(encoder->accumulator <= -FURI_HAL_ENCODER_COUNTS_PER_STEP) {
        encoder->steps--;
        encoder->accumulator += FURI_HAL_ENCODER_COUNTS_PER_STEP;
    }

    if((encoder->steps != previous_steps) && furi_hal_encoder.callback) {
        furi_hal_encoder.callback(furi_hal_encoder.callback_context);
    }
}

static void furi_hal_encoder_pin_init(const GpioPin* gpio, FuriHalEncoder* encoder) {
    furi_hal_gpio_init(gpio, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedLow);
    furi_hal_gpio_add_int_callback(gpio, furi_hal_encoder_isr, encoder);

    const IRQn_Type irqn = (IRQn_Type)(EXTI0_IRQn + __builtin_ctz(gpio->pin));
    NVIC_SetPriority(irqn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(irqn);
}

static void furi_hal_encoder_channel_init(FuriHalEncoder* encoder) {
    furi_hal_encoder_pin_init(encoder->pin_a, encoder);
    furi_hal_encoder_pin_init(encoder->pin_b, encoder);

    encoder->last_state = furi_hal_encoder_read_state(encoder);
    encoder->accumulator = 0;
    encoder->steps = 0;
}

void furi_hal_encoder_init(void) {
    furi_hal_encoder_channel_init(&furi_hal_encoder.vertical);
    furi_hal_encoder_channel_init(&furi_hal_encoder.horizontal);
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
