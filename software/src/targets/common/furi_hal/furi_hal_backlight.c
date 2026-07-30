#include <furi_hal_backlight.h>
#include <furi_hal_pwm.h>

#define FURI_HAL_BACKLIGHT_PWM_FREQ 1000U

static uint8_t furi_hal_backlight_percent = 0;

void furi_hal_backlight_set(uint8_t percent) {
    if(percent > 100) percent = 100;

    if(furi_hal_pwm_is_running(FuriHalPwmOutputIdDisplayBacklight)) {
        furi_hal_pwm_set_params(
            FuriHalPwmOutputIdDisplayBacklight, FURI_HAL_BACKLIGHT_PWM_FREQ, percent);
    } else {
        furi_hal_pwm_start(
            FuriHalPwmOutputIdDisplayBacklight, FURI_HAL_BACKLIGHT_PWM_FREQ, percent);
    }

    furi_hal_backlight_percent = percent;
}

uint8_t furi_hal_backlight_get(void) {
    return furi_hal_backlight_percent;
}
