#include "view_i.h"
#include <malloc.h>

View* view_alloc() {
    View* view = malloc(sizeof(View));
    return view;
}

void view_free(View* view) {
    free(view);
}

void view_set_draw_callback(View* view, ViewDrawCallback callback) {
    view->draw_callback = callback;
}

void view_set_custom_callback(View* view, ViewCustomCallback callback) {
    view->custom_callback = callback;
}

void view_set_context(View* view, void* context) {
    view->context = context;
}

void view_draw(View* view, Canvas* canvas) {
    if(view->draw_callback) {
        void* context = view->context;
        view->draw_callback(canvas, context);
    }
}
