#pragma once

#include "dialogs_i.h"
#include <toolbox/api_lock.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const DialogMessage* message;
} DialogsAppMessageDataDialog;

typedef union {
    DialogsAppMessageDataDialog dialog;
} DialogsAppData;

typedef union {
    DialogMessageButton dialog_value;
} DialogsAppReturn;

typedef enum {
    DialogsAppCommandDialog,
} DialogsAppCommand;

typedef struct {
    FuriApiLock lock;
    DialogsAppCommand command;
    DialogsAppData* data;
    DialogsAppReturn* return_data;
} DialogsAppMessage;

#ifdef __cplusplus
}
#endif
