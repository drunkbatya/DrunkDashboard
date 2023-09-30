#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <cmsis_compiler.h>

#if !defined(UNUSED)
#define UNUSED(x) ((void)(x))
#endif

#ifndef SYSTEM_IS_IRQ_MASKED
#define SYSTEM_IS_IRQ_MASKED() (__get_PRIMASK() != 0U)
#endif

#ifndef SYSTEM_IS_IRQ_MODE
#define SYSTEM_IS_IRQ_MODE() (__get_IPSR() != 0U)
#endif

#ifndef SYSTEM_IS_ISR
#define SYSTEM_IS_ISR() (SYSTEM_IS_IRQ_MODE() || SYSTEM_IS_IRQ_MASKED())
#endif
