#include "rusefi_can_decoder.h"

#include <furi.h>

#include <string.h>

static bool
    rusefi_can_decoder_get_u8(const DashboardCanFrame* frame, uint8_t offset, uint8_t* value) {
    if(frame->last_len < (uint32_t)offset + 1U) return false;
    *value = frame->last_data[offset];
    return true;
}

static bool
    rusefi_can_decoder_get_i8(const DashboardCanFrame* frame, uint8_t offset, int8_t* value) {
    uint8_t raw = 0;
    if(!rusefi_can_decoder_get_u8(frame, offset, &raw)) return false;
    *value = (int8_t)raw;
    return true;
}

static bool
    rusefi_can_decoder_get_u16_le(const DashboardCanFrame* frame, uint8_t offset, uint16_t* value) {
    if(frame->last_len < (uint32_t)offset + 2U) return false;
    *value = (uint16_t)frame->last_data[offset] | ((uint16_t)frame->last_data[offset + 1U] << 8);
    return true;
}

static bool
    rusefi_can_decoder_get_i16_le(const DashboardCanFrame* frame, uint8_t offset, int16_t* value) {
    uint16_t raw = 0;
    if(!rusefi_can_decoder_get_u16_le(frame, offset, &raw)) return false;
    *value = (int16_t)raw;
    return true;
}

static bool rusefi_can_decoder_get_bit(const DashboardCanFrame* frame, uint8_t bit, bool* value) {
    const uint8_t offset = bit / 8U;
    const uint8_t mask = 1U << (bit % 8U);
    uint8_t raw = 0;
    if(!rusefi_can_decoder_get_u8(frame, offset, &raw)) return false;
    *value = (raw & mask) != 0;
    return true;
}

static void rusefi_can_decoder_set(EcuOptionalValue* dst, int32_t value) {
    dst->valid = true;
    dst->value = value;
}

static void rusefi_can_decoder_decode_u8(
    const DashboardCanFrame* frame,
    uint8_t offset,
    EcuOptionalValue* dst,
    int32_t mul,
    int32_t div,
    int32_t offset_value) {
    uint8_t raw = 0;
    if(rusefi_can_decoder_get_u8(frame, offset, &raw)) {
        rusefi_can_decoder_set(dst, ((int32_t)raw * mul) / div + offset_value);
    }
}

static void rusefi_can_decoder_decode_i8(
    const DashboardCanFrame* frame,
    uint8_t offset,
    EcuOptionalValue* dst,
    int32_t mul,
    int32_t div,
    int32_t offset_value) {
    int8_t raw = 0;
    if(rusefi_can_decoder_get_i8(frame, offset, &raw)) {
        rusefi_can_decoder_set(dst, ((int32_t)raw * mul) / div + offset_value);
    }
}

static void rusefi_can_decoder_decode_u16(
    const DashboardCanFrame* frame,
    uint8_t offset,
    EcuOptionalValue* dst,
    int32_t mul,
    int32_t div,
    int32_t offset_value) {
    uint16_t raw = 0;
    if(rusefi_can_decoder_get_u16_le(frame, offset, &raw)) {
        rusefi_can_decoder_set(dst, ((int32_t)raw * mul) / div + offset_value);
    }
}

static void rusefi_can_decoder_decode_i16(
    const DashboardCanFrame* frame,
    uint8_t offset,
    EcuOptionalValue* dst,
    int32_t mul,
    int32_t div,
    int32_t offset_value) {
    int16_t raw = 0;
    if(rusefi_can_decoder_get_i16_le(frame, offset, &raw)) {
        rusefi_can_decoder_set(dst, ((int32_t)raw * mul) / div + offset_value);
    }
}

static void rusefi_can_decoder_decode_bit(
    const DashboardCanFrame* frame,
    uint8_t bit,
    EcuOptionalValue* dst) {
    bool value = false;
    if(rusefi_can_decoder_get_bit(frame, bit, &value)) {
        rusefi_can_decoder_set(dst, value ? 1 : 0);
    }
}

