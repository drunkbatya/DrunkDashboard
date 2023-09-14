#include "u8g2_glue.h"
#include <stm32h7xx_ll_utils.h>
#include <halk/halk.h>
#include <halk/halk_spi.h>
#include <halk/halk_gpio.h>

uint8_t
    u8g2_gpio_and_delay_stm32_display0(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    GPIOPin display_rst_pin = {GPIOE, LL_GPIO_PIN_12};
    GPIOPin display_dc_pin = {GPIOA, LL_GPIO_PIN_6};
    GPIOPin display_cs_pin = {GPIOE, LL_GPIO_PIN_13};
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        LL_mDelay(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        LL_mDelay(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&display_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t u8g2_4wire_hw_spi_stm32(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    switch(msg) {
    case U8X8_MSG_BYTE_INIT:
        halk_spi_init(SPI1);
        break;
    case U8X8_MSG_BYTE_SET_DC:
        u8x8_gpio_SetDC(u8x8, arg_int);
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
        break;
    case U8X8_MSG_BYTE_SEND:
        halk_spi_transmit(SPI1, (uint8_t*)arg_ptr, arg_int);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        break;
    default:
        return false;
    }
    return true;
}
