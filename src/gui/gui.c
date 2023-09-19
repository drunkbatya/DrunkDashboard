#include "gui_i.h"
#include <malloc.h>
#include <u8g2/u8g2_glue.h>

static void gui_canvases_alloc(Gui* gui) {
    for(uint8_t disp_number = 0; disp_number < gui->display_count; disp_number++) {
        gui->canvases[disp_number] = canvas_init(
            u8g2_4wire_hw_spi_stm32, U8G2_GLUE_GET_GPIO_CB_BY_DISP_NUMBER(disp_number));
    }
}

static void gui_canvases_free(Gui* gui) {
    for(uint8_t disp_number = 0; disp_number < gui->display_count; disp_number++) {
        canvas_free(gui->canvases[disp_number]);
    }
}

Gui* gui_alloc() {
    Gui* gui = malloc(sizeof(Gui));
    gui->display_count = U8G2_GLUE_TOTAL_DISPLAY_COUNT;
    gui->canvases = malloc(sizeof(Canvas*) * gui->display_count);
    gui->views = malloc(sizeof(View*) * gui->display_count);
    gui_canvases_alloc(gui);
    return gui;
}

void gui_free(Gui* gui) {
    gui_canvases_free(gui);
    free(gui->views);
    free(gui);
}

void gui_set_view_on_display(Gui* gui, View* view, uint8_t disp_number) {
    if(disp_number >= gui->display_count) return;
    gui->views[disp_number] = view;
}
void gui_remove_view_from_display(Gui* gui, uint8_t disp_number) {
    if(disp_number >= gui->display_count) return;
    gui->views[disp_number] = NULL;
}

View* gui_get_view_from_display(Gui* gui, uint8_t disp_number) {
    if(disp_number >= gui->display_count) return NULL;
    return gui->views[disp_number];
}

void gui_draw(Gui* gui) {
    for(uint8_t disp_number = 0; disp_number < gui->display_count; disp_number++) {
        canvas_reset(gui->canvases[disp_number]);
        if(gui->views[disp_number]) view_draw(gui->views[disp_number], gui->canvases[disp_number]);
        canvas_commit(gui->canvases[disp_number]);
    }
}
