#pragma once

#include "dialogs.h"
#include "dialogs_message.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DialogsApp {
    FuriMessageQueue* message_queue;
};

#ifdef __cplusplus
}
#endif
