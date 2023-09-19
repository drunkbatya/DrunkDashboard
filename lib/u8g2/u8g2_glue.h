#pragma once
#include <u8g2/u8g2.h>

#define U8G2_GLUE_TOTAL_DISPLAY_COUNT 9

uint8_t u8g2_4wire_hw_spi_stm32(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display0(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display1(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display2(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display3(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display4(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display5(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display6(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display7(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
uint8_t
    u8g2_gpio_and_delay_stm32_display8(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);

#define U8G2_GLUE_TOTAL_DISPLAY_COUNT 9

#define U8G2_GLUE_GET_GPIO_CB_BY_DISP_NUMBER(number) \
    (((number) == (0)) ? u8g2_gpio_and_delay_stm32_display0 : \
    ((number) == (1)) ? u8g2_gpio_and_delay_stm32_display1 : \
    ((number) == (2)) ? u8g2_gpio_and_delay_stm32_display2 : \
    ((number) == (3)) ? u8g2_gpio_and_delay_stm32_display3 : \
    ((number) == (4)) ? u8g2_gpio_and_delay_stm32_display4 : \
    ((number) == (5)) ? u8g2_gpio_and_delay_stm32_display5 : \
    ((number) == (6)) ? u8g2_gpio_and_delay_stm32_display6 : \
    ((number) == (7)) ? u8g2_gpio_and_delay_stm32_display7 : \
    ((number) == (8)) ? u8g2_gpio_and_delay_stm32_display8 : \
                         u8g2_gpio_and_delay_stm32_display0)
