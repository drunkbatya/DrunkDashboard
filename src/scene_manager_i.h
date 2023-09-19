#pragma once

#include "scene_manager.h"
#include <mlib/m-array.h>

ARRAY_DEF(SceneManagerIdStack, uint32_t, M_DEFAULT_OPLIST);

typedef struct {
    uint32_t state;
} SceneManagerScene;

struct SceneManager {
    SceneManagerIdStack_t scene_id_stack;
    const SceneManagerHandlers* scene_handlers;
    SceneManagerScene* scenes;
    void* context;
};
