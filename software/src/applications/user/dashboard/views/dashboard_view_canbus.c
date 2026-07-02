#include "dashboard_view_canbus.h"

#include <furi.h>

#include <stdlib.h>

struct DashboardViewCanbus {
    View* view;
    void* context;
};

typedef struct {
    DashboardCanFrame frame;
    bool has_frame;
} DashboardViewCanbusModel;

static void dashboard_view_canbus_draw_callback(Canvas* canvas, void* _model) {
    DashboardViewCanbusModel* model = _model;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    if(!model->has_frame) {
        canvas_draw_str(canvas, 0, 12, "CAN ID not found");
        return;
    }

    char str[64];

    snprintf(str, sizeof(str), "ID 0x%03lX", (unsigned long)model->frame.id);
    canvas_draw_str(canvas, 0, 10, str);

    canvas_set_font(canvas, FontSecondary);

    snprintf(
        str,
        sizeof(str),
        "Count: %lu  Len: %lu",
        (unsigned long)model->frame.count,
        (unsigned long)model->frame.last_len);
    canvas_draw_str(canvas, 0, 24, str);

    char* out = str;
    size_t remain = sizeof(str);
    int written = snprintf(out, remain, "Data:");
    out += written;
    remain -= written;

    for(uint32_t i = 0; (i < model->frame.last_len) && (i < 8U) && (remain > 4U); i++) {
        written = snprintf(out, remain, " %02X", model->frame.last_data[i]);
        out += written;
        remain -= written;
    }
    canvas_draw_str(canvas, 0, 38, str);

    out = str;
    remain = sizeof(str);
    written = snprintf(out, remain, "     ");
    out += written;
    remain -= written;

    for(uint32_t i = 8U; (i < model->frame.last_len) && (i < 16U) && (remain > 4U); i++) {
        written = snprintf(out, remain, " %02X", model->frame.last_data[i]);
        out += written;
        remain -= written;
    }
    if(model->frame.last_len > 8U) {
        canvas_draw_str(canvas, 0, 52, str);
    }
}

DashboardViewCanbus* dashboard_view_canbus_view_alloc(void) {
    DashboardViewCanbus* dashboard_view_canbus = malloc(sizeof(DashboardViewCanbus));

    dashboard_view_canbus->view = view_alloc();
    view_allocate_model(
        dashboard_view_canbus->view, ViewModelTypeLocking, sizeof(DashboardViewCanbusModel));
    view_set_context(dashboard_view_canbus->view, dashboard_view_canbus);
    view_set_draw_callback(dashboard_view_canbus->view, dashboard_view_canbus_draw_callback);

    with_view_model(
        dashboard_view_canbus->view,
        DashboardViewCanbusModel * model,
        { model->has_frame = false; },
        true);

    return dashboard_view_canbus;
}

void dashboard_view_canbus_view_free(DashboardViewCanbus* dashboard_view_canbus) {
    furi_assert(dashboard_view_canbus);
    view_free(dashboard_view_canbus->view);
    free(dashboard_view_canbus);
}

View* dashboard_view_canbus_get_view(DashboardViewCanbus* dashboard_view_canbus) {
    furi_assert(dashboard_view_canbus);
    return dashboard_view_canbus->view;
}

void dashboard_view_canbus_update(
    DashboardViewCanbus* dashboard_view_canbus,
    const DashboardCanFrame* frame) {
    with_view_model(
        dashboard_view_canbus->view,
        DashboardViewCanbusModel * model,
        {
            if(frame) {
                model->frame = *frame;
                model->has_frame = true;
            } else {
                model->has_frame = false;
            }
        },
        true);
}
