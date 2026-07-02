#include <stdint.h>
#include <string.h>
#include <stm32u585_startup.h>
#include <stm32u5xx.h>

#ifndef VECT_TAB_OFFSET
#define VECT_TAB_OFFSET 0x00000000UL
#endif

typedef void (*IsrHandler)(void);

extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

uint32_t SystemCoreClock = 4000000U;

const uint8_t AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};
const uint8_t APBPrescTable[8] = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};
const uint32_t MSIRangeTable[16] = {
    48000000U,
    24000000U,
    16000000U,
    12000000U,
    4000000U,
    2000000U,
    1330000U,
    1000000U,
    3072000U,
    1536000U,
    1024000U,
    768000U,
    400000U,
    200000U,
    133000U,
    100000U,
};

void SystemInit(void) {
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 20U) | (3UL << 22U));
#endif

    RCC->CR = RCC_CR_MSISON;

    RCC->CFGR1 = 0U;
    RCC->CFGR2 = 0U;
    RCC->CFGR3 = 0U;

    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLL1ON | RCC_CR_PLL2ON | RCC_CR_PLL3ON);
    RCC->PLL1CFGR = 0U;
    RCC->CR &= ~RCC_CR_HSEBYP;
    RCC->CIER = 0U;

#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM1_BASE | VECT_TAB_OFFSET;
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
#endif
}

static uint32_t get_msi_frequency(void) {
    uint32_t range;

    if(READ_BIT(RCC->ICSCR1, RCC_ICSCR1_MSIRGSEL) == 0U) {
        range = (RCC->CSR & RCC_CSR_MSISSRANGE) >> RCC_CSR_MSISSRANGE_Pos;
    } else {
        range = (RCC->ICSCR1 & RCC_ICSCR1_MSISRANGE) >> RCC_ICSCR1_MSISRANGE_Pos;
    }

    return MSIRangeTable[range & 0x0FU];
}

void SystemCoreClockUpdate(void) {
    uint32_t sysclk;
    uint32_t msirange = get_msi_frequency();

    switch(RCC->CFGR1 & RCC_CFGR1_SWS) {
    case RCC_CFGR1_SWS_0:
        sysclk = HSI_VALUE;
        break;
    case RCC_CFGR1_SWS_1:
        sysclk = HSE_VALUE;
        break;
    case RCC_CFGR1_SWS:
        uint32_t source = 0U;
        uint32_t pllsource = RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1SRC;

        if(pllsource == RCC_PLL1CFGR_PLL1SRC_1) {
            source = HSI_VALUE;
        } else if(pllsource == (RCC_PLL1CFGR_PLL1SRC_0 | RCC_PLL1CFGR_PLL1SRC_1)) {
            source = HSE_VALUE;
        } else if(pllsource == RCC_PLL1CFGR_PLL1SRC_0) {
            source = msirange;
        }

        uint32_t pllm = ((RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1M) >> RCC_PLL1CFGR_PLL1M_Pos) + 1U;
        uint32_t plln = (RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1N) + 1U;
        uint32_t pllr = ((RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1R) >> RCC_PLL1DIVR_PLL1R_Pos) + 1U;
        uint32_t fracn = 0U;

        if(READ_BIT(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1FRACEN) != 0U) {
            fracn = (RCC->PLL1FRACR & RCC_PLL1FRACR_PLL1FRACN) >> RCC_PLL1FRACR_PLL1FRACN_Pos;
        }

        uint64_t vco =
            ((uint64_t)source * (((uint64_t)plln * 8192U) + fracn)) / ((uint64_t)pllm * 8192U);
        sysclk = (uint32_t)(vco / pllr);
        break;
    case 0U:
    default:
        sysclk = msirange;
        break;
    }

    uint32_t hpre = (RCC->CFGR2 & RCC_CFGR2_HPRE) >> RCC_CFGR2_HPRE_Pos;
    SystemCoreClock = sysclk >> AHBPrescTable[hpre & 0x0FU];
}

uint32_t SECURE_SystemCoreClockUpdate(void) {
    SystemCoreClockUpdate();
    return SystemCoreClock;
}

void Default_Handler(void) {
    furi_crash("NotImplemented");
}

FURI_NAKED void Reset_Handler(void) {
    __asm volatile("ldr r0, =_estack");
    __asm volatile("mov sp, r0");

    SystemInit();

    memcpy((void*)&_sdata, &_sidata, (uint32_t)&_edata - (uint32_t)&_sdata);
    memset((void*)&_sbss, 0x00, (uint32_t)&_ebss - (uint32_t)&_sbss);

    __libc_init_array();
    main();

    furi_crash("WhyExit?");
}

