#include <furi_hal_cortex.h>
#include <furi.h>

#include <stm32u5xx.h>

#define FURI_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND (SystemCoreClock / 1000000)

void furi_hal_cortex_init_early(void) {
    CoreDebug->DEMCR |= (CoreDebug_DEMCR_TRCENA_Msk | CoreDebug_DEMCR_MON_EN_Msk);
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0U;

    /* Enable instruction prefetch */
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);
}

void furi_hal_cortex_delay_us(uint32_t microseconds) {
    furi_check(microseconds < (UINT32_MAX / FURI_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND));

    uint32_t start = DWT->CYCCNT;
    uint32_t time_ticks = FURI_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND * microseconds;

    while((DWT->CYCCNT - start) < time_ticks) {
    };
}

uint32_t furi_hal_cortex_instructions_per_microsecond(void) {
    return FURI_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND;
}

FURI_WARN_UNUSED FuriHalCortexTimer furi_hal_cortex_timer_get(uint32_t timeout_us) {
    furi_check(timeout_us < (UINT32_MAX / FURI_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND));

    FuriHalCortexTimer cortex_timer = {0};
    cortex_timer.start = DWT->CYCCNT;
    cortex_timer.value = FURI_HAL_CORTEX_INSTRUCTIONS_PER_MICROSECOND * timeout_us;
    return cortex_timer;
}

bool furi_hal_cortex_timer_is_expired(FuriHalCortexTimer cortex_timer) {
    return !((DWT->CYCCNT - cortex_timer.start) < cortex_timer.value);
}

void furi_hal_cortex_timer_wait(FuriHalCortexTimer cortex_timer) {
    while(!furi_hal_cortex_timer_is_expired(cortex_timer))
        ;
}

// Duck ST
#undef COMP0
#undef COMP1
#undef COMP2
#undef COMP3

void furi_hal_cortex_comp_enable(
    FuriHalCortexComp comp,
    FuriHalCortexCompFunction function,
    uint32_t value,
    uint32_t mask,
    FuriHalCortexCompSize size) {
    UNUSED(comp);
    UNUSED(function);
    UNUSED(value);
    UNUSED(mask);
    UNUSED(size);
}

void furi_hal_cortex_comp_reset(FuriHalCortexComp comp) {
    UNUSED(comp);
}
