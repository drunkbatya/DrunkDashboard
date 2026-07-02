#include <furi_hal_info.h>
#include <furi.h>
#include <stm32u5xx.h>

FURI_WEAK void furi_hal_info_get_api_version(uint16_t* major, uint16_t* minor) {
    *major = 0;
    *minor = 0;
}

void furi_hal_info_get(PropertyValueCallback out, char sep, void* context) {
    FuriString* key = furi_string_alloc();
    FuriString* value = furi_string_alloc();

    PropertyValueContext property_context = {
        .key = key,
        .value = value,
        .out = out,
        .sep = sep,
        .last = false,
        .context = context,
    };

    property_value_out(&property_context, NULL, 2, "hardware", "model", "STM32U585");

    furi_string_reset(value);
    const uint32_t* uid = (const uint32_t*)UID_BASE;
    furi_string_cat_printf(value, "%08lX%08lX%08lX", uid[0], uid[1], uid[2]);
    property_value_out(&property_context, NULL, 2, "hardware", "uid", furi_string_get_cstr(value));

    property_value_out(&property_context, "%lu", 2, "hardware", "flash", FLASH_SIZE);
    property_value_out(&property_context, "%lu", 2, "system", "clock", SystemCoreClock);

    uint16_t api_version_major = 0;
    uint16_t api_version_minor = 0;
    furi_hal_info_get_api_version(&api_version_major, &api_version_minor);
    property_value_out(&property_context, "%u", 3, "firmware", "api", "major", api_version_major);
    property_value_out(&property_context, "%u", 3, "firmware", "api", "minor", api_version_minor);

    property_context.last = true;
    property_value_out(&property_context, NULL, 2, "radio", "alive", "false");

    furi_string_free(value);
    furi_string_free(key);
}
