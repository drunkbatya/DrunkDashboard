#pragma once

#include "view.h"

struct View {
    ViewDrawCallback draw_callback;
    // ViewInputCallback input_callback;
    ViewCustomCallback custom_callback;
    void* context;
};
