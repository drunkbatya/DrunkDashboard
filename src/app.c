#include <stdbool.h>
#include <stdint.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_hal_def.h>
#include <stm32f4xx_ll_utils.h>
#include <stm32f4xx_ll_spi.h>
#include "u8g2/u8g2.h"
#include "gpio.h"

static void gpio_spi_transmit_byte(uint8_t byte) {
    while(!(SPI1->SR & SPI_SR_TXE))
        ;
    LL_SPI_TransmitData8(SPI1, byte);
    while(SPI1->SR & SPI_SR_BSY)
        ;
}

static void gpio_spi_transmit(uint8_t *data, uint8_t size) {
    while(size) {
        gpio_spi_transmit_byte(*data);
        data++;
        size--;
    }
}

static uint8_t
    u8g2_gpio_and_delay_stm32(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    UNUSED(u8x8);
    UNUSED(arg_ptr);
    GPIOPin display_rst_pin = {GPIOA, LL_GPIO_PIN_4};
    GPIOPin display_dc_pin = {GPIOB, LL_GPIO_PIN_0};
    GPIOPin display_cs_pin = {GPIOB, LL_GPIO_PIN_6};
    switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        LL_SPI_Enable(SPI1);
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
        gpio_write(&display_rst_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        gpio_write(&display_dc_pin, arg_int);
        break;
    case U8X8_MSG_GPIO_CS:
        gpio_write(&display_cs_pin, arg_int);
        break;
    default:
        return false;
    }
    return true;
}

static uint8_t u8g2_4wire_hw_spi_stm32(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr) {
    switch(msg) {
    case U8X8_MSG_BYTE_INIT:
        LL_SPI_Enable(SPI1);
        break;
    case U8X8_MSG_BYTE_SET_DC:
        u8x8_gpio_SetDC(u8x8, arg_int);
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
        break;
    case U8X8_MSG_BYTE_SEND:
        gpio_spi_transmit((uint8_t *)arg_ptr, arg_int);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        break;
    default:
        return false;
    }
    return true;
}

void app() {
    u8g2_t u8g2;
    u8g2_Setup_ssd1306_128x64_noname_f(
        &u8g2, U8G2_R0, u8g2_4wire_hw_spi_stm32, u8g2_gpio_and_delay_stm32);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_SetFont(&u8g2, u8g2_font_haxrcorp4089_tr);
    u8g2_DrawStr(&u8g2, 30, 30, "Test");
    u8g2_DrawLine(&u8g2, 0, 0, 10, 10);
    u8g2_SendBuffer(&u8g2);
    while(true) {
        ;
        ;
    }
}
