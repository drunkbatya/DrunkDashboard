#include "u8g2_glue.h"

#include <furi_hal.h>

uint8_t u8g2_gpio_and_delay_stm32(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        furi_hal_gpio_init(&gpio_disp_res, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_dc, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_cs, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_d0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_d1, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_d2, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_d3, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_d4, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_d5, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_d6, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_disp_d7, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        break;
    case U8X8_MSG_DELAY_NANO:
        for(uint8_t i = 0; i < arg_int; i++) {
            asm volatile("nop");
        }
        break;
    case U8X8_MSG_DELAY_MILLI:
        furi_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        furi_delay_us(10);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        furi_hal_gpio_write(&gpio_disp_res, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        furi_hal_gpio_write(&gpio_disp_dc, arg_int);
        break;
    case U8X8_MSG_GPIO_E:
        furi_hal_gpio_write(&gpio_disp_cs, arg_int);
        break;
    case U8X8_MSG_GPIO_D0:
        furi_hal_gpio_write(&gpio_disp_d0, arg_int);
        break;
    case U8X8_MSG_GPIO_D1:
        furi_hal_gpio_write(&gpio_disp_d1, arg_int);
        break;
    case U8X8_MSG_GPIO_D2:
        furi_hal_gpio_write(&gpio_disp_d2, arg_int);
        break;
    case U8X8_MSG_GPIO_D3:
        furi_hal_gpio_write(&gpio_disp_d3, arg_int);
        break;
    case U8X8_MSG_GPIO_D4:
        furi_hal_gpio_write(&gpio_disp_d4, arg_int);
        break;
    case U8X8_MSG_GPIO_D5:
        furi_hal_gpio_write(&gpio_disp_d5, arg_int);
        break;
    case U8X8_MSG_GPIO_D6:
        furi_hal_gpio_write(&gpio_disp_d6, arg_int);
        break;
    case U8X8_MSG_GPIO_D7:
        furi_hal_gpio_write(&gpio_disp_d7, arg_int);
        break;
    default:
        return false;
    }
    return true;
}
