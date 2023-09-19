#include "drunk_dashboard_view_pressure_i.h"
#include <malloc.h>
#include <gui/view.h>

DrunkDashboardViewPressure* drunk_dashboard_view_pressure_alloc() {
    DrunkDashboardViewPressure* app = malloc(sizeof(DrunkDashboardViewPressure));
    app->view = view_alloc();
    view_set_context(app->view, app);
    return app;
}

void drunk_dashboard_view_pressure_free(DrunkDashboardViewPressure* app) {
    view_free(app->view);
    free(app);
}

View* drunk_dashboard_view_pressure_get_view(DrunkDashboardViewPressure* app) {
    return app->view;
}
