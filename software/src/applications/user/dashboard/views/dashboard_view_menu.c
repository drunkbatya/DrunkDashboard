#include "dashboard_view_menu.h"

#include <furi.h>
#include <gui/elements.h>
#include <m-array.h>

#include <stdlib.h>

typedef struct {
    const char* label;
    const Icon* icon;
    uint32_t index;
} DashboardViewMenuItem;

ARRAY_DEF(DashboardViewMenuItemArray, DashboardViewMenuItem, M_POD_OPLIST);

struct DashboardViewMenu {
    View* view;
    DashboardViewMenuCallback callback;
    void* context;
};

typedef struct {
    DashboardViewMenuItemArray_t items;
    size_t position;
} DashboardViewMenuModel;

#define DASHBOARD_VIEW_MENU_COLUMNS 4

static void dashboard_view_menu_draw_tile(
    Canvas* canvas,
    const DashboardViewMenuItem* item,
    size_t column,
    size_t row,
    size_t tile_width,
    size_t tile_height,
    size_t gap,
    bool selected) {
    const size_t x = gap + column * (tile_width + gap);
    const size_t y = row * tile_height;

    if(selected) {
        elements_slightly_rounded_box(canvas, x, y, tile_width, tile_height - 2);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, x, y, tile_width, tile_height - 2);
        canvas_set_color(canvas, ColorBlack);
    }

    canvas_draw_icon(canvas, x + (tile_width - icon_get_width(item->icon)) / 2, y + 3, item->icon);
    canvas_set_font(canvas, FontSecondary);
    FuriString* text = furi_string_alloc_set(item->label);
    elements_string_fit_width(canvas, text, tile_width - 4);
    canvas_draw_str_aligned(
        canvas,
        x + tile_width / 2,
        y + tile_height - 5,
        AlignCenter,
        AlignBottom,
        furi_string_get_cstr(text));
    furi_string_free(text);

    canvas_set_color(canvas, ColorBlack);
}

static void dashboard_view_menu_draw_callback(Canvas* canvas, void* _model) {
    DashboardViewMenuModel* model = _model;
    canvas_clear(canvas);

    size_t items_count = DashboardViewMenuItemArray_size(model->items);
    size_t gap = 2;
    size_t tile_width = (canvas_width(canvas) - gap * (DASHBOARD_VIEW_MENU_COLUMNS + 1)) /
                        DASHBOARD_VIEW_MENU_COLUMNS;
    size_t tile_height = 32;

    for(size_t i = 0; i < items_count; i++) {
        dashboard_view_menu_draw_tile(
            canvas,
            DashboardViewMenuItemArray_get(model->items, i),
            i % DASHBOARD_VIEW_MENU_COLUMNS,
            i / DASHBOARD_VIEW_MENU_COLUMNS,
            tile_width,
            tile_height,
            gap,
            i == model->position);
    }
}

static void dashboard_view_menu_set_position(DashboardViewMenu* menu, size_t position) {
    with_view_model(
        menu->view,
        DashboardViewMenuModel * model,
        {
            size_t items_count = DashboardViewMenuItemArray_size(model->items);
            model->position = items_count ? position % items_count : 0;
        },
        true);
}

static bool dashboard_view_menu_input_callback(InputEvent* event, void* context) {
    DashboardViewMenu* menu = context;
    bool consumed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        DashboardViewMenuModel* model = view_get_model(menu->view);
        size_t position = model->position;
        size_t items_count = DashboardViewMenuItemArray_size(model->items);
        view_commit_model(menu->view, false);

        if(items_count == 0) return false;

        switch(event->key) {
        case InputKeyLeft:
            if(position > 0) {
                position--;
            } else {
                position = items_count - 1;
            }
            consumed = true;
            break;
        case InputKeyRight:
            if(position < items_count - 1) {
                position++;
            } else {
                position = 0;
            }
            consumed = true;
            break;
        case InputKeyOk:
            if(event->type == InputTypeShort && menu->callback) {
                DashboardViewMenuModel* model = view_get_model(menu->view);
                uint32_t index = DashboardViewMenuItemArray_get(model->items, position)->index;
                view_commit_model(menu->view, false);
                menu->callback(menu->context, index);
            }
            consumed = true;
            break;
        default:
            break;
        }

        if(consumed && event->key != InputKeyOk) dashboard_view_menu_set_position(menu, position);
    }

    return consumed;
}

DashboardViewMenu* dashboard_view_menu_alloc(void) {
    DashboardViewMenu* menu = malloc(sizeof(DashboardViewMenu));
    menu->view = view_alloc();
    menu->callback = NULL;
    menu->context = NULL;

    view_set_context(menu->view, menu);
    view_allocate_model(menu->view, ViewModelTypeLocking, sizeof(DashboardViewMenuModel));
    view_set_draw_callback(menu->view, dashboard_view_menu_draw_callback);
    view_set_input_callback(menu->view, dashboard_view_menu_input_callback);

    with_view_model(
        menu->view,
        DashboardViewMenuModel * model,
        {
            DashboardViewMenuItemArray_init(model->items);
            model->position = 0;
        },
        true);

    return menu;
}

void dashboard_view_menu_free(DashboardViewMenu* menu) {
    furi_assert(menu);
    dashboard_view_menu_reset(menu);
    with_view_model(
        menu->view,
        DashboardViewMenuModel * model,
        { DashboardViewMenuItemArray_clear(model->items); },
        false);
    view_free(menu->view);
    free(menu);
}

View* dashboard_view_menu_get_view(DashboardViewMenu* menu) {
    furi_assert(menu);
    return menu->view;
}

void dashboard_view_menu_reset(DashboardViewMenu* menu) {
    furi_assert(menu);
    with_view_model(
        menu->view,
        DashboardViewMenuModel * model,
        {
            DashboardViewMenuItemArray_reset(model->items);
            model->position = 0;
        },
        true);
}

void dashboard_view_menu_add_item(
    DashboardViewMenu* menu,
    const char* label,
    const Icon* icon,
    uint32_t index) {
    furi_assert(menu);
    furi_assert(label);
    furi_assert(icon);

    with_view_model(
        menu->view,
        DashboardViewMenuModel * model,
        {
            DashboardViewMenuItem* item = DashboardViewMenuItemArray_push_new(model->items);
            item->label = label;
            item->icon = icon;
            item->index = index;
        },
        true);
}

void dashboard_view_menu_set_callback(
    DashboardViewMenu* menu,
    DashboardViewMenuCallback callback,
    void* context) {
    furi_assert(menu);
    menu->callback = callback;
    menu->context = context;
}

void dashboard_view_menu_set_selected_item(DashboardViewMenu* menu, uint32_t index) {
    furi_assert(menu);
    with_view_model(
        menu->view,
        DashboardViewMenuModel * model,
        {
            size_t items_count = DashboardViewMenuItemArray_size(model->items);
            for(size_t position = 0; position < items_count; position++) {
                DashboardViewMenuItem* item =
                    DashboardViewMenuItemArray_get(model->items, position);
                if(item->index == index) {
                    model->position = position;
                    break;
                }
            }
        },
        true);
}

uint32_t dashboard_view_menu_get_selected_item(DashboardViewMenu* menu) {
    DashboardViewMenuModel* model = view_get_model(menu->view);
    uint32_t index = 0;
    if(DashboardViewMenuItemArray_size(model->items)) {
        index = DashboardViewMenuItemArray_get(model->items, model->position)->index;
    }
    view_commit_model(menu->view, false);
    return index;
}
