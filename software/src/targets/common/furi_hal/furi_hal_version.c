#include <furi_hal_version.h>
#include <furi_hal_target_hw.h>
#include <furi.h>
#include <stm32u5xx.h>

#define AD_TYPE_COMPLETE_LOCAL_NAME "\x09"

static char furi_hal_version_name[FURI_HAL_VERSION_ARRAY_NAME_LENGTH] = FURI_HAL_TARGET_NAME;
static char furi_hal_version_device_name[FURI_HAL_VERSION_DEVICE_NAME_LENGTH] =
    AD_TYPE_COMPLETE_LOCAL_NAME "Drunk" FURI_HAL_TARGET_NAME;
static uint8_t furi_hal_version_ble_mac[6] = {0};

void furi_hal_version_init(void) {
    const uint8_t* uid = (const uint8_t*)UID_BASE;
    furi_hal_version_ble_mac[0] = uid[0];
    furi_hal_version_ble_mac[1] = uid[1];
    furi_hal_version_ble_mac[2] = uid[2];
    furi_hal_version_ble_mac[3] = uid[9];
    furi_hal_version_ble_mac[4] = uid[10];
    furi_hal_version_ble_mac[5] = uid[11];
}

FuriHalVersionOtpVersion furi_hal_version_get_otp_version(void) {
    return FuriHalVersionOtpVersionEmpty;
}

uint8_t furi_hal_version_get_hw_version(void) {
    return 1;
}

uint8_t furi_hal_version_get_hw_target(void) {
    return TARGET;
}

uint8_t furi_hal_version_get_hw_body(void) {
    return 0;
}

FuriHalVersionColor furi_hal_version_get_hw_color(void) {
    return FuriHalVersionColorUnknown;
}

uint8_t furi_hal_version_get_hw_connect(void) {
    return 0;
}

FuriHalVersionRegion furi_hal_version_get_hw_region(void) {
    return FuriHalVersionRegionUnknown;
}

const char* furi_hal_version_get_hw_region_name(void) {
    return "R00";
}

FuriHalVersionDisplay furi_hal_version_get_hw_display(void) {
    return FuriHalVersionDisplayUnknown;
}

uint32_t furi_hal_version_get_hw_timestamp(void) {
    return 0;
}

const char* furi_hal_version_get_name_ptr(void) {
    return furi_hal_version_name;
}

const char* furi_hal_version_get_device_name_ptr(void) {
    return &furi_hal_version_device_name[1];
}

const char* furi_hal_version_get_ble_local_device_name_ptr(void) {
    return furi_hal_version_device_name;
}

const uint8_t* furi_hal_version_get_ble_mac(void) {
    return furi_hal_version_ble_mac;
}

const struct Version* furi_hal_version_get_firmware_version(void) {
    return version_get();
}

size_t furi_hal_version_uid_size(void) {
    return 12;
}

const uint8_t* furi_hal_version_uid(void) {
    return (const uint8_t*)UID_BASE;
}
