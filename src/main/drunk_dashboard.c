#include "drunk_dashboard.h"
#include <gui/canvas.h>
#include <stdbool.h>
#include <halk/halk_delay.h>
#include <u8g2/u8g2_glue.h>

#include <stdlib.h>

DrunkDashboard* drunk_dashboard_alloc() {
    DrunkDashboard* drunk_dashboard = malloc(sizeof(DrunkDashboard));
    drunk_dashboard->scene_manager =
        scene_manager_alloc(&drunk_dashboard_scene_handlers, drunk_dashboard);
    drunk_dashboard->gui = gui_alloc();
    drunk_dashboard->view_pressure = drunk_dashboard_view_pressure_alloc();
    return drunk_dashboard;
}

void drunk_dashboard_free(DrunkDashboard* drunk_dashboard) {
    drunk_dashboard_view_pressure_free(drunk_dashboard->view_pressure);
    gui_free(drunk_dashboard->gui);
    scene_manager_free(drunk_dashboard->scene_manager);
    free(drunk_dashboard);
}

void drunk_dashboard_run(DrunkDashboard* drunk_dashboard) {
    uint16_t pressure = 200;
    gui_set_view_on_display(
        drunk_dashboard->gui,
        drunk_dashboard_view_pressure_get_view(drunk_dashboard->view_pressure),
        0);
    gui_draw(drunk_dashboard->gui);
    drunk_dashboard_view_pressure_set_pressure(drunk_dashboard->view_pressure, pressure);
    gui_set_view_on_display(
        drunk_dashboard->gui,
        drunk_dashboard_view_pressure_get_view(drunk_dashboard->view_pressure),
        1);
    gui_draw(drunk_dashboard->gui);
    drunk_dashboard_view_pressure_set_pressure(drunk_dashboard->view_pressure, pressure);
    gui_set_view_on_display(
        drunk_dashboard->gui,
        drunk_dashboard_view_pressure_get_view(drunk_dashboard->view_pressure),
        2);
    gui_draw(drunk_dashboard->gui);
    drunk_dashboard_view_pressure_set_pressure(drunk_dashboard->view_pressure, pressure);
    gui_set_view_on_display(
        drunk_dashboard->gui,
        drunk_dashboard_view_pressure_get_view(drunk_dashboard->view_pressure),
        3);
    gui_draw(drunk_dashboard->gui);
    drunk_dashboard_view_pressure_set_pressure(drunk_dashboard->view_pressure, pressure);
    while(true) {
        drunk_dashboard_view_pressure_set_pressure(drunk_dashboard->view_pressure, pressure);
        gui_draw(drunk_dashboard->gui);
        pressure++;
        if(pressure >= 350) pressure = 0;
    }
}