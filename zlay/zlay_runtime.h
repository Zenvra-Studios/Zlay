#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <zlay.h>
#include <zlay_antialiasing.h>
#include <zlay_blurry.h>
#include <zlay_buffer.h>
#include <zlay_color.h>
#include <zlay_logger.h>
#include <zlay_shell.h>
#include <zlay_simd.h>
#include <zlay_ui.h>

#include <core/zlay_arena.h>
#include <core/zlay_cache.h>
#include <core/zlay_memory.h>

#include <lifecycle/zlay_lifecycle.h>

#include <backends/zlay_backends.h>

#include <drivers/gpu/opengl/zlay_opengl.h>
#include <drivers/gpu/vulkan/zlay_vulkan.h>
#include <drivers/gpu/zlay_backend.h>
#include <drivers/gpu/zlay_batch.h>
#include <drivers/gpu/zlay_context.h>
#include <drivers/gpu/zlay_driver.h>
#include <drivers/gpu/zlay_frame.h>
#include <drivers/gpu/zlay_gpu.h>
#include <drivers/gpu/zlay_pipeline.h>
#include <drivers/gpu/zlay_present.h>
#include <drivers/gpu/zlay_surface.h>
#include <drivers/gpu/zlay_swapchain.h>

#include <os/zlay_os.h>

#include <renderer/zlay_renderer.h>
#include <renderer/zlay_typetrue_fonts.h>
#include <renderer/zlay_wgRenderer.h>

#include <event/zlay_event_handle.h>
#include <event/zlay_add_eventlistener.h>
#include <event/zlay_hover_ui.h>
#include <event/zlay_animate.h>
#include <event/zlay_messagebox.h>
#include <event/zlay_network.h>
#include <event/zlay_effect.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_RUNTIME_MAX_NODES
#  define ZLAY_RUNTIME_MAX_NODES 256u
#endif

#ifndef ZLAY_RUNTIME_MAX_PREFABS
#  define ZLAY_RUNTIME_MAX_PREFABS 64u
#endif

#ifndef ZLAY_RUNTIME_MAX_FRAME_TASKS
#  define ZLAY_RUNTIME_MAX_FRAME_TASKS 128u
#endif

typedef void (*ZLay_RenderPrefabFn)(ZLay_RenderNode* node, ZLay_RenderScene* scene, void* user);
typedef void (*ZLay_FrameTaskFn)(ZLay_RenderScene* scene, void* user);

typedef struct ZLay_RenderPrefab {
  ZLay_EventString kind;
  ZLay_RenderLifecycleFn mounted_callback;
  ZLay_RenderLifecycleFn unmounted_callback;
  ZLay_RenderPrefabFn render;
  void* user;
  bool active;
} ZLay_RenderPrefab;

typedef struct ZLay_FrameTask {
  ZLay_FrameTaskFn callback;
  void* user;
  bool active;
} ZLay_FrameTask;

typedef struct ZLay_Runtime {
  ZLay_RenderScene scene;
  ZLay_RenderNode nodes[ZLAY_RUNTIME_MAX_NODES];
  bool node_used[ZLAY_RUNTIME_MAX_NODES];
  uint32_t next_node_index;
  ZLay_RenderPrefab prefabs[ZLAY_RUNTIME_MAX_PREFABS];
  uint32_t prefab_count;
  ZLay_FrameTask frame_tasks[ZLAY_RUNTIME_MAX_FRAME_TASKS];
  ZLay_Animator animator;
  ZLay_HoverUI hover;
  void* user;
} ZLay_Runtime;

static inline void ZLay_RuntimeInit(ZLay_Runtime* runtime, ZLay_RenderDriverKind driver_kind) {
  ZLay_EventString root_kind;
  ZLay_EventString root_id;
  ZLay_RenderNode* root;

  if (runtime == NULL) {
    return;
  }

  memset(runtime, 0, sizeof(*runtime));
  root_kind = ZLAY_EVENT_STRING_LITERAL("scene");
  root_id = ZLay_EventStringEmpty();
  root = &runtime->nodes[0];
  runtime->node_used[0] = true;
  runtime->next_node_index = 1u;
  ZLay_RenderNodeInit(root, ZLAY_RENDER_NODE_SCENE, root_kind, root_id);
  ZLay_RenderSceneInit(&runtime->scene, root, driver_kind);
  ZLay_AnimatorInit(&runtime->animator);
  ZLay_HoverUIInit(&runtime->hover, &runtime->scene);
}

