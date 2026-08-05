#include "dashboard_view_dashboard.h"

#include <furi.h>
#include <gui/canvas.h>
#include <input/input.h>

#include <stdlib.h>
#include <string.h>

#define DASHBOARD_LABEL_X 0U
#define DASHBOARD_ROWS    5U

typedef enum {
    DashboardFormatInt,
    DashboardFormatBool,
    DashboardFormatFixed1,
    DashboardFormatFixed2,
    DashboardFormatFixed3,
    DashboardFormatFixed4,
} DashboardFormat;

typedef enum {
    DashboardValueRpm,
    DashboardValueMap,
    DashboardValueIgnition,
    DashboardValueInjPw,
    DashboardValueBatt,
    DashboardValueInjDuty,
    DashboardValueIgnDuty,
    DashboardValueSpeed,
    DashboardValueGear,
    DashboardValueFlex,
    DashboardValuePps,
    DashboardValueTps1,
    DashboardValueTps2,
    DashboardValueWastegate,
    DashboardValueCylAir,
    DashboardValueMaf,
    DashboardValueKnockCount,
    DashboardValueKnock0,
    DashboardValueKnock1,
    DashboardValueKnock2,
    DashboardValueCoolant,
    DashboardValueIntake,
    DashboardValueOilTemp,
    DashboardValueFuelTemp,
    DashboardValueMcuTemp,
    DashboardValueAux1,
    DashboardValueAux2,
    DashboardValueFuelLevel,
    DashboardValueFuelUsed,
    DashboardValueFuelFlow,
    DashboardValueLambda1,
    DashboardValueLambda2,
    DashboardValueFuelTrim1,
    DashboardValueFuelTrim2,
    DashboardValueOilPress,
    DashboardValueFpLow,
    DashboardValueFpHigh,
    DashboardValueWarnings,
    DashboardValueLastError,
    DashboardValueDistance,
    DashboardValueMainRelay,
    DashboardValueFuelPump,
    DashboardValueCel,
    DashboardValueRevLimiter,
    DashboardValueFan1,
    DashboardValueFan2,
    DashboardValueEgoHeat,
    DashboardValueLambdaProtect,
    DashboardValueCam1I,
    DashboardValueCam1ITarget,
    DashboardValueCam1E,
    DashboardValueCam1ETarget,
    DashboardValueCam2I,
    DashboardValueCam2ITarget,
    DashboardValueCam2E,
    DashboardValueCam2ETarget,
    DashboardValueEgt1,
    DashboardValueEgt2,
    DashboardValueEgt3,
    DashboardValueEgt4,
    DashboardValueEgt5,
    DashboardValueEgt6,
    DashboardValueEgt7,
    DashboardValueEgt8,
} DashboardValueId;

typedef struct {
    const char* label;
    DashboardValueId value_id;
    DashboardFormat format;
    const char* unit;
} DashboardRow;

typedef struct {
    const char* title;
    DashboardRow rows[DASHBOARD_ROWS];
} DashboardPage;

struct DashboardViewDashboard {
    View* view;
    DashboardViewDashboardCallback callback;
    void* context;
};

typedef struct {
    EcuState state;
    FuriString* line;
    uint8_t page;
} DashboardViewDashboardModel;

