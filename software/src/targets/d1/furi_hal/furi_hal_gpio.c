#include <furi.h>
#include <furi_hal_gpio.h>
#include <furi_hal_version.h>
#include <furi_hal_resources.h>
#include <stm32u5xx_ll_comp.h>
#include <stm32u5xx_ll_exti.h>
#include <stm32u5xx_ll_pwr.h>

static uint32_t furi_hal_gpio_invalid_argument_crash(void) {
    furi_crash("Invalid argument");
    return 0;
}

#define GPIO_PORT_MAP(port, prefix)    \
    (((port) == (GPIOA)) ? prefix##A : \
     ((port) == (GPIOB)) ? prefix##B : \
     ((port) == (GPIOC)) ? prefix##C : \
     ((port) == (GPIOD)) ? prefix##D : \
     ((port) == (GPIOE)) ? prefix##E : \
     ((port) == (GPIOH)) ? prefix##H : \
                           furi_hal_gpio_invalid_argument_crash())

#define GPIO_PIN_MAP(pin, prefix)               \
    (((pin) == (LL_GPIO_PIN_0))  ? prefix##0 :  \
     ((pin) == (LL_GPIO_PIN_1))  ? prefix##1 :  \
     ((pin) == (LL_GPIO_PIN_2))  ? prefix##2 :  \
     ((pin) == (LL_GPIO_PIN_3))  ? prefix##3 :  \
     ((pin) == (LL_GPIO_PIN_4))  ? prefix##4 :  \
     ((pin) == (LL_GPIO_PIN_5))  ? prefix##5 :  \
     ((pin) == (LL_GPIO_PIN_6))  ? prefix##6 :  \
     ((pin) == (LL_GPIO_PIN_7))  ? prefix##7 :  \
     ((pin) == (LL_GPIO_PIN_8))  ? prefix##8 :  \
     ((pin) == (LL_GPIO_PIN_9))  ? prefix##9 :  \
     ((pin) == (LL_GPIO_PIN_10)) ? prefix##10 : \
     ((pin) == (LL_GPIO_PIN_11)) ? prefix##11 : \
     ((pin) == (LL_GPIO_PIN_12)) ? prefix##12 : \
     ((pin) == (LL_GPIO_PIN_13)) ? prefix##13 : \
     ((pin) == (LL_GPIO_PIN_14)) ? prefix##14 : \
     ((pin) == (LL_GPIO_PIN_15)) ? prefix##15 : \
                                   furi_hal_gpio_invalid_argument_crash())

#define GET_EXTI_PORT(port)       GPIO_PORT_MAP(port, LL_EXTI_EXTI_PORT)
#define GET_EXTI_SOURCE_LINE(pin) GPIO_PIN_MAP(pin, LL_EXTI_EXTI_LINE)
#define GET_EXTI_LINE(pin)        GPIO_PIN_MAP(pin, LL_EXTI_LINE_)

#define GET_PWR_PIN(pin) GPIO_PIN_MAP(pin, LL_PWR_GPIO_PIN_)

static volatile GpioInterrupt gpio_interrupt[GPIO_NUMBER];

static uint8_t furi_hal_gpio_get_pin_num(const GpioPin* gpio) {
    uint8_t pin_num = 0;
    for(pin_num = 0; pin_num < GPIO_NUMBER; pin_num++) {
        if(gpio->pin & (1 << pin_num)) break;
    }
    return pin_num;
}

static uint32_t furi_hal_gpio_get_pwr_port(GPIO_TypeDef* port) {
    if(port == GPIOA) return (uint32_t)LL_PWR_GPIO_PORTA;
    if(port == GPIOB) return (uint32_t)LL_PWR_GPIO_PORTB;
    if(port == GPIOC) return (uint32_t)LL_PWR_GPIO_PORTC;
    if(port == GPIOD) return (uint32_t)LL_PWR_GPIO_PORTD;
    if(port == GPIOE) return (uint32_t)LL_PWR_GPIO_PORTE;
    if(port == GPIOH) return (uint32_t)LL_PWR_GPIO_PORTH;
    furi_crash("Invalid argument");
}

void furi_hal_gpio_init_simple(const GpioPin* gpio, const GpioMode mode) {
    furi_hal_gpio_init(gpio, mode, GpioPullNo, GpioSpeedLow);
}

void furi_hal_gpio_init(
    const GpioPin* gpio,
    const GpioMode mode,
    const GpioPull pull,
    const GpioSpeed speed) {
    // we cannot set alternate mode in this function
    furi_check(mode != GpioModeAltFunctionPushPull);
    furi_check(mode != GpioModeAltFunctionOpenDrain);

    furi_hal_gpio_init_ex(gpio, mode, pull, speed, GpioAltFnUnused);
}

