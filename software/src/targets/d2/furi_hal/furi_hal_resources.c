#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi_hal_gpio.h>
#include <furi.h>

#include <stm32u5xx_ll_gpio.h>

#define TAG "FuriHalResources"

const GpioPin gpio_swdio = {.port = GPIOA, .pin = LL_GPIO_PIN_13};
const GpioPin gpio_swclk = {.port = GPIOA, .pin = LL_GPIO_PIN_14};

const GpioPin gpio_disp_res = {.port = GPIOC, .pin = LL_GPIO_PIN_10};
const GpioPin gpio_disp_dc = {.port = GPIOC, .pin = LL_GPIO_PIN_12};
const GpioPin gpio_disp_cs = {.port = GPIOC, .pin = LL_GPIO_PIN_11};
const GpioPin gpio_disp_d0 = {.port = GPIOD, .pin = LL_GPIO_PIN_0};
const GpioPin gpio_disp_d1 = {.port = GPIOD, .pin = LL_GPIO_PIN_1};
const GpioPin gpio_disp_d2 = {.port = GPIOD, .pin = LL_GPIO_PIN_2};
const GpioPin gpio_disp_d3 = {.port = GPIOD, .pin = LL_GPIO_PIN_3};
const GpioPin gpio_disp_d4 = {.port = GPIOD, .pin = LL_GPIO_PIN_4};
const GpioPin gpio_disp_d5 = {.port = GPIOD, .pin = LL_GPIO_PIN_5};
const GpioPin gpio_disp_d6 = {.port = GPIOD, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_disp_d7 = {.port = GPIOD, .pin = LL_GPIO_PIN_7};
const GpioPin gpio_disp_pwm = {.port = GPIOB, .pin = LL_GPIO_PIN_3};

const GpioPin gpio_button_ok = {.port = GPIOB, .pin = LL_GPIO_PIN_2};
const GpioPin gpio_button_back = {.port = GPIOE, .pin = LL_GPIO_PIN_8};

const GpioPin gpio_encoder_1_a = {.port = GPIOC, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_encoder_1_b = {.port = GPIOC, .pin = LL_GPIO_PIN_7};
const GpioPin gpio_encoder_2_a = {.port = GPIOB, .pin = LL_GPIO_PIN_1};
const GpioPin gpio_encoder_2_b = {.port = GPIOD, .pin = LL_GPIO_PIN_9};

const GpioPin gpio_spi_sck = {.port = GPIOA, .pin = LL_GPIO_PIN_5};
const GpioPin gpio_spi_miso = {.port = GPIOA, .pin = LL_GPIO_PIN_6};
const GpioPin gpio_spi_mosi = {.port = GPIOA, .pin = LL_GPIO_PIN_7};

const GpioPin gpio_usart_tx = {.port = GPIOA, .pin = LL_GPIO_PIN_9};
const GpioPin gpio_usart_rx = {.port = GPIOA, .pin = LL_GPIO_PIN_10};

const GpioPin gpio_fdcan1_rx = {.port = GPIOA, .pin = LL_GPIO_PIN_11};
const GpioPin gpio_fdcan1_tx = {.port = GPIOA, .pin = LL_GPIO_PIN_12};

const GpioPin gpio_i2c1_scl = {.port = GPIOB, .pin = LL_GPIO_PIN_8};
const GpioPin gpio_i2c1_sda = {.port = GPIOB, .pin = LL_GPIO_PIN_9};

static const GpioPin gpio_adc1_in1 = {.port = GPIOC, .pin = LL_GPIO_PIN_0};
static const GpioPin gpio_adc1_in2 = {.port = GPIOC, .pin = LL_GPIO_PIN_1};
static const GpioPin gpio_adc1_in3 = {.port = GPIOC, .pin = LL_GPIO_PIN_2};
static const GpioPin gpio_adc1_in4 = {.port = GPIOC, .pin = LL_GPIO_PIN_3};
static const GpioPin gpio_adc1_in5 = {.port = GPIOA, .pin = LL_GPIO_PIN_0};
static const GpioPin gpio_adc1_in6 = {.port = GPIOA, .pin = LL_GPIO_PIN_1};
static const GpioPin gpio_adc1_in7 = {.port = GPIOA, .pin = LL_GPIO_PIN_2};
static const GpioPin gpio_adc1_in8 = {.port = GPIOA, .pin = LL_GPIO_PIN_3};
static const GpioPin gpio_adc1_in9 = {.port = GPIOA, .pin = LL_GPIO_PIN_4};

const InputPin input_pins[] = {
    {.gpio = &gpio_button_ok, .key = InputKeyOk, .inverted = true, .name = "OK"},
    {.gpio = &gpio_button_back, .key = InputKeyBack, .inverted = true, .name = "Back"},
};

const size_t input_pins_count = COUNT_OF(input_pins);

const GpioPinRecord gpio_pins[] = {
    {
        .pin = &gpio_adc1_in1,
        .name = "PC0",
        .channel = FuriHalAdcChannel1,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 1,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in2,
        .name = "PC1",
        .channel = FuriHalAdcChannel2,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 2,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in3,
        .name = "PC2",
        .channel = FuriHalAdcChannel3,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 3,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in4,
        .name = "PC3",
        .channel = FuriHalAdcChannel4,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 4,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in5,
        .name = "PA0",
        .channel = FuriHalAdcChannel5,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 5,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in6,
        .name = "PA1",
        .channel = FuriHalAdcChannel6,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 6,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in7,
        .name = "PA2",
        .channel = FuriHalAdcChannel7,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 7,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in8,
        .name = "PA3",
        .channel = FuriHalAdcChannel8,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 8,
        .debug = false,
    },
    {
        .pin = &gpio_adc1_in9,
        .name = "PA4",
        .channel = FuriHalAdcChannel9,
        .pwm_output = FuriHalPwmOutputIdNone,
        .number = 9,
        .debug = false,
    },
    {
        .pin = &gpio_disp_pwm,
        .name = "PB3",
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
    furi_hal_bus_enable(FuriHalBusGPIOD);
    furi_hal_bus_enable(FuriHalBusGPIOE);
    furi_hal_bus_enable(FuriHalBusGPIOH);

    furi_hal_resources_init_input_pins(GpioModeInput);
}

void furi_hal_resources_deinit_early(void) {
    furi_hal_resources_init_input_pins(GpioModeAnalog);
    furi_hal_bus_disable(FuriHalBusGPIOA);
    furi_hal_bus_disable(FuriHalBusGPIOB);
    furi_hal_bus_disable(FuriHalBusGPIOC);
    furi_hal_bus_disable(FuriHalBusGPIOD);
    furi_hal_bus_disable(FuriHalBusGPIOE);
    furi_hal_bus_disable(FuriHalBusGPIOH);
}

void furi_hal_resources_init(void) {
    furi_hal_resources_init_input_pins(GpioModeInterruptRiseFall);

    NVIC_SetPriority(EXTI2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_SetPriority(EXTI8_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(EXTI8_IRQn);

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
