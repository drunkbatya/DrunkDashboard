#include "dashboard_view_dashboard.h"

#include <furi.h>

#include <stdlib.h>

struct DashboardViewDashboard {
    View* view;
    DashboardViewDashboardCallback callback;
    void* context;
};

typedef struct {
    uint8_t reserved;
} DashboardViewDashboardModel;

static void dashboard_view_dashboard_draw_callback(Canvas* canvas, void* _model) {
    UNUSED(_model);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 12, "Dashboard");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 30, "Sensors view");
    canvas_draw_str(canvas, 4, 44, "No data yet");
}

static bool dashboard_view_dashboard_input_callback(InputEvent* event, void* context) {
    DashboardViewDashboard* dashboard_view_dashboard = context;
    bool consumed = false;

    if(event->key == InputKeyOk && event->type == InputTypeShort) {
        if(dashboard_view_dashboard->callback) {
            dashboard_view_dashboard->callback(dashboard_view_dashboard->context, event->key);
        }
        consumed = true;
    }

    return consumed;
}

DashboardViewDashboard* dashboard_view_dashboard_alloc(void) {
    DashboardViewDashboard* dashboard_view_dashboard = malloc(sizeof(DashboardViewDashboard));

    dashboard_view_dashboard->view = view_alloc();
    dashboard_view_dashboard->callback = NULL;
    dashboard_view_dashboard->context = NULL;
    view_allocate_model(
        dashboard_view_dashboard->view, ViewModelTypeLocking, sizeof(DashboardViewDashboardModel));
    view_set_context(dashboard_view_dashboard->view, dashboard_view_dashboard);
    view_set_draw_callback(dashboard_view_dashboard->view, dashboard_view_dashboard_draw_callback);
    view_set_input_callback(
        dashboard_view_dashboard->view, dashboard_view_dashboard_input_callback);

    with_view_model(
        dashboard_view_dashboard->view,
        DashboardViewDashboardModel * model,
        { model->reserved = 0; },
        true);

    return dashboard_view_dashboard;
}

void dashboard_view_dashboard_free(DashboardViewDashboard* dashboard_view_dashboard) {
    furi_assert(dashboard_view_dashboard);
    view_free(dashboard_view_dashboard->view);
    free(dashboard_view_dashboard);
}

View* dashboard_view_dashboard_get_view(DashboardViewDashboard* dashboard_view_dashboard) {
    furi_assert(dashboard_view_dashboard);
    return dashboard_view_dashboard->view;
}

void dashboard_view_dashboard_set_callback(
    DashboardViewDashboard* dashboard_view_dashboard,
    DashboardViewDashboardCallback callback,
    void* context) {
    furi_assert(dashboard_view_dashboard);
    dashboard_view_dashboard->callback = callback;
    dashboard_view_dashboard->context = context;
}

void dashboard_view_dashboard_update(DashboardViewDashboard* dashboard_view_dashboard) {
    with_view_model(
        dashboard_view_dashboard->view,
        DashboardViewDashboardModel * model,
        { UNUSED(model); },
        true);
}