void furi_hal_gpio_init_ex(
    const GpioPin* gpio,
    const GpioMode mode,
    const GpioPull pull,
    const GpioSpeed speed,
    const GpioAltFn alt_fn) {
    const uint32_t exti_port = GET_EXTI_PORT(gpio->port);
    const uint32_t exti_source_line = GET_EXTI_SOURCE_LINE(gpio->pin);
    const uint32_t exti_line = GET_EXTI_LINE(gpio->pin);
    const uint32_t pwr_port = furi_hal_gpio_get_pwr_port(gpio->port);
    const uint32_t pwr_pin = GET_PWR_PIN(gpio->pin);

    // Configure gpio with interrupts disabled
    FURI_CRITICAL_ENTER();

    // Set gpio speed
    switch(speed) {
    case GpioSpeedLow:
        LL_GPIO_SetPinSpeed(gpio->port, gpio->pin, LL_GPIO_SPEED_FREQ_LOW);
        break;
    case GpioSpeedMedium:
        LL_GPIO_SetPinSpeed(gpio->port, gpio->pin, LL_GPIO_SPEED_FREQ_MEDIUM);
        break;
    case GpioSpeedHigh:
        LL_GPIO_SetPinSpeed(gpio->port, gpio->pin, LL_GPIO_SPEED_FREQ_HIGH);
        break;
    case GpioSpeedVeryHigh:
        LL_GPIO_SetPinSpeed(gpio->port, gpio->pin, LL_GPIO_SPEED_FREQ_VERY_HIGH);
        break;
    }

    // Set gpio pull mode
    switch(pull) {
    case GpioPullNo:
        LL_GPIO_SetPinPull(gpio->port, gpio->pin, LL_GPIO_PULL_NO);
        LL_PWR_DisableGPIOPullUp(pwr_port, pwr_pin);
        LL_PWR_DisableGPIOPullDown(pwr_port, pwr_pin);
        break;
    case GpioPullUp:
        LL_GPIO_SetPinPull(gpio->port, gpio->pin, LL_GPIO_PULL_UP);
        LL_PWR_DisableGPIOPullDown(pwr_port, pwr_pin);
        LL_PWR_EnableGPIOPullUp(pwr_port, pwr_pin);
        break;
    case GpioPullDown:
        LL_GPIO_SetPinPull(gpio->port, gpio->pin, LL_GPIO_PULL_DOWN);
        LL_PWR_DisableGPIOPullUp(pwr_port, pwr_pin);
        LL_PWR_EnableGPIOPullDown(pwr_port, pwr_pin);
        break;
    default:
        furi_crash("Incorrect GpioPull");
    }

    // Set gpio mode
    if(mode >= GpioModeInterruptRise) {
        // Set pin in interrupt mode
        LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_INPUT);
        LL_EXTI_SetEXTISource(exti_port, exti_source_line);
        if(mode == GpioModeInterruptRise || mode == GpioModeInterruptRiseFall) {
            LL_EXTI_EnableRisingTrig_0_31(exti_line);
        }
        if(mode == GpioModeInterruptFall || mode == GpioModeInterruptRiseFall) {
            LL_EXTI_EnableFallingTrig_0_31(exti_line);
        }
        if(mode == GpioModeEventRise || mode == GpioModeEventRiseFall) {
            LL_EXTI_EnableEvent_0_31(exti_line);
            LL_EXTI_EnableRisingTrig_0_31(exti_line);
        }
        if(mode == GpioModeEventFall || mode == GpioModeEventRiseFall) {
            LL_EXTI_EnableEvent_0_31(exti_line);
            LL_EXTI_EnableFallingTrig_0_31(exti_line);
        }
    } else {
        // Disable interrupts if set
        if(LL_EXTI_GetEXTISource(exti_source_line) == exti_port &&
           LL_EXTI_IsEnabledIT_0_31(exti_line)) {
            LL_EXTI_DisableIT_0_31(exti_line);
            LL_EXTI_ClearRisingFlag_0_31(exti_line);
            LL_EXTI_ClearFallingFlag_0_31(exti_line);
            LL_EXTI_DisableRisingTrig_0_31(exti_line);
            LL_EXTI_DisableFallingTrig_0_31(exti_line);
        }

        // Prepare alternative part if any
        if(mode == GpioModeAltFunctionPushPull || mode == GpioModeAltFunctionOpenDrain) {
            // set alternate function
            if(furi_hal_gpio_get_pin_num(gpio) < 8) {
                LL_GPIO_SetAFPin_0_7(gpio->port, gpio->pin, alt_fn);
            } else {
                LL_GPIO_SetAFPin_8_15(gpio->port, gpio->pin, alt_fn);
            }
        }

        // Set not interrupt pin modes
        switch(mode) {
        case GpioModeInput:
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_INPUT);
            break;
        case GpioModeOutputPushPull:
            LL_GPIO_SetPinOutputType(gpio->port, gpio->pin, LL_GPIO_OUTPUT_PUSHPULL);
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_OUTPUT);
            break;
        case GpioModeAltFunctionPushPull:
            LL_GPIO_SetPinOutputType(gpio->port, gpio->pin, LL_GPIO_OUTPUT_PUSHPULL);
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_ALTERNATE);
            break;
        case GpioModeOutputOpenDrain:
            LL_GPIO_SetPinOutputType(gpio->port, gpio->pin, LL_GPIO_OUTPUT_OPENDRAIN);
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_OUTPUT);
            break;
        case GpioModeAltFunctionOpenDrain:
            LL_GPIO_SetPinOutputType(gpio->port, gpio->pin, LL_GPIO_OUTPUT_OPENDRAIN);
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_ALTERNATE);
            break;
        case GpioModeAnalog:
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_ANALOG);
            break;
        default:
            furi_crash("Incorrect GpioMode");
        }
    }
    FURI_CRITICAL_EXIT();
}

