#include "drunk_dashboard.h"
#include "drunk_dashboard_system.h"
#include <gui/canvas.h>
#include <stdbool.h>
#include <halk/halk_delay.h>
#include <u8g2/u8g2_glue.h>
#include <cmsis_os2.h>

#include <stdlib.h>

static DrunkDashboard* drunk_dashboard_alloc() {
    DrunkDashboard* drunk_dashboard = malloc(sizeof(DrunkDashboard));
    drunk_dashboard->scene_manager =
        scene_manager_alloc(&drunk_dashboard_scene_handlers, drunk_dashboard);
    drunk_dashboard->gui = gui_alloc();
    drunk_dashboard->view_pressure = drunk_dashboard_view_pressure_alloc();
    drunk_dashboard->view_debug = drunk_dashboard_view_debug_alloc();
    return drunk_dashboard;
}

static void drunk_dashboard_free(DrunkDashboard* drunk_dashboard) {
    drunk_dashboard_view_debug_free(drunk_dashboard->view_debug);
    drunk_dashboard_view_pressure_free(drunk_dashboard->view_pressure);
    gui_free(drunk_dashboard->gui);
    scene_manager_free(drunk_dashboard->scene_manager);
    free(drunk_dashboard);
}

static void drunk_dashboard_thread(void* context) {
    DrunkDashboard* drunk_dashboard = context;
    scene_manager_next_scene(drunk_dashboard->scene_manager, DrunkDashboardSceneMain);
    uint16_t pressure = 200;
    while(true) {
        drunk_dashboard_view_pressure_set_pressure(drunk_dashboard->view_pressure, pressure);
        gui_draw(drunk_dashboard->gui);
        pressure++;
        if(pressure >= 350) pressure = 0;
    }
}

static void init_thread() {
    DrunkDashboard* drunk_dashboard = drunk_dashboard_alloc();
    drunk_dashboard_thread(drunk_dashboard);
    drunk_dashboard_free(drunk_dashboard);
}

void drunk_dashboard_init() {
    drunk_dashboard_system_init();

    const osThreadAttr_t init_thread_attributes = {
        .name = "Init",
        .stack_size = 2048,
        .priority = (osPriority_t)osPriorityNormal,
    };
    osThreadId_t testTaskHandle = osThreadNew(init_thread, NULL, &init_thread_attributes);

    drunk_dashboard_system_start_kernel();
}
