#pragma once

#include <stdbool.h>

void system_assert(bool condition);
void system_check(bool condition);
void system_crash(const char* str);