__attribute__((section(".isr_vector"), used)) const IsrHandler g_pfnVectors[] = {
    (IsrHandler)&_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    SecureFault_Handler,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,
    PVD_PVM_IRQHandler,
    RTC_IRQHandler,
    RTC_S_IRQHandler,
    TAMP_IRQHandler,
    RAMCFG_IRQHandler,
    FLASH_IRQHandler,
    FLASH_S_IRQHandler,
    GTZC_IRQHandler,
    RCC_IRQHandler,
    RCC_S_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    EXTI5_IRQHandler,
    EXTI6_IRQHandler,
    EXTI7_IRQHandler,
    EXTI8_IRQHandler,
    EXTI9_IRQHandler,
    EXTI10_IRQHandler,
    EXTI11_IRQHandler,
    EXTI12_IRQHandler,
    EXTI13_IRQHandler,
    EXTI14_IRQHandler,
    EXTI15_IRQHandler,
    IWDG_IRQHandler,
    SAES_IRQHandler,
    GPDMA1_Channel0_IRQHandler,
    GPDMA1_Channel1_IRQHandler,
    GPDMA1_Channel2_IRQHandler,
    GPDMA1_Channel3_IRQHandler,
    GPDMA1_Channel4_IRQHandler,
    GPDMA1_Channel5_IRQHandler,
    GPDMA1_Channel6_IRQHandler,
    GPDMA1_Channel7_IRQHandler,
    ADC1_IRQHandler,
    DAC1_IRQHandler,
    FDCAN1_IT0_IRQHandler,
    FDCAN1_IT1_IRQHandler,
    TIM1_BRK_IRQHandler,
    TIM1_UP_IRQHandler,
    TIM1_TRG_COM_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    TIM5_IRQHandler,
    TIM6_IRQHandler,
    TIM7_IRQHandler,
    TIM8_BRK_IRQHandler,
    TIM8_UP_IRQHandler,
    TIM8_TRG_COM_IRQHandler,
    TIM8_CC_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,
    UART4_IRQHandler,
    UART5_IRQHandler,
    LPUART1_IRQHandler,
    LPTIM1_IRQHandler,
    LPTIM2_IRQHandler,
    TIM15_IRQHandler,
    TIM16_IRQHandler,
    TIM17_IRQHandler,
    COMP_IRQHandler,
    OTG_FS_IRQHandler,
    CRS_IRQHandler,
    FMC_IRQHandler,
    OCTOSPI1_IRQHandler,
    PWR_S3WU_IRQHandler,
    SDMMC1_IRQHandler,
    SDMMC2_IRQHandler,
    GPDMA1_Channel8_IRQHandler,
    GPDMA1_Channel9_IRQHandler,
    GPDMA1_Channel10_IRQHandler,
    GPDMA1_Channel11_IRQHandler,
    GPDMA1_Channel12_IRQHandler,
    GPDMA1_Channel13_IRQHandler,
    GPDMA1_Channel14_IRQHandler,
    GPDMA1_Channel15_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    SAI1_IRQHandler,
    SAI2_IRQHandler,
    TSC_IRQHandler,
    AES_IRQHandler,
    RNG_IRQHandler,
    FPU_IRQHandler,
    HASH_IRQHandler,
    PKA_IRQHandler,
    LPTIM3_IRQHandler,
    SPI3_IRQHandler,
    I2C4_ER_IRQHandler,
    I2C4_EV_IRQHandler,
    MDF1_FLT0_IRQHandler,
    MDF1_FLT1_IRQHandler,
    MDF1_FLT2_IRQHandler,
    MDF1_FLT3_IRQHandler,
    UCPD1_IRQHandler,
    ICACHE_IRQHandler,
    OTFDEC1_IRQHandler,
    OTFDEC2_IRQHandler,
    LPTIM4_IRQHandler,
    DCACHE1_IRQHandler,
    ADF1_IRQHandler,
    ADC4_IRQHandler,
    LPDMA1_Channel0_IRQHandler,
    LPDMA1_Channel1_IRQHandler,
    LPDMA1_Channel2_IRQHandler,
    LPDMA1_Channel3_IRQHandler,
    DMA2D_IRQHandler,
    DCMI_PSSI_IRQHandler,
    OCTOSPI2_IRQHandler,
    MDF1_FLT4_IRQHandler,
    MDF1_FLT5_IRQHandler,
    CORDIC_IRQHandler,
    FMAC_IRQHandler,
    LSECSSD_IRQHandler,
};
