#pragma once

#include "view.h"

typedef struct Gui Gui;

Gui* gui_alloc();
void gui_free(Gui* gui);
void gui_set_view_on_display(Gui* gui, View* view, uint8_t disp_number);
void gui_remove_view_from_display(Gui* gui, uint8_t disp_number);
View* gui_get_view_from_display(Gui* gui, uint8_t disp_number);
void gui_draw(Gui* gui);