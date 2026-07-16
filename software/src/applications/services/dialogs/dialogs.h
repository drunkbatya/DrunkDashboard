#pragma once

#include <furi.h>
#include <gui/canvas.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RECORD_DIALOGS "dialogs"

typedef struct DialogsApp DialogsApp;
typedef struct DialogMessage DialogMessage;

typedef enum {
    DialogMessageButtonBack,
    DialogMessageButtonLeft,
    DialogMessageButtonCenter,
    DialogMessageButtonRight,
} DialogMessageButton;

DialogMessage* dialog_message_alloc(void);
void dialog_message_free(DialogMessage* message);

void dialog_message_set_text(
    DialogMessage* message,
    const char* text,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical);

void dialog_message_set_header(
    DialogMessage* message,
    const char* text,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical);

void dialog_message_set_icon(DialogMessage* message, const Icon* icon, uint8_t x, uint8_t y);

void dialog_message_set_buttons(
    DialogMessage* message,
    const char* left,
    const char* center,
    const char* right);

DialogMessageButton dialog_message_show(DialogsApp* context, const DialogMessage* message);

#ifdef __cplusplus
}
#endif