typedef enum {
    RusEfiCanFrame200,
    RusEfiCanFrame201,
    RusEfiCanFrame202,
    RusEfiCanFrame203,
    RusEfiCanFrame204,
    RusEfiCanFrame205,
    RusEfiCanFrame206,
    RusEfiCanFrame207,
    RusEfiCanFrame208,
    RusEfiCanFrame209,
    RusEfiCanFrame20A,
    RusEfiCanFrameCount,
} RusEfiCanFrameIndex;

_Static_assert(RusEfiCanFrameCount <= ECU_DECODER_MAX_FRAMES, "too many frames for decoder input");

static const uint32_t rusefi_can_frame_ids[RusEfiCanFrameCount] =
    {0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208, 0x209, 0x20A};

static void rusefi_can_decoder_decode(const EcuCanFrames* input, EcuState* output) {
    furi_assert(input);
    furi_assert(output);

    memset(output, 0, sizeof(EcuState));

    const DashboardCanFrame* frames = input->frames;
    const bool* found = input->found;

    if(found[RusEfiCanFrame200]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame200];
        rusefi_can_decoder_decode_u16(frame, 0, &output->warning_counter, 1, 1, 0);
        rusefi_can_decoder_decode_u16(frame, 2, &output->last_error, 1, 1, 0);
        rusefi_can_decoder_decode_bit(frame, 32, &output->rev_lim_act);
        rusefi_can_decoder_decode_bit(frame, 33, &output->main_relay_act);
        rusefi_can_decoder_decode_bit(frame, 34, &output->fuel_pump_act);
        rusefi_can_decoder_decode_bit(frame, 35, &output->cel_act);
        rusefi_can_decoder_decode_bit(frame, 36, &output->ego_heat_act);
        rusefi_can_decoder_decode_bit(frame, 37, &output->lambda_protect_act);
        rusefi_can_decoder_decode_bit(frame, 38, &output->fan);
        rusefi_can_decoder_decode_bit(frame, 39, &output->fan2);
        rusefi_can_decoder_decode_u8(frame, 5, &output->current_gear, 1, 1, 0);
        rusefi_can_decoder_decode_u16(frame, 6, &output->distance_deci_km, 1, 1, 0);
    }

    if(found[RusEfiCanFrame201]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame201];
        rusefi_can_decoder_decode_u16(frame, 0, &output->rpm, 1, 1, 0);
        rusefi_can_decoder_decode_i16(frame, 2, &output->ignition_cdeg, 2, 1, 0);
        rusefi_can_decoder_decode_u8(frame, 4, &output->inj_duty_cpercent, 50, 1, 0);
        rusefi_can_decoder_decode_u8(frame, 5, &output->ign_duty_cpercent, 50, 1, 0);
        rusefi_can_decoder_decode_u8(frame, 6, &output->vehicle_speed_kph, 1, 1, 0);
        rusefi_can_decoder_decode_u8(frame, 7, &output->flex_percent, 1, 1, 0);
    }

    if(found[RusEfiCanFrame202]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame202];
        rusefi_can_decoder_decode_i16(frame, 0, &output->pps_cpercent, 1, 1, 0);
        rusefi_can_decoder_decode_i16(frame, 2, &output->tps1_cpercent, 1, 1, 0);
        rusefi_can_decoder_decode_i16(frame, 4, &output->tps2_cpercent, 1, 1, 0);
        rusefi_can_decoder_decode_i16(frame, 6, &output->wastegate_cpercent, 1, 1, 0);
    }

    if(found[RusEfiCanFrame203]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame203];
        rusefi_can_decoder_decode_u16(frame, 0, &output->map_deci_kpa, 1, 3, 0);
        rusefi_can_decoder_decode_u8(frame, 2, &output->coolant_temp_c, 1, 1, -40);
        rusefi_can_decoder_decode_u8(frame, 3, &output->intake_temp_c, 1, 1, -40);
        rusefi_can_decoder_decode_u8(frame, 4, &output->aux1_temp_c, 1, 1, -40);
        rusefi_can_decoder_decode_u8(frame, 5, &output->aux2_temp_c, 1, 1, -40);
        rusefi_can_decoder_decode_u8(frame, 6, &output->mcu_temp_c, 1, 1, -40);
        rusefi_can_decoder_decode_u8(frame, 7, &output->fuel_level_cpercent, 50, 1, 0);
    }

    if(found[RusEfiCanFrame204]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame204];
        rusefi_can_decoder_decode_u16(frame, 2, &output->oil_press_deci_kpa, 1, 3, 0);
        rusefi_can_decoder_decode_u8(frame, 4, &output->oil_temp_c, 1, 1, -40);
        rusefi_can_decoder_decode_u8(frame, 5, &output->fuel_temp_c, 1, 1, -40);
        rusefi_can_decoder_decode_u16(frame, 6, &output->batt_mv, 1, 1, 0);
    }

    if(found[RusEfiCanFrame205]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame205];
        rusefi_can_decoder_decode_u16(frame, 0, &output->cyl_airmass_mg, 1, 1, 0);
        rusefi_can_decoder_decode_u16(frame, 2, &output->est_maf_centi_kgh, 1, 1, 0);
        rusefi_can_decoder_decode_u16(frame, 4, &output->inj_pw_us, 10, 3, 0);
        rusefi_can_decoder_decode_u16(frame, 6, &output->knock_count, 1, 1, 0);
    }

    if(found[RusEfiCanFrame206]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame206];
        rusefi_can_decoder_decode_u16(frame, 0, &output->fuel_used_g, 1, 1, 0);
        rusefi_can_decoder_decode_u16(frame, 2, &output->fuel_flow_mgps, 5, 1, 0);
        rusefi_can_decoder_decode_i16(frame, 4, &output->fuel_trim1_cpercent, 1, 1, 0);
        rusefi_can_decoder_decode_i16(frame, 6, &output->fuel_trim2_cpercent, 1, 1, 0);
    }

    if(found[RusEfiCanFrame207]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame207];
        rusefi_can_decoder_decode_u16(frame, 0, &output->lambda1_ten_thousandth, 1, 1, 0);
        rusefi_can_decoder_decode_u16(frame, 2, &output->lambda2_ten_thousandth, 1, 1, 0);
        rusefi_can_decoder_decode_u16(frame, 4, &output->fp_low_deci_kpa, 1, 3, 0);
        rusefi_can_decoder_decode_u16(frame, 6, &output->fp_high_decibar, 1, 1, 0);
    }

    if(found[RusEfiCanFrame208]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame208];
        rusefi_can_decoder_decode_i8(frame, 0, &output->cam1_i_deg, 1, 1, 0);
        rusefi_can_decoder_decode_i8(frame, 1, &output->cam1_i_target_deg, 1, 1, 0);
        rusefi_can_decoder_decode_i8(frame, 2, &output->cam1_e_deg, 1, 1, 0);
        rusefi_can_decoder_decode_i8(frame, 3, &output->cam1_e_target_deg, 1, 1, 0);
        rusefi_can_decoder_decode_i8(frame, 4, &output->cam2_i_deg, 1, 1, 0);
        rusefi_can_decoder_decode_i8(frame, 5, &output->cam2_i_target_deg, 1, 1, 0);
        rusefi_can_decoder_decode_i8(frame, 6, &output->cam2_e_deg, 1, 1, 0);
        rusefi_can_decoder_decode_i8(frame, 7, &output->cam2_e_target_deg, 1, 1, 0);
    }

    if(found[RusEfiCanFrame209]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame209];
        for(uint8_t i = 0; i < 8U; i++) {
            rusefi_can_decoder_decode_u8(frame, i, &output->egt_c[i], 5, 1, 0);
        }
    }

    if(found[RusEfiCanFrame20A]) {
        const DashboardCanFrame* frame = &frames[RusEfiCanFrame20A];
        for(uint8_t i = 0; i < 3U; i++) {
            rusefi_can_decoder_decode_i8(frame, i, &output->knock_db[i], 1, 1, 0);
        }
    }
}

const EcuDecoder rusefi_can_decoder = {
    .name = "rusEFI",
    .frame_ids = rusefi_can_frame_ids,
    .frame_count = RusEfiCanFrameCount,
    .decode = rusefi_can_decoder_decode,
};
