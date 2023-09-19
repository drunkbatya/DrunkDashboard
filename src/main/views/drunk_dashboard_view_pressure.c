#include "drunk_dashboard_view_pressure_i.h"
#include <malloc.h>
#include <halk/halk.h>
#include <gui/view.h>
#include <stdio.h>

void drunk_dashboard_view_pressure_draw_callback(Canvas* canvas, void* context) {
    char* str = malloc(sizeof(char) * 100);
    DrunkDashboardViewPressure* app = context;
    sprintf(str, "%u.%.2u", app->pressure / 100, app->pressure % 100);
    // canvas_draw_frame(canvas, 0, 0, canvas_get_width(canvas), canvas_get_height(canvas));
    canvas_draw_icon(canvas, 4, 8, &I_Turbocharger_59_48);
    canvas_set_font(canvas, CanvasFontPrimary);
    canvas_draw_str(canvas, 75, 10, "Pressure");
    canvas_set_font(canvas, CanvasFontBigNumbers);
    canvas_draw_str(canvas, 57, 52, str);
    canvas_set_font(canvas, CanvasFontSecondary);
    canvas_draw_str(canvas, 105, 51, "bar");
    free(str);
}

DrunkDashboardViewPressure* drunk_dashboard_view_pressure_alloc() {
    DrunkDashboardViewPressure* app = malloc(sizeof(DrunkDashboardViewPressure));
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, drunk_dashboard_view_pressure_draw_callback);

    app->pressure = 12;
    return app;
}

void drunk_dashboard_view_pressure_free(DrunkDashboardViewPressure* app) {
    view_free(app->view);
    free(app);
}

View* drunk_dashboard_view_pressure_get_view(DrunkDashboardViewPressure* app) {
    return app->view;
}

void drunk_dashboard_view_pressure_set_pressure(DrunkDashboardViewPressure* app, uint16_t pressure) {
    app->pressure = pressure;
}
