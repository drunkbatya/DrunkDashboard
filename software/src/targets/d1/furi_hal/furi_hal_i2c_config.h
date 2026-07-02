#pragma once

#include <furi_hal_i2c_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** I2C1 bus, under reset when not used */
extern FuriHalI2cBus furi_hal_i2c_bus_i2c1;

/** Handle for I2C1 bus
 * Bus: furi_hal_i2c_bus_i2c1
 * Pins: PB8(SCL) / PB9(SDA), float on release
 * Params: CubeMX timing 0x30909DEC
 */
extern const FuriHalI2cBusHandle furi_hal_i2c_handle_i2c1;

#ifdef __cplusplus
}
#endif