static const DashboardPage dashboard_pages[] = {
    {
        .title = "Engine",
        .rows =
            {
                {"RPM", DashboardValueRpm, DashboardFormatInt, "rpm"},
                {"MAP", DashboardValueMap, DashboardFormatFixed1, "kPa"},
                {"Ignition", DashboardValueIgnition, DashboardFormatFixed2, "deg"},
                {"Injector PW", DashboardValueInjPw, DashboardFormatFixed3, "ms"},
                {"Battery", DashboardValueBatt, DashboardFormatFixed3, "V"},
            },
    },
    {
        .title = "Engine 2",
        .rows =
            {
                {"Injector Duty", DashboardValueInjDuty, DashboardFormatFixed2, "%"},
                {"Ignition Duty", DashboardValueIgnDuty, DashboardFormatFixed2, "%"},
                {"Speed", DashboardValueSpeed, DashboardFormatInt, "kph"},
                {"Gear", DashboardValueGear, DashboardFormatInt, NULL},
                {"Flex", DashboardValueFlex, DashboardFormatInt, "%"},
            },
    },
    {
        .title = "Throttle/Air",
        .rows =
            {
                {"Pedal", DashboardValuePps, DashboardFormatFixed2, "%"},
                {"TPS1", DashboardValueTps1, DashboardFormatFixed2, "%"},
                {"TPS2", DashboardValueTps2, DashboardFormatFixed2, "%"},
                {"Wastegate", DashboardValueWastegate, DashboardFormatFixed2, "%"},
                {"Cylinder Air", DashboardValueCylAir, DashboardFormatInt, "mg"},
            },
    },
    {
        .title = "Air/Knock",
        .rows =
            {
                {"Estimated MAF", DashboardValueMaf, DashboardFormatFixed2, "kg/h"},
                {"Knock Count", DashboardValueKnockCount, DashboardFormatInt, NULL},
                {"Knock 0", DashboardValueKnock0, DashboardFormatInt, "db"},
                {"Knock 1", DashboardValueKnock1, DashboardFormatInt, "db"},
                {"Knock 2", DashboardValueKnock2, DashboardFormatInt, "db"},
            },
    },
    {
        .title = "Temps",
        .rows =
            {
                {"Coolant", DashboardValueCoolant, DashboardFormatInt, "C"},
                {"Intake Air", DashboardValueIntake, DashboardFormatInt, "C"},
                {"Oil", DashboardValueOilTemp, DashboardFormatInt, "C"},
                {"Fuel", DashboardValueFuelTemp, DashboardFormatInt, "C"},
                {"MCU", DashboardValueMcuTemp, DashboardFormatInt, "C"},
            },
    },
    {
        .title = "Temps/Fuel",
        .rows =
            {
                {"Aux1", DashboardValueAux1, DashboardFormatInt, "C"},
                {"Aux2", DashboardValueAux2, DashboardFormatInt, "C"},
                {"Fuel Level", DashboardValueFuelLevel, DashboardFormatFixed2, "%"},
                {"Fuel Used", DashboardValueFuelUsed, DashboardFormatInt, "g"},
                {"Fuel Flow", DashboardValueFuelFlow, DashboardFormatFixed3, "g/s"},
            },
    },
    {
        .title = "Fuel/Pressure",
        .rows =
            {
                {"Lambda 1", DashboardValueLambda1, DashboardFormatFixed4, NULL},
                {"Lambda 2", DashboardValueLambda2, DashboardFormatFixed4, NULL},
                {"Fuel Trim 1", DashboardValueFuelTrim1, DashboardFormatFixed2, "%"},
                {"Fuel Trim 2", DashboardValueFuelTrim2, DashboardFormatFixed2, "%"},
                {"Oil Pressure", DashboardValueOilPress, DashboardFormatFixed1, "kPa"},
            },
    },
    {
        .title = "Pressure",
        .rows =
            {
                {"Fuel Low", DashboardValueFpLow, DashboardFormatFixed1, "kPa"},
                {"Fuel High", DashboardValueFpHigh, DashboardFormatFixed1, "bar"},
            },
    },
    {
        .title = "Status",
        .rows =
            {
                {"Warnings", DashboardValueWarnings, DashboardFormatInt, NULL},
                {"Last Error", DashboardValueLastError, DashboardFormatInt, NULL},
                {"Distance", DashboardValueDistance, DashboardFormatFixed1, "km"},
                {"Main Relay", DashboardValueMainRelay, DashboardFormatBool, NULL},
                {"Fuel Pump", DashboardValueFuelPump, DashboardFormatBool, NULL},
            },
    },
    {
        .title = "Status 2",
        .rows =
            {
                {"CEL", DashboardValueCel, DashboardFormatBool, NULL},
                {"Rev Limiter", DashboardValueRevLimiter, DashboardFormatBool, NULL},
                {"Fan 1", DashboardValueFan1, DashboardFormatBool, NULL},
                {"Fan 2", DashboardValueFan2, DashboardFormatBool, NULL},
                {"EGO Heat", DashboardValueEgoHeat, DashboardFormatBool, NULL},
            },
    },
    {
        .title = "Cams",
        .rows =
            {
                {"Cam1 Intake", DashboardValueCam1I, DashboardFormatInt, "deg"},
                {"Cam1 Intake Tar", DashboardValueCam1ITarget, DashboardFormatInt, "deg"},
                {"Cam1 Exhaust", DashboardValueCam1E, DashboardFormatInt, "deg"},
                {"Cam1 Exhaust Tar", DashboardValueCam1ETarget, DashboardFormatInt, "deg"},
                {"Cam2 Intake", DashboardValueCam2I, DashboardFormatInt, "deg"},
            },
    },
    {
        .title = "Cams/EGT",
        .rows =
            {
                {"Cam2 Intake Tar", DashboardValueCam2ITarget, DashboardFormatInt, "deg"},
                {"Cam2 Exhaust", DashboardValueCam2E, DashboardFormatInt, "deg"},
                {"Cam2 Exhaust Tar", DashboardValueCam2ETarget, DashboardFormatInt, "deg"},
                {"Lambda Protect", DashboardValueLambdaProtect, DashboardFormatBool, NULL},
                {"EGT 1", DashboardValueEgt1, DashboardFormatInt, "C"},
            },
    },
    {
        .title = "EGT",
        .rows =
            {
                {"EGT 2", DashboardValueEgt2, DashboardFormatInt, "C"},
                {"EGT 3", DashboardValueEgt3, DashboardFormatInt, "C"},
                {"EGT 4", DashboardValueEgt4, DashboardFormatInt, "C"},
                {"EGT 5", DashboardValueEgt5, DashboardFormatInt, "C"},
                {"EGT 6", DashboardValueEgt6, DashboardFormatInt, "C"},
            },
    },
    {
        .title = "EGT 2",
        .rows =
            {
                {"EGT 7", DashboardValueEgt7, DashboardFormatInt, "C"},
                {"EGT 8", DashboardValueEgt8, DashboardFormatInt, "C"},
            },
    },
};

