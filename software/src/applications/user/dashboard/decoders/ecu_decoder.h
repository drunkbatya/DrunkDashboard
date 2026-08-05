#pragma once

#include "../dashboard_can_frame.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ECU_DECODER_MAX_FRAMES 16U

typedef struct {
    DashboardCanFrame frames[ECU_DECODER_MAX_FRAMES];
    bool found[ECU_DECODER_MAX_FRAMES];
} EcuCanFrames;

typedef struct {
    bool valid;
    int32_t value;
} EcuOptionalValue;

typedef struct {
    EcuOptionalValue warning_counter;
    EcuOptionalValue last_error;
    EcuOptionalValue rev_lim_act;
    EcuOptionalValue main_relay_act;
    EcuOptionalValue fuel_pump_act;
    EcuOptionalValue cel_act;
    EcuOptionalValue ego_heat_act;
    EcuOptionalValue lambda_protect_act;
    EcuOptionalValue fan;
    EcuOptionalValue fan2;
    EcuOptionalValue current_gear;
    EcuOptionalValue distance_deci_km;

    EcuOptionalValue rpm;
    EcuOptionalValue ignition_cdeg;
    EcuOptionalValue inj_duty_cpercent;
    EcuOptionalValue ign_duty_cpercent;
    EcuOptionalValue vehicle_speed_kph;
    EcuOptionalValue flex_percent;

    EcuOptionalValue pps_cpercent;
    EcuOptionalValue tps1_cpercent;
    EcuOptionalValue tps2_cpercent;
    EcuOptionalValue wastegate_cpercent;

    EcuOptionalValue map_deci_kpa;
    EcuOptionalValue coolant_temp_c;
    EcuOptionalValue intake_temp_c;
    EcuOptionalValue aux1_temp_c;
    EcuOptionalValue aux2_temp_c;
    EcuOptionalValue mcu_temp_c;
    EcuOptionalValue fuel_level_cpercent;

    EcuOptionalValue oil_press_deci_kpa;
    EcuOptionalValue oil_temp_c;
    EcuOptionalValue fuel_temp_c;
    EcuOptionalValue batt_mv;

    EcuOptionalValue cyl_airmass_mg;
    EcuOptionalValue est_maf_centi_kgh;
    EcuOptionalValue inj_pw_us;
    EcuOptionalValue knock_count;

    EcuOptionalValue fuel_used_g;
    EcuOptionalValue fuel_flow_mgps;
    EcuOptionalValue fuel_trim1_cpercent;
    EcuOptionalValue fuel_trim2_cpercent;

    EcuOptionalValue lambda1_ten_thousandth;
    EcuOptionalValue lambda2_ten_thousandth;
    EcuOptionalValue fp_low_deci_kpa;
    EcuOptionalValue fp_high_decibar;

    EcuOptionalValue cam1_i_deg;
    EcuOptionalValue cam1_i_target_deg;
    EcuOptionalValue cam1_e_deg;
    EcuOptionalValue cam1_e_target_deg;
    EcuOptionalValue cam2_i_deg;
    EcuOptionalValue cam2_i_target_deg;
    EcuOptionalValue cam2_e_deg;
    EcuOptionalValue cam2_e_target_deg;

    EcuOptionalValue egt_c[8];
    EcuOptionalValue knock_db[3];
} EcuState;

typedef struct {
    const char* name;
    const uint32_t* frame_ids;
    size_t frame_count;
    void (*decode)(const EcuCanFrames* input, EcuState* output);
} EcuDecoder;
