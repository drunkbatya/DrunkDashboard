#include "dialogs_message.h"
#include "dialogs_module_message.h"

#include <toolbox/api_lock.h>

static DialogsApp* dialogs_app_alloc(void) {
    DialogsApp* app = malloc(sizeof(DialogsApp));
    app->message_queue = furi_message_queue_alloc(8, sizeof(DialogsAppMessage));

    return app;
}

static void dialogs_app_process_message(DialogsApp* app, DialogsAppMessage* message) {
    UNUSED(app);
    switch(message->command) {
    case DialogsAppCommandDialog:
        message->return_data->dialog_value =
            dialogs_app_process_module_message(&message->data->dialog);
        break;
    }
    api_lock_unlock(message->lock);
}

int32_t dialogs_srv(void* p) {
    UNUSED(p);
    DialogsApp* app = dialogs_app_alloc();
    furi_record_create(RECORD_DIALOGS, app);

    DialogsAppMessage message;
    while(1) {
        if(furi_message_queue_get(app->message_queue, &message, FuriWaitForever) == FuriStatusOk) {
            dialogs_app_process_message(app, &message);
        }
    }

    return 0;
}
