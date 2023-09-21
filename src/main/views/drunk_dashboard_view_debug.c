#include "drunk_dashboard_view_debug_i.h"
#include <malloc.h>
#include <halk/halk.h>
#include <halk/halk_memmory.h>
#include <gui/view.h>
#include <stdio.h>

void drunk_dashboard_view_debug_draw_callback(Canvas* canvas, void* context) {
    DrunkDashboardViewDebug* app = context;
    UNUSED(app);
    char* str = malloc(sizeof(char) * 100);
    size_t heap_free = halk_memmory_get_free_heap();
    size_t heap_total = halk_memmory_get_total_heap();

    canvas_set_font(canvas, CanvasFontPrimary);
    sprintf(str, "Debug:");
    uint16_t str_width = canvas_get_string_width(canvas, str);
    uint8_t str_pos_x = (canvas_get_width(canvas) / 2) - (str_width / 2);
    canvas_draw_str(canvas, str_pos_x, 10, str);

    canvas_set_font(canvas, CanvasFontSecondary);
    sprintf(str, "Free heap: %u bytes", heap_free);
    canvas_draw_str(canvas, 0, 20, str);
    sprintf(str, "Total heap: %u bytes", heap_total);
    canvas_draw_str(canvas, 0, 30, str);

    free(str);
}

DrunkDashboardViewDebug* drunk_dashboard_view_debug_alloc() {
    DrunkDashboardViewDebug* app = malloc(sizeof(DrunkDashboardViewDebug));
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, drunk_dashboard_view_debug_draw_callback);
    return app;
}

void drunk_dashboard_view_debug_free(DrunkDashboardViewDebug* app) {
    view_free(app->view);
    free(app);
}

View* drunk_dashboard_view_debug_get_view(DrunkDashboardViewDebug* app) {
    return app->view;
}
