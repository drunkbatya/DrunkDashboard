#include "dashboard_view_main.h"

#include <furi.h>

#include <stdlib.h>

struct DashboardViewMain {
    View* view;
    void* context;
};

typedef struct {
    uint8_t x;
    bool direction_forward;
} DashboardViewMainModel;

static void dashboard_view_main_draw_callback(Canvas* canvas, void* _model) {
    DashboardViewMainModel* model = _model;

    canvas_draw_str(canvas, 0, 10, "Helllowwww..ТестFuck");
    canvas_draw_box(canvas, model->x, 30, 10, 10);
}

DashboardViewMain* dashboard_view_main_view_alloc(void) {
    DashboardViewMain* dashboard_view_main = malloc(sizeof(DashboardViewMain));

    dashboard_view_main->view = view_alloc();
    view_allocate_model(
        dashboard_view_main->view, ViewModelTypeLocking, sizeof(DashboardViewMainModel));
    view_set_context(dashboard_view_main->view, dashboard_view_main);
    view_set_draw_callback(dashboard_view_main->view, dashboard_view_main_draw_callback);

    with_view_model(
        dashboard_view_main->view,
        DashboardViewMainModel * model,
        {
            model->x = 0;
            model->direction_forward = true;
        },
        true);
    //view_set_input_callback(dashboard_view_main->view, dashboard_view_main_input_callback);

    return dashboard_view_main;
}

void dashboard_view_main_view_free(DashboardViewMain* dashboard_view_main) {
    furi_assert(dashboard_view_main);
    view_free(dashboard_view_main->view);
    free(dashboard_view_main);
}

View* dashboard_view_main_get_view(DashboardViewMain* dashboard_view_main) {
    furi_assert(dashboard_view_main);
    return dashboard_view_main->view;
}

void dashboard_view_main_update(DashboardViewMain* dashboard_view_main) {
    with_view_model(
        dashboard_view_main->view,
        DashboardViewMainModel * model,
        {
            if(model->direction_forward == true) {
                model->x += 1;
            } else {
                model->x -= 1;
            }
            if(model->x == 200) {
                model->direction_forward = false;
            } else if(model->x == 0) {
                model->direction_forward = true;
            }
        },
        true);
}