static const uint8_t dashboard_page_count = COUNT_OF(dashboard_pages);

static const EcuOptionalValue*
    dashboard_view_dashboard_get_value(const EcuState* state, DashboardValueId value_id) {
    switch(value_id) {
    case DashboardValueRpm:
        return &state->rpm;
    case DashboardValueMap:
        return &state->map_deci_kpa;
    case DashboardValueIgnition:
        return &state->ignition_cdeg;
    case DashboardValueInjPw:
        return &state->inj_pw_us;
    case DashboardValueBatt:
        return &state->batt_mv;
    case DashboardValueInjDuty:
        return &state->inj_duty_cpercent;
    case DashboardValueIgnDuty:
        return &state->ign_duty_cpercent;
    case DashboardValueSpeed:
        return &state->vehicle_speed_kph;
    case DashboardValueGear:
        return &state->current_gear;
    case DashboardValueFlex:
        return &state->flex_percent;
    case DashboardValuePps:
        return &state->pps_cpercent;
    case DashboardValueTps1:
        return &state->tps1_cpercent;
    case DashboardValueTps2:
        return &state->tps2_cpercent;
    case DashboardValueWastegate:
        return &state->wastegate_cpercent;
    case DashboardValueCylAir:
        return &state->cyl_airmass_mg;
    case DashboardValueMaf:
        return &state->est_maf_centi_kgh;
    case DashboardValueKnockCount:
        return &state->knock_count;
    case DashboardValueKnock0:
        return &state->knock_db[0];
    case DashboardValueKnock1:
        return &state->knock_db[1];
    case DashboardValueKnock2:
        return &state->knock_db[2];
    case DashboardValueCoolant:
        return &state->coolant_temp_c;
    case DashboardValueIntake:
        return &state->intake_temp_c;
    case DashboardValueOilTemp:
        return &state->oil_temp_c;
    case DashboardValueFuelTemp:
        return &state->fuel_temp_c;
    case DashboardValueMcuTemp:
        return &state->mcu_temp_c;
    case DashboardValueAux1:
        return &state->aux1_temp_c;
    case DashboardValueAux2:
        return &state->aux2_temp_c;
    case DashboardValueFuelLevel:
        return &state->fuel_level_cpercent;
    case DashboardValueFuelUsed:
        return &state->fuel_used_g;
    case DashboardValueFuelFlow:
        return &state->fuel_flow_mgps;
    case DashboardValueLambda1:
        return &state->lambda1_ten_thousandth;
    case DashboardValueLambda2:
        return &state->lambda2_ten_thousandth;
    case DashboardValueFuelTrim1:
        return &state->fuel_trim1_cpercent;
    case DashboardValueFuelTrim2:
        return &state->fuel_trim2_cpercent;
    case DashboardValueOilPress:
        return &state->oil_press_deci_kpa;
    case DashboardValueFpLow:
        return &state->fp_low_deci_kpa;
    case DashboardValueFpHigh:
        return &state->fp_high_decibar;
    case DashboardValueWarnings:
        return &state->warning_counter;
    case DashboardValueLastError:
        return &state->last_error;
    case DashboardValueDistance:
        return &state->distance_deci_km;
    case DashboardValueMainRelay:
        return &state->main_relay_act;
    case DashboardValueFuelPump:
        return &state->fuel_pump_act;
    case DashboardValueCel:
        return &state->cel_act;
    case DashboardValueRevLimiter:
        return &state->rev_lim_act;
    case DashboardValueFan1:
        return &state->fan;
    case DashboardValueFan2:
        return &state->fan2;
    case DashboardValueEgoHeat:
        return &state->ego_heat_act;
    case DashboardValueLambdaProtect:
        return &state->lambda_protect_act;
    case DashboardValueCam1I:
        return &state->cam1_i_deg;
    case DashboardValueCam1ITarget:
        return &state->cam1_i_target_deg;
    case DashboardValueCam1E:
        return &state->cam1_e_deg;
    case DashboardValueCam1ETarget:
        return &state->cam1_e_target_deg;
    case DashboardValueCam2I:
        return &state->cam2_i_deg;
    case DashboardValueCam2ITarget:
        return &state->cam2_i_target_deg;
    case DashboardValueCam2E:
        return &state->cam2_e_deg;
    case DashboardValueCam2ETarget:
        return &state->cam2_e_target_deg;
    case DashboardValueEgt1:
        return &state->egt_c[0];
    case DashboardValueEgt2:
        return &state->egt_c[1];
    case DashboardValueEgt3:
        return &state->egt_c[2];
    case DashboardValueEgt4:
        return &state->egt_c[3];
    case DashboardValueEgt5:
        return &state->egt_c[4];
    case DashboardValueEgt6:
        return &state->egt_c[5];
    case DashboardValueEgt7:
        return &state->egt_c[6];
    case DashboardValueEgt8:
        return &state->egt_c[7];
    }

    furi_crash();
}

