#pragma once

void* pvPortMalloc(size_t xWantedSize);
void vPortFree(void* pv);
size_t xPortGetFreeHeapSize(void);
size_t xPortGetTotalHeapSize(void);
