#include <furi_hal_bus.h>

#include <furi.h>
#include <stm32u5xx_ll_bus.h>

typedef enum {
    FuriHalBusDomainNone,
    FuriHalBusDomainAhb1,
    FuriHalBusDomainAhb2,
    FuriHalBusDomainAhb3,
    FuriHalBusDomainApb1Grp1,
    FuriHalBusDomainApb1Grp2,
    FuriHalBusDomainApb2,
    FuriHalBusDomainApb3,
} FuriHalBusDomain;

typedef struct {
    FuriHalBusDomain domain;
    uint32_t mask;
} FuriHalBusMapping;

static const FuriHalBusMapping furi_hal_bus_map[FuriHalBusMAX] = {
    [FuriHalBusDMA1] = {FuriHalBusDomainAhb1, LL_AHB1_GRP1_PERIPH_GPDMA1},
    [FuriHalBusDMA2] = {FuriHalBusDomainAhb3, LL_AHB3_GRP1_PERIPH_LPDMA1},
    [FuriHalBusCRC] = {FuriHalBusDomainAhb1, LL_AHB1_GRP1_PERIPH_CRC},
    [FuriHalBusTSC] = {FuriHalBusDomainAhb1, LL_AHB1_GRP1_PERIPH_TSC},

    [FuriHalBusGPIOA] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_GPIOA},
    [FuriHalBusGPIOB] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_GPIOB},
    [FuriHalBusGPIOC] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_GPIOC},
    [FuriHalBusGPIOD] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_GPIOD},
    [FuriHalBusGPIOE] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_GPIOE},
    [FuriHalBusGPIOH] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_GPIOH},
    [FuriHalBusADC] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_ADC12},
    [FuriHalBusAES1] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_AES},

    [FuriHalBusPKA] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_PKA},
    [FuriHalBusAES2] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_AES},
    [FuriHalBusRNG] = {FuriHalBusDomainAhb2, LL_AHB2_GRP1_PERIPH_RNG},
    [FuriHalBusFLASH] = {FuriHalBusDomainAhb1, LL_AHB1_GRP1_PERIPH_FLASH},

    [FuriHalBusTIM2] = {FuriHalBusDomainApb1Grp1, LL_APB1_GRP1_PERIPH_TIM2},
    [FuriHalBusTIM3] = {FuriHalBusDomainApb1Grp1, LL_APB1_GRP1_PERIPH_TIM3},
    [FuriHalBusTIM5] = {FuriHalBusDomainApb1Grp1, LL_APB1_GRP1_PERIPH_TIM5},
    [FuriHalBusSPI2] = {FuriHalBusDomainApb1Grp1, LL_APB1_GRP1_PERIPH_SPI2},
    [FuriHalBusI2C1] = {FuriHalBusDomainApb1Grp1, LL_APB1_GRP1_PERIPH_I2C1},
    [FuriHalBusI2C3] = {FuriHalBusDomainApb3, LL_APB3_GRP1_PERIPH_I2C3},
    [FuriHalBusCRS] = {FuriHalBusDomainApb1Grp1, LL_APB1_GRP1_PERIPH_CRS},
    [FuriHalBusLPTIM1] = {FuriHalBusDomainApb3, LL_APB3_GRP1_PERIPH_LPTIM1},

    [FuriHalBusLPUART1] = {FuriHalBusDomainApb3, LL_APB3_GRP1_PERIPH_LPUART1},
    [FuriHalBusLPTIM2] = {FuriHalBusDomainApb1Grp2, LL_APB1_GRP2_PERIPH_LPTIM2},
    [FuriHalBusFDCAN1] = {FuriHalBusDomainApb1Grp2, LL_APB1_GRP2_PERIPH_FDCAN1},

    [FuriHalBusTIM1] = {FuriHalBusDomainApb2, LL_APB2_GRP1_PERIPH_TIM1},
    [FuriHalBusTIM8] = {FuriHalBusDomainApb2, LL_APB2_GRP1_PERIPH_TIM8},
    [FuriHalBusSPI1] = {FuriHalBusDomainApb2, LL_APB2_GRP1_PERIPH_SPI1},
    [FuriHalBusUSART1] = {FuriHalBusDomainApb2, LL_APB2_GRP1_PERIPH_USART1},
    [FuriHalBusTIM16] = {FuriHalBusDomainApb2, LL_APB2_GRP1_PERIPH_TIM16},
    [FuriHalBusTIM17] = {FuriHalBusDomainApb2, LL_APB2_GRP1_PERIPH_TIM17},
    [FuriHalBusSAI1] = {FuriHalBusDomainApb2, LL_APB2_GRP1_PERIPH_SAI1},
};

static inline FuriHalBusMapping furi_hal_bus_get(FuriHalBus bus) {
    furi_check(bus < FuriHalBusMAX);
    return furi_hal_bus_map[bus];
}

void furi_hal_bus_init_early(void) {
}

void furi_hal_bus_deinit_early(void) {
}