void furi_hal_gpio_add_int_callback(const GpioPin* gpio, GpioExtiCallback cb, void* ctx) {
    furi_check(gpio);
    furi_check(cb);

    FURI_CRITICAL_ENTER();

    uint8_t pin_num = furi_hal_gpio_get_pin_num(gpio);
    furi_check(gpio_interrupt[pin_num].callback == NULL);
    gpio_interrupt[pin_num].callback = cb;
    gpio_interrupt[pin_num].context = ctx;

    const uint32_t exti_line = GET_EXTI_LINE(gpio->pin);
    LL_EXTI_EnableIT_0_31(exti_line);

    FURI_CRITICAL_EXIT();
}

void furi_hal_gpio_enable_int_callback(const GpioPin* gpio) {
    furi_check(gpio);

    FURI_CRITICAL_ENTER();

    const uint32_t exti_line = GET_EXTI_LINE(gpio->pin);
    LL_EXTI_EnableIT_0_31(exti_line);

    FURI_CRITICAL_EXIT();
}

void furi_hal_gpio_disable_int_callback(const GpioPin* gpio) {
    furi_check(gpio);

    FURI_CRITICAL_ENTER();

    const uint32_t exti_line = GET_EXTI_LINE(gpio->pin);
    LL_EXTI_DisableIT_0_31(exti_line);
    LL_EXTI_ClearRisingFlag_0_31(exti_line);
    LL_EXTI_ClearFallingFlag_0_31(exti_line);

    FURI_CRITICAL_EXIT();
}

void furi_hal_gpio_remove_int_callback(const GpioPin* gpio) {
    furi_check(gpio);

    FURI_CRITICAL_ENTER();

    const uint32_t exti_line = GET_EXTI_LINE(gpio->pin);
    LL_EXTI_DisableIT_0_31(exti_line);
    LL_EXTI_ClearRisingFlag_0_31(exti_line);
    LL_EXTI_ClearFallingFlag_0_31(exti_line);

    uint8_t pin_num = furi_hal_gpio_get_pin_num(gpio);
    gpio_interrupt[pin_num].callback = NULL;
    gpio_interrupt[pin_num].context = NULL;

    FURI_CRITICAL_EXIT();
}

FURI_ALWAYS_INLINE static void furi_hal_gpio_int_call(uint16_t pin_num) {
    if(gpio_interrupt[pin_num].callback) {
        gpio_interrupt[pin_num].callback(gpio_interrupt[pin_num].context);
    }
}

FURI_ALWAYS_INLINE static void furi_hal_gpio_exti_handle(uint32_t exti_line, uint16_t pin_num) {
    if(LL_EXTI_IsActiveRisingFlag_0_31(exti_line)) {
        LL_EXTI_ClearRisingFlag_0_31(exti_line);
        furi_hal_gpio_int_call(pin_num);
    }
    if(LL_EXTI_IsActiveFallingFlag_0_31(exti_line)) {
        LL_EXTI_ClearFallingFlag_0_31(exti_line);
        furi_hal_gpio_int_call(pin_num);
    }
}

/* Interrupt handlers */
void EXTI0_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_0, 0);
}

void EXTI1_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_1, 1);
}

void EXTI2_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_2, 2);
}

void EXTI3_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_3, 3);
}

void EXTI4_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_4, 4);
}

void EXTI5_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_5, 5);
}

void EXTI6_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_6, 6);
}

void EXTI7_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_7, 7);
}

void EXTI8_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_8, 8);
}

void EXTI9_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_9, 9);
}

void EXTI10_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_10, 10);
}

void EXTI11_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_11, 11);
}

void EXTI12_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_12, 12);
}

void EXTI13_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_13, 13);
}

void EXTI14_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_14, 14);
}

void EXTI15_IRQHandler(void) {
    furi_hal_gpio_exti_handle(LL_EXTI_LINE_15, 15);
}
