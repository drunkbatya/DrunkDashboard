#include <furi_hal_fdcan.h>

#include <furi.h>
#include <furi_hal_bus.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <stm32u5xx_ll_fdcan.h>
#include <stm32u5xx_ll_rcc.h>

#define TAG "FuriHalFdcan"

static LL_FDCAN_MessageRAMTypeDef furi_hal_fdcan1_message_ram;

static void furi_hal_fdcan_gpio_init(void) {
    furi_hal_gpio_init_ex(
        &gpio_fdcan1_rx, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn9FDCAN1);
    furi_hal_gpio_init_ex(
        &gpio_fdcan1_tx, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn9FDCAN1);
}

static void furi_hal_fdcan_gpio_deinit(void) {
    furi_hal_gpio_init_simple(&gpio_fdcan1_rx, GpioModeAnalog);
    furi_hal_gpio_init_simple(&gpio_fdcan1_tx, GpioModeAnalog);
}

void furi_hal_fdcan_init(void) {
    LL_RCC_SetFDCANClockSource(LL_RCC_FDCAN_CLKSOURCE_HSE);
    furi_hal_bus_enable(FuriHalBusFDCAN1);
    furi_hal_fdcan_gpio_init();

    LL_FDCAN_InitTypeDef fdcan_init = {
        .ClockDivider = LL_FDCAN_CLOCK_DIV1,
        .FrameFormat = LL_FDCAN_FRAME_CLASSIC,
        .Mode = LL_FDCAN_MODE_NORMAL,
        .AutoRetransmission = LL_FDCAN_AUTO_RETRANSMISSION_DISABLE,
        .TransmitPause = LL_FDCAN_TRANSMIT_PAUSE_DISABLE,
        .ProtocolException = LL_FDCAN_PROTOCOL_EXCEPTION_DISABLE,
        // HSE 25MHz / (5 * (1 + 7 + 2)) = 500 kbit/s
        .NominalPrescaler = 5,
        .NominalSyncJumpWidth = 2,
        .NominalTimeSeg1 = 7,
        .NominalTimeSeg2 = 2,
        .DataPrescaler = 1,
        .DataSyncJumpWidth = 1,
        .DataTimeSeg1 = 1,
        .DataTimeSeg2 = 1,
        .StdFiltersNbr = 0,
        .ExtFiltersNbr = 0,
        .TxFifoQueueMode = LL_FDCAN_TX_FIFO_OPERATION,
    };

    furi_check(LL_FDCAN_Init(FDCAN1, &fdcan_init, &furi_hal_fdcan1_message_ram) == SUCCESS);

    LL_FDCAN_ConfigGlobalFilter(
        FDCAN1,
        LL_FDCAN_ACCEPT_IN_RX_FIFO0,
        LL_FDCAN_ACCEPT_IN_RX_FIFO0,
        LL_FDCAN_FILTER_REMOTE,
        LL_FDCAN_FILTER_REMOTE);
    LL_FDCAN_SetRxFifoMode(FDCAN1, LL_FDCAN_RX_FIFO0, LL_FDCAN_RX_FIFO_BLOCKING);

    LL_FDCAN_SetITLine(FDCAN1, LL_FDCAN_IT_GROUP_RX_FIFO0, LL_FDCAN_INTERRUPT_LINE0);
    LL_FDCAN_EnableIT(
        FDCAN1,
        LL_FDCAN_IT_RX_FIFO0_NEW_MESSAGE | LL_FDCAN_IT_RX_FIFO0_FULL |
            LL_FDCAN_IT_RX_FIFO0_MESSAGE_LOST);
    LL_FDCAN_EnableITLine(FDCAN1, LL_FDCAN_INTERRUPT_LINE0);

    LL_FDCAN_RequestOperation(FDCAN1);
    while(LL_FDCAN_IsActiveFlag_INIT(FDCAN1) != 0U) {
        furi_delay_ms(1);
    }

    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_fdcan_deinit(void) {
    LL_FDCAN_DeInit(FDCAN1);
    furi_hal_bus_disable(FuriHalBusFDCAN1);
    furi_hal_fdcan_gpio_deinit();
}

bool furi_hal_fdcan_get_message(FuriHalFdcanMessage* message) {
    furi_check(message);

    LL_FDCAN_RxHeaderTypeDef header;
    if(LL_FDCAN_GetRxMessage(
           FDCAN1, &furi_hal_fdcan1_message_ram, LL_FDCAN_RX_FIFO0, &header, message->data) !=
       SUCCESS) {
        return false;
    }

    message->id = header.Identifier;
    message->data_size = LL_FDCAN_DLCToBytes(header.DataLength);

    return true;
}