void furi_hal_bus_enable(FuriHalBus bus) {
    const FuriHalBusMapping mapping = furi_hal_bus_get(bus);
    if(mapping.domain == FuriHalBusDomainNone) return;

    FURI_CRITICAL_ENTER();
    switch(mapping.domain) {
    case FuriHalBusDomainAhb1:
        LL_AHB1_GRP1_EnableClock(mapping.mask);
        LL_AHB1_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainAhb2:
        LL_AHB2_GRP1_EnableClock(mapping.mask);
        LL_AHB2_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainAhb3:
        LL_AHB3_GRP1_EnableClock(mapping.mask);
        LL_AHB3_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainApb1Grp1:
        LL_APB1_GRP1_EnableClock(mapping.mask);
        LL_APB1_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainApb1Grp2:
        LL_APB1_GRP2_EnableClock(mapping.mask);
        LL_APB1_GRP2_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainApb2:
        LL_APB2_GRP1_EnableClock(mapping.mask);
        LL_APB2_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainApb3:
        LL_APB3_GRP1_EnableClock(mapping.mask);
        LL_APB3_GRP1_ReleaseReset(mapping.mask);
        break;
    default:
        break;
    }
    FURI_CRITICAL_EXIT();
}

void furi_hal_bus_reset(FuriHalBus bus) {
    const FuriHalBusMapping mapping = furi_hal_bus_get(bus);
    if(mapping.domain == FuriHalBusDomainNone) return;

    FURI_CRITICAL_ENTER();
    switch(mapping.domain) {
    case FuriHalBusDomainAhb1:
        LL_AHB1_GRP1_ForceReset(mapping.mask);
        LL_AHB1_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainAhb2:
        LL_AHB2_GRP1_ForceReset(mapping.mask);
        LL_AHB2_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainAhb3:
        LL_AHB3_GRP1_ForceReset(mapping.mask);
        LL_AHB3_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainApb1Grp1:
        LL_APB1_GRP1_ForceReset(mapping.mask);
        LL_APB1_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainApb1Grp2:
        LL_APB1_GRP2_ForceReset(mapping.mask);
        LL_APB1_GRP2_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainApb2:
        LL_APB2_GRP1_ForceReset(mapping.mask);
        LL_APB2_GRP1_ReleaseReset(mapping.mask);
        break;
    case FuriHalBusDomainApb3:
        LL_APB3_GRP1_ForceReset(mapping.mask);
        LL_APB3_GRP1_ReleaseReset(mapping.mask);
        break;
    default:
        break;
    }
    FURI_CRITICAL_EXIT();
}

void furi_hal_bus_disable(FuriHalBus bus) {
    const FuriHalBusMapping mapping = furi_hal_bus_get(bus);
    if(mapping.domain == FuriHalBusDomainNone) return;

    FURI_CRITICAL_ENTER();
    switch(mapping.domain) {
    case FuriHalBusDomainAhb1:
        LL_AHB1_GRP1_ForceReset(mapping.mask);
        LL_AHB1_GRP1_DisableClock(mapping.mask);
        break;
    case FuriHalBusDomainAhb2:
        LL_AHB2_GRP1_ForceReset(mapping.mask);
        LL_AHB2_GRP1_DisableClock(mapping.mask);
        break;
    case FuriHalBusDomainAhb3:
        LL_AHB3_GRP1_ForceReset(mapping.mask);
        LL_AHB3_GRP1_DisableClock(mapping.mask);
        break;
    case FuriHalBusDomainApb1Grp1:
        LL_APB1_GRP1_ForceReset(mapping.mask);
        LL_APB1_GRP1_DisableClock(mapping.mask);
        break;
    case FuriHalBusDomainApb1Grp2:
        LL_APB1_GRP2_ForceReset(mapping.mask);
        LL_APB1_GRP2_DisableClock(mapping.mask);
        break;
    case FuriHalBusDomainApb2:
        LL_APB2_GRP1_ForceReset(mapping.mask);
        LL_APB2_GRP1_DisableClock(mapping.mask);
        break;
    case FuriHalBusDomainApb3:
        LL_APB3_GRP1_ForceReset(mapping.mask);
        LL_APB3_GRP1_DisableClock(mapping.mask);
        break;
    default:
        break;
    }
    FURI_CRITICAL_EXIT();
}

bool furi_hal_bus_is_enabled(FuriHalBus bus) {
    const FuriHalBusMapping mapping = furi_hal_bus_get(bus);
    if(mapping.domain == FuriHalBusDomainNone) return false;

    switch(mapping.domain) {
    case FuriHalBusDomainAhb1:
        return LL_AHB1_GRP1_IsEnabledClock(mapping.mask);
    case FuriHalBusDomainAhb2:
        return LL_AHB2_GRP1_IsEnabledClock(mapping.mask);
    case FuriHalBusDomainAhb3:
        return LL_AHB3_GRP1_IsEnabledClock(mapping.mask);
    case FuriHalBusDomainApb1Grp1:
        return LL_APB1_GRP1_IsEnabledClock(mapping.mask);
    case FuriHalBusDomainApb1Grp2:
        return LL_APB1_GRP2_IsEnabledClock(mapping.mask);
    case FuriHalBusDomainApb2:
        return LL_APB2_GRP1_IsEnabledClock(mapping.mask);
    case FuriHalBusDomainApb3:
        return LL_APB3_GRP1_IsEnabledClock(mapping.mask);
    default:
        return false;
    }
}
