#pragma once

#include <furi.h>
#include <furi_hal_adc.h>
#include <furi_hal_pwm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_DEBOUNCE_TICKS 4

typedef enum {
    InputKeyUp,
    InputKeyDown,
    InputKeyRight,
    InputKeyLeft,
    InputKeyOk,
    InputKeyBack,
    InputKeyMAX,
} InputKey;

typedef struct {
    const GpioPin* gpio;
    const InputKey key;
    const bool inverted;
    const char* name;
} InputPin;

typedef struct {
    const GpioPin* pin;
    const char* name;
    const FuriHalAdcChannel channel;
    const FuriHalPwmOutputId pwm_output;
    const uint8_t number;
    const bool debug;
} GpioPinRecord;

extern const InputPin input_pins[];
extern const size_t input_pins_count;

extern const GpioPinRecord gpio_pins[];
extern const size_t gpio_pins_count;

extern const GpioPin gpio_swdio;
extern const GpioPin gpio_swclk;

extern const GpioPin gpio_disp_res;
extern const GpioPin gpio_disp_dc;
extern const GpioPin gpio_disp_cs;
extern const GpioPin gpio_disp_d0;
extern const GpioPin gpio_disp_d1;
extern const GpioPin gpio_disp_d2;
extern const GpioPin gpio_disp_d3;
extern const GpioPin gpio_disp_d4;
extern const GpioPin gpio_disp_d5;
extern const GpioPin gpio_disp_d6;
extern const GpioPin gpio_disp_d7;
extern const GpioPin gpio_disp_pwm;

extern const GpioPin gpio_button_ok;

extern const GpioPin gpio_spi2_sck;
extern const GpioPin gpio_spi2_miso;
extern const GpioPin gpio_spi2_mosi;

extern const GpioPin gpio_adc1_in7;
extern const GpioPin gpio_adc1_in8;
extern const GpioPin gpio_adc1_in11;
extern const GpioPin gpio_adc1_in12;

extern const GpioPin gpio_usart_tx;
extern const GpioPin gpio_usart_rx;

extern const GpioPin gpio_i2c1_scl;
extern const GpioPin gpio_i2c1_sda;

void furi_hal_resources_init_early(void);
void furi_hal_resources_deinit_early(void);
void furi_hal_resources_init(void);

int32_t furi_hal_resources_get_ext_pin_number(const GpioPin* gpio);
const GpioPinRecord* furi_hal_resources_pin_by_name(const char* name);
const GpioPinRecord* furi_hal_resources_pin_by_number(uint8_t number);

#ifdef __cplusplus
}
#endif
