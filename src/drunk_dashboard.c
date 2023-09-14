#include <stdbool.h>
#include <stdint.h>
#include <stm32h7xx_hal_def.h>
#include <malloc.h>
#include "u8g2/u8g2.h"
#include "u8g2/u8g2_glue.h"

void drunk_dashboard() {
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
    while(true) {
        u8g2_SendBuffer(display0);
        //u8g2_SendBuffer(display1);
    }
    //free(display1fb);
    free(display0fb);
    //free(display1);
    free(display0);
}