static void
    dashboard_view_dashboard_format_fixed(FuriString* out, int32_t value, uint8_t decimals) {
    int32_t div = 1;
    for(uint8_t i = 0; i < decimals; i++) {
        div *= 10;
    }

    const bool negative = value < 0;
    const uint32_t abs_value = negative ? (uint32_t)(-(value + 1)) + 1U : (uint32_t)value;
    const char* sign = negative ? "-" : "";
    const uint32_t whole = abs_value / (uint32_t)div;
    const uint32_t fraction = abs_value % (uint32_t)div;

    furi_string_printf(out, "%s%lu.%0*lu", sign, whole, decimals, fraction);
}

static void dashboard_view_dashboard_format_value(
    FuriString* out,
    const EcuOptionalValue* value,
    DashboardFormat format) {
    if(!value->valid) {
        furi_string_set_str(out, format == DashboardFormatBool ? "-" : "--");
        return;
    }

    switch(format) {
    case DashboardFormatBool:
        furi_string_set_str(out, value->value ? "1" : "0");
        break;
    case DashboardFormatInt:
        furi_string_printf(out, "%ld", value->value);
        break;
    case DashboardFormatFixed1:
        dashboard_view_dashboard_format_fixed(out, value->value, 1U);
        break;
    case DashboardFormatFixed2:
        dashboard_view_dashboard_format_fixed(out, value->value, 2U);
        break;
    case DashboardFormatFixed3:
        dashboard_view_dashboard_format_fixed(out, value->value, 3U);
        break;
    case DashboardFormatFixed4:
        dashboard_view_dashboard_format_fixed(out, value->value, 4U);
        break;
    default:
        furi_crash();
    }
}

