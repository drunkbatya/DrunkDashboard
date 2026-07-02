#pragma once

#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int main(void);
extern void __libc_init_array(void);

void Default_Handler(void);

#define DEFAULT FURI_DEFAULT("Default_Handler")

DEFAULT void NMI_Handler(void);
DEFAULT void HardFault_Handler(void);
DEFAULT void MemManage_Handler(void);
DEFAULT void BusFault_Handler(void);
DEFAULT void UsageFault_Handler(void);
DEFAULT void SecureFault_Handler(void);
DEFAULT void SVC_Handler(void);
DEFAULT void DebugMon_Handler(void);
DEFAULT void PendSV_Handler(void);
DEFAULT void SysTick_Handler(void);

DEFAULT void WWDG_IRQHandler(void);
DEFAULT void PVD_PVM_IRQHandler(void);
DEFAULT void RTC_IRQHandler(void);
DEFAULT void RTC_S_IRQHandler(void);
DEFAULT void TAMP_IRQHandler(void);
DEFAULT void RAMCFG_IRQHandler(void);
DEFAULT void FLASH_IRQHandler(void);
DEFAULT void FLASH_S_IRQHandler(void);
DEFAULT void GTZC_IRQHandler(void);
DEFAULT void RCC_IRQHandler(void);
DEFAULT void RCC_S_IRQHandler(void);
DEFAULT void EXTI0_IRQHandler(void);
DEFAULT void EXTI1_IRQHandler(void);
DEFAULT void EXTI2_IRQHandler(void);
DEFAULT void EXTI3_IRQHandler(void);
DEFAULT void EXTI4_IRQHandler(void);
DEFAULT void EXTI5_IRQHandler(void);
DEFAULT void EXTI6_IRQHandler(void);
DEFAULT void EXTI7_IRQHandler(void);
DEFAULT void EXTI8_IRQHandler(void);
DEFAULT void EXTI9_IRQHandler(void);
DEFAULT void EXTI10_IRQHandler(void);
DEFAULT void EXTI11_IRQHandler(void);
DEFAULT void EXTI12_IRQHandler(void);
DEFAULT void EXTI13_IRQHandler(void);
DEFAULT void EXTI14_IRQHandler(void);
DEFAULT void EXTI15_IRQHandler(void);
DEFAULT void IWDG_IRQHandler(void);
DEFAULT void SAES_IRQHandler(void);
DEFAULT void GPDMA1_Channel0_IRQHandler(void);
DEFAULT void GPDMA1_Channel1_IRQHandler(void);
DEFAULT void GPDMA1_Channel2_IRQHandler(void);
DEFAULT void GPDMA1_Channel3_IRQHandler(void);
DEFAULT void GPDMA1_Channel4_IRQHandler(void);
DEFAULT void GPDMA1_Channel5_IRQHandler(void);
DEFAULT void GPDMA1_Channel6_IRQHandler(void);
DEFAULT void GPDMA1_Channel7_IRQHandler(void);
DEFAULT void ADC1_IRQHandler(void);
DEFAULT void DAC1_IRQHandler(void);
DEFAULT void FDCAN1_IT0_IRQHandler(void);
DEFAULT void FDCAN1_IT1_IRQHandler(void);
DEFAULT void TIM1_BRK_IRQHandler(void);
DEFAULT void TIM1_UP_IRQHandler(void);
DEFAULT void TIM1_TRG_COM_IRQHandler(void);
DEFAULT void TIM1_CC_IRQHandler(void);
DEFAULT void TIM2_IRQHandler(void);
DEFAULT void TIM3_IRQHandler(void);
DEFAULT void TIM4_IRQHandler(void);
DEFAULT void TIM5_IRQHandler(void);
DEFAULT void TIM6_IRQHandler(void);
DEFAULT void TIM7_IRQHandler(void);
DEFAULT void TIM8_BRK_IRQHandler(void);
DEFAULT void TIM8_UP_IRQHandler(void);
DEFAULT void TIM8_TRG_COM_IRQHandler(void);
DEFAULT void TIM8_CC_IRQHandler(void);
DEFAULT void I2C1_EV_IRQHandler(void);
DEFAULT void I2C1_ER_IRQHandler(void);
DEFAULT void I2C2_EV_IRQHandler(void);
DEFAULT void I2C2_ER_IRQHandler(void);
DEFAULT void SPI1_IRQHandler(void);
DEFAULT void SPI2_IRQHandler(void);
DEFAULT void USART1_IRQHandler(void);
DEFAULT void USART2_IRQHandler(void);
DEFAULT void USART3_IRQHandler(void);
DEFAULT void UART4_IRQHandler(void);
DEFAULT void UART5_IRQHandler(void);
DEFAULT void LPUART1_IRQHandler(void);
DEFAULT void LPTIM1_IRQHandler(void);
DEFAULT void LPTIM2_IRQHandler(void);
DEFAULT void TIM15_IRQHandler(void);
DEFAULT void TIM16_IRQHandler(void);
DEFAULT void TIM17_IRQHandler(void);
DEFAULT void COMP_IRQHandler(void);
DEFAULT void OTG_FS_IRQHandler(void);
DEFAULT void CRS_IRQHandler(void);
DEFAULT void FMC_IRQHandler(void);
DEFAULT void OCTOSPI1_IRQHandler(void);
DEFAULT void PWR_S3WU_IRQHandler(void);
DEFAULT void SDMMC1_IRQHandler(void);
DEFAULT void SDMMC2_IRQHandler(void);
DEFAULT void GPDMA1_Channel8_IRQHandler(void);
DEFAULT void GPDMA1_Channel9_IRQHandler(void);
DEFAULT void GPDMA1_Channel10_IRQHandler(void);
DEFAULT void GPDMA1_Channel11_IRQHandler(void);
DEFAULT void GPDMA1_Channel12_IRQHandler(void);
DEFAULT void GPDMA1_Channel13_IRQHandler(void);
DEFAULT void GPDMA1_Channel14_IRQHandler(void);
DEFAULT void GPDMA1_Channel15_IRQHandler(void);
DEFAULT void I2C3_EV_IRQHandler(void);
DEFAULT void I2C3_ER_IRQHandler(void);
DEFAULT void SAI1_IRQHandler(void);
DEFAULT void SAI2_IRQHandler(void);
DEFAULT void TSC_IRQHandler(void);
DEFAULT void AES_IRQHandler(void);
DEFAULT void RNG_IRQHandler(void);
DEFAULT void FPU_IRQHandler(void);
DEFAULT void HASH_IRQHandler(void);
DEFAULT void PKA_IRQHandler(void);
DEFAULT void LPTIM3_IRQHandler(void);
DEFAULT void SPI3_IRQHandler(void);
DEFAULT void I2C4_ER_IRQHandler(void);
DEFAULT void I2C4_EV_IRQHandler(void);
DEFAULT void MDF1_FLT0_IRQHandler(void);
DEFAULT void MDF1_FLT1_IRQHandler(void);
DEFAULT void MDF1_FLT2_IRQHandler(void);
DEFAULT void MDF1_FLT3_IRQHandler(void);
DEFAULT void UCPD1_IRQHandler(void);
DEFAULT void ICACHE_IRQHandler(void);
DEFAULT void OTFDEC1_IRQHandler(void);
DEFAULT void OTFDEC2_IRQHandler(void);
DEFAULT void LPTIM4_IRQHandler(void);
DEFAULT void DCACHE1_IRQHandler(void);
DEFAULT void ADF1_IRQHandler(void);
DEFAULT void ADC4_IRQHandler(void);
DEFAULT void LPDMA1_Channel0_IRQHandler(void);
DEFAULT void LPDMA1_Channel1_IRQHandler(void);
DEFAULT void LPDMA1_Channel2_IRQHandler(void);
DEFAULT void LPDMA1_Channel3_IRQHandler(void);
DEFAULT void DMA2D_IRQHandler(void);
DEFAULT void DCMI_PSSI_IRQHandler(void);
DEFAULT void OCTOSPI2_IRQHandler(void);
DEFAULT void MDF1_FLT4_IRQHandler(void);
DEFAULT void MDF1_FLT5_IRQHandler(void);
DEFAULT void CORDIC_IRQHandler(void);
DEFAULT void FMAC_IRQHandler(void);
DEFAULT void LSECSSD_IRQHandler(void);

#undef DEFAULT

#ifdef __cplusplus
}
#endif
