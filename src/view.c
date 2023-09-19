#include "view_i.h"
#include <malloc.h>

View* view_alloc() {
    View* view = malloc(sizeof(View));
    return view;
}

void view_free(View* view) {
    free(view->model);
    free(view);
}
