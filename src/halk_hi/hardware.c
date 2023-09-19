#include "hardware.h"

// displays
// displays global
const GPIOPin displays_dc_pin = {GPIOA, LL_GPIO_PIN_6};

// display 0
const GPIOPin display0_rst_pin = {GPIOE, LL_GPIO_PIN_12};
const GPIOPin display0_cs_pin = {GPIOE, LL_GPIO_PIN_13};

// display 1
const GPIOPin display1_rst_pin = {GPIOB, LL_GPIO_PIN_4};
const GPIOPin display1_cs_pin = {GPIOD, LL_GPIO_PIN_7};

// display 2
const GPIOPin display2_rst_pin = {GPIOB, LL_GPIO_PIN_5};
const GPIOPin display2_cs_pin = {GPIOB, LL_GPIO_PIN_7};

// display 3
const GPIOPin display3_rst_pin = {GPIOE, LL_GPIO_PIN_7};
const GPIOPin display3_cs_pin = {GPIOB, LL_GPIO_PIN_2};

// display 4
const GPIOPin display4_rst_pin = {GPIOB, LL_GPIO_PIN_9};
const GPIOPin display4_cs_pin = {GPIOB, LL_GPIO_PIN_6};

// display 5
const GPIOPin display5_rst_pin = {GPIOC, LL_GPIO_PIN_12};
const GPIOPin display5_cs_pin = {GPIOD, LL_GPIO_PIN_4};

// display 6
const GPIOPin display6_rst_pin = {GPIOE, LL_GPIO_PIN_10};
const GPIOPin display6_cs_pin = {GPIOE, LL_GPIO_PIN_4};

// display 7
const GPIOPin display7_rst_pin = {GPIOE, LL_GPIO_PIN_1};
const GPIOPin display7_cs_pin = {GPIOB, LL_GPIO_PIN_8};

// display 8
const GPIOPin display8_rst_pin = {GPIOC, LL_GPIO_PIN_10};
const GPIOPin display8_cs_pin = {GPIOD, LL_GPIO_PIN_1};