static void dashboard_view_dashboard_draw_row(
    DashboardViewDashboardModel* model,
    Canvas* canvas,
    uint8_t row,
    const DashboardRow* dashboard_row) {
    const uint8_t y = 8 + row * 10;

    canvas_draw_str(canvas, DASHBOARD_LABEL_X, y, dashboard_row->label);
    dashboard_view_dashboard_format_value(
        model->line,
        dashboard_view_dashboard_get_value(&model->state, dashboard_row->value_id),
        dashboard_row->format);
    if(dashboard_row->unit) {
        furi_string_cat_printf(model->line, " %s", dashboard_row->unit);
    }
    canvas_draw_str_aligned(
        canvas,
        canvas_width(canvas) - 1U,
        y,
        AlignRight,
        AlignBottom,
        furi_string_get_cstr(model->line));
}

static void dashboard_view_dashboard_draw_callback(Canvas* canvas, void* _model) {
    DashboardViewDashboardModel* model = _model;
    const DashboardPage* page = &dashboard_pages[model->page];

    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);

    furi_string_printf(model->line, "< %u/%u >", model->page + 1U, dashboard_page_count);
    canvas_draw_str_aligned(
        canvas,
        canvas_width(canvas) - 1U,
        8,
        AlignRight,
        AlignBottom,
        furi_string_get_cstr(model->line));

    furi_string_set_str(model->line, page->title);
    canvas_draw_str(canvas, 0, 8, furi_string_get_cstr(model->line));

    for(uint8_t i = 0; i < DASHBOARD_ROWS; i++) {
        if(page->rows[i].label) {
            dashboard_view_dashboard_draw_row(model, canvas, i + 1U, &page->rows[i]);
        }
    }
}

static bool dashboard_view_dashboard_input_callback(InputEvent* event, void* context) {
    DashboardViewDashboard* view_dashboard = context;

    if((event->type != InputTypeShort) && (event->type != InputTypeRepeat)) {
        return false;
    }

    if(event->key == InputKeyOk) {
        if(view_dashboard->callback) {
            view_dashboard->callback(view_dashboard->context, event->key);
        }
        return true;
    }

    if((event->key != InputKeyLeft) && (event->key != InputKeyRight)) {
        return false;
    }

    with_view_model(
        view_dashboard->view,
        DashboardViewDashboardModel * model,
        {
            if(event->key == InputKeyLeft) {
                model->page = (model->page == 0) ? dashboard_page_count - 1U : model->page - 1U;
            } else {
                model->page = (model->page + 1U) % dashboard_page_count;
            }
        },
        true);

    return true;
}

DashboardViewDashboard* dashboard_view_dashboard_alloc(void) {
    DashboardViewDashboard* view_dashboard = malloc(sizeof(DashboardViewDashboard));

    view_dashboard->view = view_alloc();
    view_dashboard->callback = NULL;
    view_dashboard->context = NULL;
    view_allocate_model(
        view_dashboard->view, ViewModelTypeLocking, sizeof(DashboardViewDashboardModel));
    view_set_context(view_dashboard->view, view_dashboard);
    view_set_draw_callback(view_dashboard->view, dashboard_view_dashboard_draw_callback);
    view_set_input_callback(view_dashboard->view, dashboard_view_dashboard_input_callback);

    with_view_model(
        view_dashboard->view,
        DashboardViewDashboardModel * model,
        {
            memset(&model->state, 0, sizeof(model->state));
            model->line = furi_string_alloc();
            model->page = 0;
        },
        true);

    return view_dashboard;
}

void dashboard_view_dashboard_free(DashboardViewDashboard* view_dashboard) {
    furi_assert(view_dashboard);
    with_view_model(
        view_dashboard->view,
        DashboardViewDashboardModel * model,
        { furi_string_free(model->line); },
        false);
    view_free(view_dashboard->view);
    free(view_dashboard);
}

View* dashboard_view_dashboard_get_view(DashboardViewDashboard* view_dashboard) {
    furi_assert(view_dashboard);
    return view_dashboard->view;
}

void dashboard_view_dashboard_update(DashboardViewDashboard* view_dashboard, const EcuState* state) {
    furi_assert(view_dashboard);
    furi_assert(state);

    with_view_model(
        view_dashboard->view,
        DashboardViewDashboardModel * model,
        { model->state = *state; },
        true);
}

void dashboard_view_dashboard_set_callback(
    DashboardViewDashboard* view_dashboard,
    DashboardViewDashboardCallback callback,
    void* context) {
    furi_assert(view_dashboard);
    view_dashboard->callback = callback;
    view_dashboard->context = context;
}
