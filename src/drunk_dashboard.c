#include <stdbool.h>
#include <stdint.h>
#include <stm32h7xx_hal_def.h>
#include <stm32h7xx_ll_utils.h>
#include <malloc.h>
#include "u8g2/u8g2.h"
#include <halk/halk_spi.h>
#include <halk/halk_gpio.h>

static uint8_t
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

static uint8_t u8g2_4wire_hw_spi_stm32(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    switch(msg) {
    case U8X8_MSG_BYTE_INIT:
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

void drunk_dashboard() {
    halk_spi_init(SPI1);
    u8g2_t* display0 = malloc(sizeof(u8g2_t));
    //u8g2_t* display1 = malloc(sizeof(u8g2_t));

    u8g2_Setup_ssd1306_128x64_noname_f(
        display0, U8G2_R0, u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32_display0);
    //u8g2_Setup_ssd1306_128x64_noname_f(
    //    display1, U8G2_R0, u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32_display1);

    uint8_t* display0fb = malloc(u8g2_GetBufferSize(display0));
    //uint8_t* display1fb = malloc(u8g2_GetBufferSize(display1));

    u8g2_SetBufferPtr(display0, display0fb);
    u8g2_ClearBuffer(display0);
    //u8g2_SetBufferPtr(display1, display1fb);
    //u8g2_ClearBuffer(display1);

    u8g2_InitDisplay(display0);
    u8g2_SetContrast(display0, 0xff);
    u8g2_SetPowerSave(display0, 0);
    u8g2_SetFont(display0, u8g2_font_haxrcorp4089_tr);
    u8g2_DrawStr(display0, 30, 30, "Test");
    u8g2_DrawLine(display0, 0, 0, 10, 10);
    u8g2_DrawCircle(display0, 100, 50, 10, U8G2_DRAW_ALL);
    u8g2_DrawBox(display0, 50, 50, 10, 10);

    // u8g2_InitDisplay(display1);
    //u8g2_SetContrast(display1, 0x00);
    //u8g2_SetPowerSave(display1, 0);
    //u8g2_SetFont(display1, u8g2_font_haxrcorp4089_tr);
    //u8g2_DrawStr(display1, 5, 30, "Hi, from display1");
    //u8g2_DrawLine(display1, 50, 50, 57, 57);
    //u8g2_DrawTriangle(display1, 80, 30, 100, 50, 120, 30);
    //u8g2_DrawCircle(display1, 50, 50, 12, U8G2_DRAW_ALL);
    //u8g2_SendBuffer(display1);
    GPIOPin test_pin = {GPIOE, LL_GPIO_PIN_3};
    while(true) {
        u8g2_SendBuffer(display0);
        //u8g2_SendBuffer(display1);
    }
    //free(display1fb);
    free(display0fb);
    //free(display1);
    free(display0);
}
