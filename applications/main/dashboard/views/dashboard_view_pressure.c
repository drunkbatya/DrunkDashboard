#include "dashboard_view_pressure_i.h"
#include <malloc.h>
#include <system_core_defs.h>
#include <gui/view.h>
#include <stdio.h>

void dashboard_view_pressure_draw_callback(Canvas* canvas, void* context) {
    char str[30];
    DashboardViewPressure* app = context;
    sprintf(str, "%u.%.2u", app->pressure / 100, app->pressure % 100);
    // canvas_draw_frame(canvas, 0, 0, canvas_get_width(canvas), canvas_get_height(canvas));
    canvas_draw_icon(canvas, 4, 8, &I_Turbocharger_59_48);
    canvas_set_font(canvas, CanvasFontPrimary);
    canvas_draw_str(canvas, 75, 10, "Pressure");
    canvas_set_font(canvas, CanvasFontBigNumbers);
    canvas_draw_str(canvas, 57, 52, str);
    canvas_set_font(canvas, CanvasFontSecondary);
    canvas_draw_str(canvas, 105, 51, "bar");
}

DashboardViewPressure* dashboard_view_pressure_alloc() {
    DashboardViewPressure* app = malloc(sizeof(DashboardViewPressure));
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, dashboard_view_pressure_draw_callback);

    app->pressure = 12;
    return app;
}

void dashboard_view_pressure_free(DashboardViewPressure* app) {
    view_free(app->view);
    free(app);
}

View* dashboard_view_pressure_get_view(DashboardViewPressure* app) {
    return app->view;
}

void dashboard_view_pressure_set_pressure(DashboardViewPressure* app, uint16_t pressure) {
    app->pressure = pressure;
}