static inline ZLay_RenderNode* ZLay_RuntimeAllocNode(ZLay_Runtime* runtime) {
  if (runtime == NULL) {
    return NULL;
  }

  for (uint32_t step = 0u; step < ZLAY_RUNTIME_MAX_NODES; ++step) {
    uint32_t index = (runtime->next_node_index + step) % ZLAY_RUNTIME_MAX_NODES;
    if (!runtime->node_used[index]) {
      runtime->node_used[index] = true;
      runtime->next_node_index = (index + 1u) % ZLAY_RUNTIME_MAX_NODES;
      return &runtime->nodes[index];
    }
  }

  return NULL;
}

static inline void ZLay_RuntimeFreeNode(ZLay_Runtime* runtime, ZLay_RenderNode* node) {
  if (runtime == NULL || node == NULL) {
    return;
  }

  for (uint32_t i = 0u; i < ZLAY_RUNTIME_MAX_NODES; ++i) {
    if (&runtime->nodes[i] == node) {
      runtime->node_used[i] = false;
      memset(node, 0, sizeof(*node));
      return;
    }
  }
}

static inline ZLay_RenderNode* ZLay_RuntimeCreateNode(
  ZLay_Runtime* runtime,
  const char* kind,
  const char* id
) {
  ZLay_RenderNode* node = ZLay_RuntimeAllocNode(runtime);
  if (node == NULL) {
    return NULL;
  }

  ZLay_RenderNodeInit(
    node,
    ZLAY_RENDER_NODE_LAYER,
    ZLay_EventStringFromCString(kind),
    ZLay_EventStringFromCString(id)
  );
  return node;
}

static inline ZLay_RenderNode* ZLay_RuntimeCreateText(ZLay_Runtime* runtime, const char* text) {
  ZLay_RenderNode* node = ZLay_RuntimeAllocNode(runtime);
  if (node == NULL) {
    return NULL;
  }

  ZLay_RenderNodeInit(
    node,
    ZLAY_RENDER_NODE_TEXT,
    ZLAY_EVENT_STRING_LITERAL("text"),
    ZLay_EventStringEmpty()
  );
  ZLay_RenderNodeSetText(node, ZLay_EventStringFromCString(text));
  return node;
}

static inline ZLay_RenderNode* ZLay_RuntimeFindById(ZLay_Runtime* runtime, const char* id) {
  if (runtime == NULL) {
    return NULL;
  }
  return ZLay_RenderFindById(runtime->scene.root, ZLay_EventStringFromCString(id));
}

static inline ZLay_RenderNode* ZLay_RuntimeFindByKind(ZLay_Runtime* runtime, const char* kind) {
  if (runtime == NULL) {
    return NULL;
  }
  return ZLay_RenderFindByKind(runtime->scene.root, ZLay_EventStringFromCString(kind));
}

static inline ZLay_RenderNode* ZLay_RuntimeFindByLayer(ZLay_Runtime* runtime, const char* layer) {
  if (runtime == NULL) {
    return NULL;
  }
  return ZLay_RenderFindByLayer(runtime->scene.root, ZLay_EventStringFromCString(layer));
}

static inline bool ZLay_DefineRenderPrefab(
  ZLay_Runtime* runtime,
  const char* kind,
  ZLay_RenderLifecycleFn mounted_callback,
  ZLay_RenderLifecycleFn unmounted_callback,
  ZLay_RenderPrefabFn render,
  void* user
) {
  ZLay_RenderPrefab* prefab;

  if (runtime == NULL || kind == NULL || runtime->prefab_count >= ZLAY_RUNTIME_MAX_PREFABS) {
    return false;
  }

  prefab = &runtime->prefabs[runtime->prefab_count++];
  memset(prefab, 0, sizeof(*prefab));
  prefab->kind = ZLay_EventStringFromCString(kind);
  prefab->mounted_callback = mounted_callback;
  prefab->unmounted_callback = unmounted_callback;
  prefab->render = render;
  prefab->user = user;
  prefab->active = true;
  return true;
}

