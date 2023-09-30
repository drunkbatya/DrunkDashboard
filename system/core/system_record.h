#pragma once

#include <stdbool.h>

void system_record_init();
void system_record_create(const char* name, void* data);
bool system_record_destroy(const char* name);
void* system_record_open(const char* name);
void system_record_close(const char* name);
