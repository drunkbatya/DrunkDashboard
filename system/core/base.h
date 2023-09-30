#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SystemWaitForever = 0xFFFFFFFFU,
} SystemWait;

typedef enum {
    SystemFlagWaitAny = 0x00000000U, ///< Wait for any flag (default).
    SystemFlagWaitAll = 0x00000001U, ///< Wait for all flags.
    SystemFlagNoClear = 0x00000002U, ///< Do not clear flags which have been specified to wait for.

    SystemFlagError = 0x80000000U, ///< Error indicator.
    SystemFlagErrorUnknown = 0xFFFFFFFFU, ///< SystemStatusError (-1).
    SystemFlagErrorTimeout = 0xFFFFFFFEU, ///< SystemStatusErrorTimeout (-2).
    SystemFlagErrorResource = 0xFFFFFFFDU, ///< SystemStatusErrorResource (-3).
    SystemFlagErrorParameter = 0xFFFFFFFCU, ///< SystemStatusErrorParameter (-4).
    SystemFlagErrorISR = 0xFFFFFFFAU, ///< SystemStatusErrorISR (-6).
} SystemFlag;

typedef enum {
    SystemStatusOk = 0, ///< Operation completed successfully.
    SystemStatusError =
        -1, ///< Unspecified RTOS error: run-time error but no other error message fits.
    SystemStatusErrorTimeout = -2, ///< Operation not completed within the timeout period.
    SystemStatusErrorResource = -3, ///< Resource not available.
    SystemStatusErrorParameter = -4, ///< Parameter error.
    SystemStatusErrorNoMemory =
        -5, ///< System is out of memory: it was impossible to allocate or reserve memory for the operation.
    SystemStatusErrorISR =
        -6, ///< Not allowed in ISR context: the function cannot be called from interrupt service routines.
    SystemStatusReserved = 0x7FFFFFFF ///< Prevents enum down-size compiler optimization.
} SystemStatus;
