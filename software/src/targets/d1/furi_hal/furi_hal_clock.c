#include <furi_hal_clock.h>
#include <furi.h>

#include <stm32u5xx_ll_bus.h>
#include <stm32u5xx_ll_cortex.h>
#include <stm32u5xx_ll_pwr.h>
#include <stm32u5xx_ll_rcc.h>
#include <stm32u5xx_ll_system.h>
#include <stm32u5xx_ll_utils.h>

#define CPU_CLOCK_EARLY_HZ 4000000U
#define CPU_CLOCK_HSI_HZ   16000000U
#define CPU_CLOCK_HSE_HZ   25000000U
#define CPU_CLOCK_PLL_HZ   160000000U

#define TICK_INT_PRIORITY 15U

void furi_hal_clock_init_early(void) {
    NVIC_SetPriorityGrouping(3U);
    NVIC_SetPriority(
        PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), TICK_INT_PRIORITY, 0));
    NVIC_SetPriority(
        SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), TICK_INT_PRIORITY, 0));

    LL_SetSystemCoreClock(CPU_CLOCK_EARLY_HZ);
    LL_Init1msTick(SystemCoreClock);
    LL_SYSTICK_EnableIT();
}

void furi_hal_clock_deinit_early(void) {
}

void furi_hal_clock_init(void) {
    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4) {
    }

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    while(LL_PWR_IsActiveFlag_VOS() == 0) {
    }

    LL_RCC_HSE_Enable();
    while(LL_RCC_HSE_IsReady() != 1) {
    }

    LL_PWR_EnableBkUpAccess();
    while(LL_PWR_IsEnabledBkUpAccess() == 0U) {
    }

    LL_RCC_PLL1_ConfigDomain_SYS(LL_RCC_PLL1SOURCE_HSE, 5, 32, 1);
    LL_RCC_PLL1_EnableDomain_SYS();
    LL_RCC_SetPll1EPodPrescaler(LL_RCC_PLL1MBOOST_DIV_2);
    LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
    LL_RCC_PLL1_Enable();
    while(LL_RCC_PLL1_IsReady() != 1) {
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1) {
    }

    for(__IO uint32_t i = (CPU_CLOCK_PLL_HZ / 1000000U) >> 1; i != 0; i--) {
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

    LL_Init1msTick(CPU_CLOCK_PLL_HZ);
    LL_SetSystemCoreClock(CPU_CLOCK_PLL_HZ);
    LL_SYSTICK_EnableIT();
    NVIC_SetPriority(
        SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), TICK_INT_PRIORITY, 0));
    NVIC_EnableIRQ(SysTick_IRQn);
}

void furi_hal_clock_switch_hse2hsi(void) {
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1) {
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {
    }

    LL_SetSystemCoreClock(CPU_CLOCK_HSI_HZ);
    SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000U) - 1UL);
}

void furi_hal_clock_switch_hsi2hse(void) {
    LL_RCC_HSE_Enable();
    while(LL_RCC_HSE_IsReady() != 1) {
    }

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1) {
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE) {
    }

    LL_SetSystemCoreClock(CPU_CLOCK_HSE_HZ);
    SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000U) - 1UL);
}

bool furi_hal_clock_switch_hse2pll(void) {
    if(LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_PLL1) {
        return true;
    }

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4) {
    }

    LL_RCC_PLL1_Enable();
    while(LL_RCC_PLL1_IsReady() != 1) {
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1) {
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_SetSystemCoreClock(CPU_CLOCK_PLL_HZ);
    SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000U) - 1UL);

    return true;
}

bool furi_hal_clock_switch_pll2hse(void) {
    if(LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_HSE) {
        return true;
    }

    LL_RCC_HSE_Enable();
    while(LL_RCC_HSE_IsReady() != 1) {
    }

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE) {
    }

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_SetSystemCoreClock(CPU_CLOCK_HSE_HZ);
    SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000U) - 1UL);

    return true;
}

void furi_hal_clock_suspend_tick(void) {
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

void furi_hal_clock_resume_tick(void) {
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

void furi_hal_clock_mco_enable(FuriHalClockMcoSourceId source, FuriHalClockMcoDivisorId div) {
    uint32_t mco_source = LL_RCC_MCO1SOURCE_NOCLOCK;

    switch(source) {
    case FuriHalClockMcoLse:
        mco_source = LL_RCC_MCO1SOURCE_LSE;
        break;
    case FuriHalClockMcoSysclk:
        mco_source = LL_RCC_MCO1SOURCE_SYSCLK;
        break;
    default:
        mco_source = LL_RCC_MCO1SOURCE_MSIS;
        break;
    }

    LL_RCC_ConfigMCO(mco_source, div);
}

void furi_hal_clock_mco_disable(void) {
    LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_NOCLOCK, FuriHalClockMcoDiv1);
}
