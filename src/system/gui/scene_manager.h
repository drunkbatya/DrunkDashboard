#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SceneManagerEventTypeCustom,
    SceneManagerEventTypeBack,
    SceneManagerEventTypeTick,
} SceneManagerEventType;

typedef struct {
    SceneManagerEventType type;
    uint32_t event;
} SceneManagerEvent;

typedef void (*SceneManagerSceneOnEnterCallback)(void* context);
typedef bool (*SceneManagerSceneOnEventCallback)(void* context, SceneManagerEvent event);
typedef void (*SceneManagerSceneOnExitCallback)(void* context);

typedef struct {
    const SceneManagerSceneOnEnterCallback* on_enter_handlers;
    const SceneManagerSceneOnEventCallback* on_event_handlers;
    const SceneManagerSceneOnExitCallback* on_exit_handlers;
    const uint32_t scene_num;
} SceneManagerHandlers;

typedef struct SceneManager SceneManager;

SceneManager* scene_manager_alloc(const SceneManagerHandlers* scene_handlers, void* context);
void scene_manager_free(SceneManager* scene_manager);
bool scene_manager_handle_custom_event(SceneManager* scene_manager, uint32_t custom_event);
bool scene_manager_handle_back_event(SceneManager* scene_manager);
void scene_manager_next_scene(SceneManager* scene_manager, uint32_t next_scene_id);
bool scene_manager_previous_scene(SceneManager* scene_manager);
void scene_manager_stop(SceneManager* scene_manager);
