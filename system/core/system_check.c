#include "system_check.h"
#include "system_core_defs.h"

#include <cmsis_compiler.h>

void system_assert(bool condition) {
    if(!condition) {
        __disable_irq();
        while(true);
    }
}

void system_check(bool condition) {
    system_assert(condition);
}

void system_crash(const char* str) {
    UNUSED(str);
    system_assert(false);
}
