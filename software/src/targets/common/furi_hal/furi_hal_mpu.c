#include <furi_hal_mpu.h>
#include <stm32u5xx_ll_cortex.h>

#define FURI_HAL_MPU_ATTR_INDEX  LL_MPU_ATTRIBUTES_NUMBER0
#define FURI_HAL_MPU_ATTR_NORMAL ((LL_MPU_NOT_CACHEABLE << 4U) | LL_MPU_NOT_CACHEABLE)
#define FURI_HAL_MPU_REGION_ATTR_BASE \
    (LL_MPU_ACCESS_NOT_SHAREABLE | LL_MPU_INSTRUCTION_ACCESS_ENABLE)
#define FURI_HAL_MPU_STACK_PROTECT_REGION FuriHalMPURegionSize32B

static uint32_t furi_hal_mpu_size_to_bytes(FuriHalMPURegionSize size) {
    return 1UL << (size + 1U);
}

static uint32_t furi_hal_mpu_limit_address(uint32_t address, FuriHalMPURegionSize size) {
    return address + furi_hal_mpu_size_to_bytes(size) - 1U;
}

void furi_hal_mpu_init(void) {
    furi_hal_mpu_disable();
    LL_MPU_ConfigAttributes(FURI_HAL_MPU_ATTR_INDEX, FURI_HAL_MPU_ATTR_NORMAL);
    furi_hal_mpu_enable();

    furi_hal_mpu_protect_no_access(FuriHalMpuRegionNULL, 0x00, FuriHalMPURegionSize1MB);
}

void furi_hal_mpu_enable(void) {
    LL_MPU_Enable(LL_MPU_CTRL_PRIVILEGED_DEFAULT);
}

void furi_hal_mpu_disable(void) {
    LL_MPU_Disable();
}

void furi_hal_mpu_protect_no_access(
    FuriHalMpuRegion region,
    uint32_t address,
    FuriHalMPURegionSize size) {
    furi_hal_mpu_disable();
    LL_MPU_ConfigRegion(
        region,
        FURI_HAL_MPU_REGION_ATTR_BASE | LL_MPU_REGION_PRIV_RW | LL_MPU_INSTRUCTION_ACCESS_DISABLE,
        FURI_HAL_MPU_ATTR_INDEX,
        address,
        furi_hal_mpu_limit_address(address, size));
    furi_hal_mpu_enable();
}

void furi_hal_mpu_protect_read_only(
    FuriHalMpuRegion region,
    uint32_t address,
    FuriHalMPURegionSize size) {
    furi_hal_mpu_disable();
    LL_MPU_ConfigRegion(
        region,
        FURI_HAL_MPU_REGION_ATTR_BASE | LL_MPU_REGION_ALL_RO,
        FURI_HAL_MPU_ATTR_INDEX,
        address,
        furi_hal_mpu_limit_address(address, size));
    furi_hal_mpu_enable();
}

void furi_hal_mpu_protect_disable(FuriHalMpuRegion region) {
    furi_hal_mpu_disable();
    LL_MPU_DisableRegion(region);
    furi_hal_mpu_enable();
}

void furi_hal_mpu_set_stack_protection(uint32_t* stack) {
    uint32_t stack_ptr = (uint32_t)stack;
    uint32_t mask = furi_hal_mpu_size_to_bytes(FURI_HAL_MPU_STACK_PROTECT_REGION) - 1U;
    stack_ptr &= ~mask;
    if(stack_ptr < (uint32_t)stack) stack_ptr += mask + 1U;

    furi_hal_mpu_protect_read_only(
        FuriHalMpuRegionThreadStack, stack_ptr, FURI_HAL_MPU_STACK_PROTECT_REGION);
}