static inline const ZLay_RenderPrefab* ZLay_RuntimeFindPrefab(
  const ZLay_Runtime* runtime,
  ZLay_EventString kind
) {
  if (runtime == NULL) {
    return NULL;
  }

  for (uint32_t i = 0u; i < runtime->prefab_count; ++i) {
    if (runtime->prefabs[i].active && ZLay_EventStringEquals(runtime->prefabs[i].kind, kind)) {
      return &runtime->prefabs[i];
    }
  }

  return NULL;
}

static inline ZLay_RenderNode* ZLay_RuntimeCreatePrefab(
  ZLay_Runtime* runtime,
  const char* kind,
  const char* id
) {
  const ZLay_RenderPrefab* prefab;
  ZLay_RenderNode* node = ZLay_RuntimeCreateNode(runtime, kind, id);

  if (node == NULL) {
    return NULL;
  }

  prefab = ZLay_RuntimeFindPrefab(runtime, node->kind);
  if (prefab != NULL) {
    node->mounted_callback = prefab->mounted_callback;
    node->unmounted_callback = prefab->unmounted_callback;
    node->user = prefab->user;
  }

  return node;
}

static inline void ZLay_RuntimeRenderPrefabs(ZLay_Runtime* runtime, ZLay_RenderNode* root) {
  const ZLay_RenderPrefab* prefab;

  if (runtime == NULL || root == NULL) {
    return;
  }

  prefab = ZLay_RuntimeFindPrefab(runtime, root->kind);
  if (prefab != NULL && prefab->render != NULL) {
    prefab->render(root, &runtime->scene, prefab->user);
  }

  for (uint32_t i = 0u; i < root->child_count; ++i) {
    ZLay_RuntimeRenderPrefabs(runtime, root->children[i]);
  }
}

static inline bool ZLay_RenderSceneFocusNode(ZLay_RenderScene* scene, ZLay_RenderNode* node) {
  ZLay_RenderNode* previous;

  if (scene == NULL || scene->active_node == node) {
    return false;
  }

  previous = scene->active_node;
  if (previous != NULL) {
    ZLay_RenderNodeSetState(previous, ZLAY_RENDER_STATE_FOCUSED, false);
    ZLay_DispatchSimpleEvent(previous, ZLAY_EVT_BLUR, ZLAY_RENDER_EVENT_ASCENDS, NULL);
  }

  scene->active_node = node;
  if (node != NULL) {
    ZLay_RenderNodeSetState(node, ZLAY_RENDER_STATE_FOCUSED, true);
    ZLay_DispatchSimpleEvent(node, ZLAY_EVT_FOCUS, ZLAY_RENDER_EVENT_ASCENDS, NULL);
  }

  return true;
}

static inline bool ZLay_QueueFrameTask(ZLay_Runtime* runtime, ZLay_FrameTaskFn callback, void* user) {
  if (runtime == NULL || callback == NULL) {
    return false;
  }

  for (uint32_t i = 0u; i < ZLAY_RUNTIME_MAX_FRAME_TASKS; ++i) {
    if (!runtime->frame_tasks[i].active) {
      runtime->frame_tasks[i].callback = callback;
      runtime->frame_tasks[i].user = user;
      runtime->frame_tasks[i].active = true;
      return true;
    }
  }

  return false;
}

static inline void ZLay_RunFrameTasks(ZLay_Runtime* runtime) {
  if (runtime == NULL) {
    return;
  }

  for (uint32_t i = 0u; i < ZLAY_RUNTIME_MAX_FRAME_TASKS; ++i) {
    if (runtime->frame_tasks[i].active) {
      ZLay_FrameTask task = runtime->frame_tasks[i];
      runtime->frame_tasks[i].active = false;
      task.callback(&runtime->scene, task.user);
    }
  }
}

static inline void ZLay_RuntimeTick(ZLay_Runtime* runtime, double delta_ms) {
  if (runtime == NULL) {
    return;
  }

  ZLay_AnimatorTick(&runtime->animator, delta_ms);
  ZLay_RunFrameTasks(runtime);
  ZLay_RuntimeRenderPrefabs(runtime, runtime->scene.root);
}

#ifdef __cplusplus
}
#endif
