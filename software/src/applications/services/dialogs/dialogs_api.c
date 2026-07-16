#include "dialogs_message.h"

#include <toolbox/api_lock.h>

DialogMessageButton dialog_message_show(DialogsApp* context, const DialogMessage* dialog_message) {
    furi_check(context);

    FuriApiLock lock = api_lock_alloc_locked();
    furi_check(lock != NULL);

    DialogsAppData data = {
        .dialog = {
            .message = dialog_message,
        }};

    DialogsAppReturn return_data;
    DialogsAppMessage message = {
        .lock = lock,
        .command = DialogsAppCommandDialog,
        .data = &data,
        .return_data = &return_data,
    };

    furi_check(
        furi_message_queue_put(context->message_queue, &message, FuriWaitForever) == FuriStatusOk);
    api_lock_wait_unlock_and_free(lock);

    return return_data.dialog_value;
}
