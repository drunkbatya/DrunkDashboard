#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi_hal_gpio.h>
#include <furi.h>

#include <stm32u5xx_ll_gpio.h>

#define TAG "FuriHalResources"

const GpioPin gpio_swdio = {.port = GPIOA, .pin = LL_GPIO_PIN_13};
const GpioPin gpio_swclk = {.port = GPIOA, .pin = LL_GPIO_PIN_14};

const GpioPin gpio_disp_res = {.port = GPIOA, .pin = LL_GPIO_PIN_15};
const GpioPin gpio_disp_dc = {.port = GPIOB, .pin = LL_GPIO_PIN_13};
const GpioPin gpio_disp_cs = {.port = GPIOC, .pin = LL_GPIO_PIN_13};
const GpioPin gpio_disp_d0 = {.port = GPIOB, .pin = LL_GPIO_PIN_0};
const GpioPin gpio_disp_d1 = {.port = GPIOB, .pin = LL_GPIO_PIN_1};
const GpioPin gpio_disp_d2 = {.port = GPIOB, .pin = LL_GPIO_PIN_2};
const GpioPin gpio_disp_d3 = {.port = GPIOB, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_disp_d4 = {.port = GPIOB, .pin = LL_GPIO_PIN_4};
const GpioPin gpio_disp_d5 = {.port = GPIOB, .pin = LL_GPIO_PIN_5};
const GpioPin gpio_disp_d6 = {.port = GPIOB, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_disp_d7 = {.port = GPIOB, .pin = LL_GPIO_PIN_7};
const GpioPin gpio_disp_pwm = {.port = GPIOA, .pin = LL_GPIO_PIN_8};

const GpioPin gpio_button_ok = {.port = GPIOB, .pin = LL_GPIO_PIN_12};

const GpioPin gpio_spi2_sck = {.port = GPIOB, .pin = LL_GPIO_PIN_10};
const GpioPin gpio_spi2_miso = {.port = GPIOB, .pin = LL_GPIO_PIN_14};
const GpioPin gpio_spi2_mosi = {.port = GPIOB, .pin = LL_GPIO_PIN_15};

const GpioPin gpio_adc1_in7 = {.port = GPIOA, .pin = LL_GPIO_PIN_2};
const GpioPin gpio_adc1_in8 = {.port = GPIOA, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_adc1_in11 = {.port = GPIOA, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_adc1_in12 = {.port = GPIOA, .pin = LL_GPIO_PIN_7};

const GpioPin gpio_usart_tx = {.port = GPIOA, .pin = LL_GPIO_PIN_9};
const GpioPin gpio_usart_rx = {.port = GPIOA, .pin = LL_GPIO_PIN_10};

const GpioPin gpio_i2c1_scl = {.port = GPIOB, .pin = LL_GPIO_PIN_8};
const GpioPin gpio_i2c1_sda = {.port = GPIOB, .pin = LL_GPIO_PIN_9};

const InputPin input_pins[] = {
    {.gpio = &gpio_button_ok, .key = InputKeyOk, .inverted = true, .name = "OK"},
};

const size_t input_pins_count = COUNT_OF(input_pins);

const GpioPinRecord gpio_pins[] = {
    {
        .pin = &gpio_adc1_in7,
        .name = "PA2",
        .channel = FuriHalAdcChannel7,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 0,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in8,
        .name = "PA3",
        .channel = FuriHalAdcChannel8,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 0,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in11,
        .name = "PA6",
        .channel = FuriHalAdcChannel11,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 0,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in12,
        .name = "PA7",
        .channel = FuriHalAdcChannel12,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 0,
        .debug = false,
    },
    {
        .pin = &gpio_disp_pwm,
        .name = "PA8",
        .channel = FuriHalAdcChannelNone,
        .pwm_output = FuriHalPwmOutputIdDisplayBacklight,
        .number = 0,
        .debug = false,
    },
};

const size_t gpio_pins_count = COUNT_OF(gpio_pins);

static void furi_hal_resources_init_input_pins(GpioMode mode) {
    for(size_t i = 0; i < input_pins_count; i++) {
        furi_hal_gpio_init(
            input_pins[i].gpio,
            mode,
            input_pins[i].inverted ? GpioPullUp : GpioPullDown,
            GpioSpeedLow);
    }
}

void furi_hal_resources_init_early(void) {
    furi_hal_bus_enable(FuriHalBusGPIOA);
    furi_hal_bus_enable(FuriHalBusGPIOB);
    furi_hal_bus_enable(FuriHalBusGPIOC);
    furi_hal_bus_enable(FuriHalBusGPIOH);

    furi_hal_resources_init_input_pins(GpioModeInput);
}

void furi_hal_resources_deinit_early(void) {
    furi_hal_resources_init_input_pins(GpioModeAnalog);
    furi_hal_bus_disable(FuriHalBusGPIOA);
    furi_hal_bus_disable(FuriHalBusGPIOB);
    furi_hal_bus_disable(FuriHalBusGPIOC);
    furi_hal_bus_disable(FuriHalBusGPIOH);
}

void furi_hal_resources_init(void) {
    furi_hal_resources_init_input_pins(GpioModeInterruptRiseFall);

    NVIC_SetPriority(EXTI12_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI12_IRQn);

    FURI_LOG_I(TAG, "Init OK");
}

int32_t furi_hal_resources_get_ext_pin_number(const GpioPin* gpio) {
    for(size_t i = 0; i < gpio_pins_count; i++) {
        if(gpio_pins[i].pin == gpio) return gpio_pins[i].number;
    }
    return -1;
}

const GpioPinRecord* furi_hal_resources_pin_by_name(const char* name) {
    for(size_t i = 0; i < gpio_pins_count; i++) {
        const GpioPinRecord* record = &gpio_pins[i];
        if(strcasecmp(name, record->name) == 0) return record;
    }
    return NULL;
}

const GpioPinRecord* furi_hal_resources_pin_by_number(uint8_t number) {
    for(size_t i = 0; i < gpio_pins_count; i++) {
        const GpioPinRecord* record = &gpio_pins[i];
        if(record->number == number) return record;
    }
    return NULL;
}
