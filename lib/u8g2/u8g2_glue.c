#include "u8g2_glue.h"
#include <hardware.h>
#include <halk/halk.h>
#include <halk/halk_delay.h>
#include <halk/halk_spi.h>
#include <halk/halk_gpio.h>

uint8_t
    u8g2_gpio_and_delay_stm32_display0(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display0_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display0_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t
    u8g2_gpio_and_delay_stm32_display1(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display1_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display1_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t
    u8g2_gpio_and_delay_stm32_display2(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display2_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display2_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t
    u8g2_gpio_and_delay_stm32_display3(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display3_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display3_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t
    u8g2_gpio_and_delay_stm32_display4(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display4_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display4_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t
    u8g2_gpio_and_delay_stm32_display5(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display5_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display5_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t
    u8g2_gpio_and_delay_stm32_display6(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display6_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display6_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t
    u8g2_gpio_and_delay_stm32_display7(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display7_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display7_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

uint8_t
    u8g2_gpio_and_delay_stm32_display8(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        break;
    case U8X8_MSG_DELAY_MILLI:
        halk_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        halk_delay_ms(1);
        break;
    case U8X8_MSG_DELAY_100NANO:
        asm("nop");
        break;
    case U8X8_MSG_GPIO_RESET:
        halk_gpio_pin_write(&display8_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        halk_gpio_pin_write(&displays_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        halk_gpio_pin_write(&display8_cs_pin, arg_int);
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
