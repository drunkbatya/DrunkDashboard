#include <furi_hal_power.h>

#include <stm32u5xx.h>

static uint16_t insomnia_level = 0;

void furi_hal_power_init(void) {
}

bool furi_hal_power_gauge_is_ok(void) {
    return false;
}

bool furi_hal_power_is_shutdown_requested(void) {
    return false;
}

uint16_t furi_hal_power_insomnia_level(void) {
    return insomnia_level;
}

void furi_hal_power_insomnia_enter(void) {
    insomnia_level++;
}

void furi_hal_power_insomnia_exit(void) {
    if(insomnia_level) insomnia_level--;
}

bool furi_hal_power_sleep_available(void) {
    return insomnia_level == 0;
}

void furi_hal_power_sleep(void) {
    __WFI();
}

uint8_t furi_hal_power_get_pct(void) {
    return 0;
}

uint8_t furi_hal_power_get_bat_health_pct(void) {
    return 0;
}

bool furi_hal_power_is_charging(void) {
    return false;
}

bool furi_hal_power_is_charging_done(void) {
    return false;
}

void furi_hal_power_shutdown(void) {
    NVIC_SystemReset();
}

void furi_hal_power_off(void) {
    NVIC_SystemReset();
}

void furi_hal_power_reset(void) {
    NVIC_SystemReset();
    __builtin_unreachable();
}

bool furi_hal_power_enable_otg(void) {
    return false;
}

void furi_hal_power_disable_otg(void) {
}

bool furi_hal_power_check_otg_fault(void) {
    return false;
}

void furi_hal_power_check_otg_status(void) {
}

bool furi_hal_power_is_otg_enabled(void) {
    return false;
}

float furi_hal_power_get_battery_charge_voltage_limit(void) {
    return 0.0f;
}

void furi_hal_power_set_battery_charge_voltage_limit(float voltage) {
    UNUSED(voltage);
}

uint32_t furi_hal_power_get_battery_remaining_capacity(void) {
    return 0;
}

uint32_t furi_hal_power_get_battery_full_capacity(void) {
    return 0;
}

uint32_t furi_hal_power_get_battery_design_capacity(void) {
    return 0;
}

float furi_hal_power_get_battery_voltage(FuriHalPowerIC ic) {
    UNUSED(ic);
    return 0.0f;
}

float furi_hal_power_get_battery_current(FuriHalPowerIC ic) {
    UNUSED(ic);
    return 0.0f;
}

float furi_hal_power_get_battery_temperature(FuriHalPowerIC ic) {
    UNUSED(ic);
    return 0.0f;
}

float furi_hal_power_get_usb_voltage(void) {
    return 0.0f;
}

void furi_hal_power_enable_external_3_3v(void) {
}

void furi_hal_power_disable_external_3_3v(void) {
}

void furi_hal_power_suppress_charge_enter(void) {
}

void furi_hal_power_suppress_charge_exit(void) {
}

void furi_hal_power_info_get(PropertyValueCallback callback, char sep, void* context) {
    UNUSED(callback);
    UNUSED(sep);
    UNUSED(context);
}

void furi_hal_power_debug_get(PropertyValueCallback callback, void* context) {
    UNUSED(callback);
    UNUSED(context);
}
