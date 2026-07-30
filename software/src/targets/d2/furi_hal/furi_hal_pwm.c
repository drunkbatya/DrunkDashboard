#include <furi_hal_pwm.h>
#include <furi_hal_bus.h>
#include <furi_hal_resources.h>
#include <furi.h>

#include <stm32u5xx_ll_gpio.h>
#include <stm32u5xx_ll_tim.h>

#define FURI_HAL_PWM_DISPLAY_TIMER   TIM2
#define FURI_HAL_PWM_DISPLAY_CHANNEL LL_TIM_CHANNEL_CH2
#define FURI_HAL_PWM_DISPLAY_AF      LL_GPIO_AF_1

static bool furi_hal_pwm_display_running = false;

static void furi_hal_pwm_display_gpio_init(void) {
    LL_GPIO_InitTypeDef gpio_init = {0};

    furi_hal_bus_enable(FuriHalBusGPIOB);
    gpio_init.Pin = gpio_disp_pwm.pin;
    gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    gpio_init.Alternate = FURI_HAL_PWM_DISPLAY_AF;
    LL_GPIO_Init(gpio_disp_pwm.port, &gpio_init);
}

static void furi_hal_pwm_display_timer_init(void) {
    furi_hal_bus_enable(FuriHalBusTIM2);

    LL_TIM_InitTypeDef timer_init = {0};
    timer_init.Prescaler = 0;
    timer_init.CounterMode = LL_TIM_COUNTERMODE_UP;
    timer_init.Autoreload = 65535;
    timer_init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    timer_init.RepetitionCounter = 0;
    LL_TIM_Init(FURI_HAL_PWM_DISPLAY_TIMER, &timer_init);
    LL_TIM_DisableARRPreload(FURI_HAL_PWM_DISPLAY_TIMER);

    LL_TIM_OC_InitTypeDef oc_init = {0};
    oc_init.OCMode = LL_TIM_OCMODE_PWM1;
    oc_init.OCState = LL_TIM_OCSTATE_ENABLE;
    oc_init.OCNState = LL_TIM_OCSTATE_DISABLE;
    oc_init.CompareValue = 0;
    oc_init.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    oc_init.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    oc_init.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    oc_init.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(FURI_HAL_PWM_DISPLAY_TIMER, FURI_HAL_PWM_DISPLAY_CHANNEL, &oc_init);
    LL_TIM_OC_EnablePreload(FURI_HAL_PWM_DISPLAY_TIMER, FURI_HAL_PWM_DISPLAY_CHANNEL);
    LL_TIM_OC_DisableFast(FURI_HAL_PWM_DISPLAY_TIMER, FURI_HAL_PWM_DISPLAY_CHANNEL);
}

void furi_hal_pwm_start(FuriHalPwmOutputId channel, uint32_t freq, uint8_t duty) {
    furi_check(channel == FuriHalPwmOutputIdDisplayBacklight);

    furi_hal_pwm_display_gpio_init();
    furi_hal_pwm_display_timer_init();
    furi_hal_pwm_set_params(channel, freq, duty);
    LL_TIM_CC_EnableChannel(FURI_HAL_PWM_DISPLAY_TIMER, FURI_HAL_PWM_DISPLAY_CHANNEL);
    LL_TIM_EnableCounter(FURI_HAL_PWM_DISPLAY_TIMER);
    furi_hal_pwm_display_running = true;
}

void furi_hal_pwm_stop(FuriHalPwmOutputId channel) {
    furi_check(channel == FuriHalPwmOutputIdDisplayBacklight);

    LL_TIM_CC_DisableChannel(FURI_HAL_PWM_DISPLAY_TIMER, FURI_HAL_PWM_DISPLAY_CHANNEL);
    LL_TIM_DisableCounter(FURI_HAL_PWM_DISPLAY_TIMER);
    furi_hal_bus_disable(FuriHalBusTIM2);
    furi_hal_pwm_display_running = false;
}

bool furi_hal_pwm_is_running(FuriHalPwmOutputId channel) {
    furi_check(channel == FuriHalPwmOutputIdDisplayBacklight);
    return furi_hal_pwm_display_running;
}

void furi_hal_pwm_set_params(FuriHalPwmOutputId channel, uint32_t freq, uint8_t duty) {
    furi_check(channel == FuriHalPwmOutputIdDisplayBacklight);
    furi_check(freq > 0);
    if(duty > 100) duty = 100;

    uint32_t timer_clock = SystemCoreClock;
    uint32_t prescaler = (timer_clock / (freq * 65536UL)) + 1UL;
    uint32_t period = timer_clock / (prescaler * freq);
    if(period == 0) period = 1;
    if(period > 65536UL) period = 65536UL;

    uint32_t compare = (period * duty) / 100UL;

    LL_TIM_SetPrescaler(FURI_HAL_PWM_DISPLAY_TIMER, prescaler - 1UL);
    LL_TIM_SetAutoReload(FURI_HAL_PWM_DISPLAY_TIMER, period - 1UL);
    LL_TIM_OC_SetCompareCH2(FURI_HAL_PWM_DISPLAY_TIMER, compare);
    LL_TIM_GenerateEvent_UPDATE(FURI_HAL_PWM_DISPLAY_TIMER);
}
