#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_RENDER_MAX_CHILDREN
#  define ZLAY_RENDER_MAX_CHILDREN 64u
#endif

#ifndef ZLAY_RENDER_MAX_META
#  define ZLAY_RENDER_MAX_META 32u
#endif

#ifndef ZLAY_RENDER_MAX_LAYERS
#  define ZLAY_RENDER_MAX_LAYERS 16u
#endif

#ifndef ZLAY_RENDER_MAX_PARAMS
#  define ZLAY_RENDER_MAX_PARAMS 48u
#endif

#ifndef ZLAY_RENDER_MAX_EVENT_LISTENERS
#  define ZLAY_RENDER_MAX_EVENT_LISTENERS 32u
#endif

#ifndef ZLAY_RENDER_MAX_EVENT_PATH
#  define ZLAY_RENDER_MAX_EVENT_PATH 64u
#endif

#ifdef __cplusplus
#  define ZLAY_EVENT_STRING_LITERAL(value) ZLay_EventString{(value), static_cast<uint32_t>(sizeof(value) - 1u)}
#else
#  define ZLAY_EVENT_STRING_LITERAL(value) ((ZLay_EventString){(value), (uint32_t)(sizeof(value) - 1u)})
#endif

typedef struct ZLay_RenderNode ZLay_RenderNode;
typedef struct ZLay_RenderScene ZLay_RenderScene;
typedef struct ZLay_RenderEvent ZLay_RenderEvent;

typedef struct ZLay_EventString {
  const char* data;
  uint32_t length;
} ZLay_EventString;

static inline ZLay_EventString ZLay_EventStringEmpty(void) {
  ZLay_EventString result;
  result.data = NULL;
  result.length = 0u;
  return result;
}

typedef enum ZLay_RenderNodeType {
  ZLAY_RENDER_NODE_SCENE = 0,
  ZLAY_RENDER_NODE_LAYER,
  ZLAY_RENDER_NODE_TEXT,
  ZLAY_RENDER_NODE_GROUP
} ZLay_RenderNodeType;

typedef enum ZLay_RenderEventRoute {
  ZLAY_RENDER_EVENT_ROUTE_NONE = 0,
  ZLAY_RENDER_EVENT_ROUTE_DESCEND,
  ZLAY_RENDER_EVENT_ROUTE_TARGET,
  ZLAY_RENDER_EVENT_ROUTE_ASCEND
} ZLay_RenderEventRoute;

typedef enum ZLay_RenderEventFlags {
  ZLAY_RENDER_EVENT_NONE = 0,
  ZLAY_RENDER_EVENT_ASCENDS = 1u << 0,
  ZLAY_RENDER_EVENT_CANCELABLE = 1u << 1,
  ZLAY_RENDER_EVENT_TRUSTED = 1u << 2
} ZLay_RenderEventFlags;

typedef enum ZLay_RenderEventListenerOptions {
  ZLAY_RENDER_LISTENER_NONE = 0,
  ZLAY_RENDER_LISTENER_DESCEND = 1u << 0,
  ZLAY_RENDER_LISTENER_ONCE = 1u << 1,
  ZLAY_RENDER_LISTENER_PASSIVE = 1u << 2
} ZLay_RenderEventListenerOptions;

typedef enum ZLay_RenderStateFlags {
  ZLAY_RENDER_STATE_NONE = 0,
  ZLAY_RENDER_STATE_HOVERED = 1u << 0,
  ZLAY_RENDER_STATE_ACTIVE = 1u << 1,
  ZLAY_RENDER_STATE_FOCUSED = 1u << 2,
  ZLAY_RENDER_STATE_DISABLED = 1u << 3,
  ZLAY_RENDER_STATE_CHECKED = 1u << 4,
  ZLAY_RENDER_STATE_SELECTED = 1u << 5,
  ZLAY_RENDER_STATE_HIDDEN = 1u << 6,
  ZLAY_RENDER_STATE_DIRTY_STYLE = 1u << 7,
  ZLAY_RENDER_STATE_DIRTY_LAYOUT = 1u << 8,
  ZLAY_RENDER_STATE_DIRTY_PAINT = 1u << 9
} ZLay_RenderStateFlags;

typedef enum ZLay_RenderDriverKind {
  ZLAY_RENDER_DRIVER_NONE = 0,
  ZLAY_RENDER_DRIVER_OPENGL,
  ZLAY_RENDER_DRIVER_VULKAN
} ZLay_RenderDriverKind;

typedef struct ZLay_RenderMeta {
  ZLay_EventString name;
  ZLay_EventString value;
} ZLay_RenderMeta;

typedef struct ZLay_RenderParam {
  ZLay_EventString name;
  ZLay_EventString value;
} ZLay_RenderParam;

typedef struct ZLay_RenderNodeStyle {
  ZLay_Style layout;
  ZLay_RenderParam params[ZLAY_RENDER_MAX_PARAMS];
  uint32_t param_count;
} ZLay_RenderNodeStyle;

typedef void (*ZLay_RenderEventListenerFn)(ZLay_RenderEvent* event, void* user);
typedef void (*ZLay_RenderLifecycleFn)(ZLay_RenderNode* node, void* user);

typedef struct ZLay_RenderEventListener {
  ZLay_EventString type;
  ZLay_RenderEventListenerFn callback;
  void* user;
  uint32_t options;
  bool active;
} ZLay_RenderEventListener;

struct ZLay_RenderEvent {
  ZLay_EventString type;
  uint32_t flags;
  ZLay_RenderEventRoute route;
  ZLay_RenderNode* target;
  ZLay_RenderNode* current_target;
  double timestamp_ms;
  int pointer_id;
  float client_x;
  float client_y;
  int key_code;
  uint32_t key_mods;
  void* detail;
  bool default_prevented;
  bool route_stopped;
  bool immediate_route_stopped;
};

struct ZLay_RenderNode {
  ZLay_RenderNodeType type;
  ZLay_EventString kind;
  ZLay_EventString id;
  ZLay_EventString text;
  ZLay_RenderNode* parent;
  ZLay_RenderNode* children[ZLAY_RENDER_MAX_CHILDREN];
  uint32_t child_count;
  ZLay_RenderMeta meta[ZLAY_RENDER_MAX_META];
  uint32_t meta_count;
  ZLay_EventString layers[ZLAY_RENDER_MAX_LAYERS];
  uint32_t layer_count;
  ZLay_RenderNodeStyle style;
  ZLay_Rect layout;
  uint32_t state;
  ZLay_RenderEventListener listeners[ZLAY_RENDER_MAX_EVENT_LISTENERS];
  uint32_t listener_count;
  ZLay_RenderLifecycleFn mounted_callback;
  ZLay_RenderLifecycleFn unmounted_callback;
  void* user;
  void* native_handle;
};

struct ZLay_RenderScene {
  ZLay_RenderNode* root;
  ZLay_RenderNode* active_node;
  ZLay_RenderNode* hovered_node;
  ZLay_RenderNode* pointer_capture_node;
  ZLay_RenderDriverKind driver_kind;
  void* driver_handle;
  void* network_client;
  void* user;
};

static inline ZLay_EventString ZLay_EventStringFromCString(const char* text) {
  ZLay_EventString result;
  result.data = text;
  result.length = 0u;
  if (text != NULL) {
    while (text[result.length] != '\0') {
      ++result.length;
    }
  }
  return result;
}

static inline bool ZLay_EventStringIsEmpty(ZLay_EventString value) {
  return value.data == NULL || value.length == 0u;
}

static inline bool ZLay_EventStringEquals(ZLay_EventString a, ZLay_EventString b) {
  if (a.length != b.length) {
    return false;
  }
  if (a.length == 0u) {
    return true;
  }
  if (a.data == NULL || b.data == NULL) {
    return false;
  }
  return memcmp(a.data, b.data, a.length) == 0;
}

static inline bool ZLay_EventStringEqualsCString(ZLay_EventString a, const char* b) {
  return ZLay_EventStringEquals(a, ZLay_EventStringFromCString(b));
}

static inline bool ZLay_EventStringRangeEquals(
  ZLay_EventString value,
  const char* data,
  uint32_t length
) {
  if (value.length != length) {
    return false;
  }
  if (length == 0u) {
    return true;
  }
  if (value.data == NULL || data == NULL) {
    return false;
  }
  return memcmp(value.data, data, length) == 0;
}

static inline void ZLay_RenderNodeStyleInit(ZLay_RenderNodeStyle* style) {
  if (style == NULL) {
    return;
  }
  style->layout = ZLay_StyleDefault();
  style->param_count = 0u;
}

static inline void ZLay_RenderNodeInit(
  ZLay_RenderNode* node,
  ZLay_RenderNodeType type,
  ZLay_EventString kind,
  ZLay_EventString id
) {
  if (node == NULL) {
    return;
  }

  memset(node, 0, sizeof(*node));
  node->type = type;
  node->kind = kind;
  node->id = id;
  ZLay_RenderNodeStyleInit(&node->style);
}

static inline void ZLay_RenderSceneInit(
  ZLay_RenderScene* scene,
  ZLay_RenderNode* root,
  ZLay_RenderDriverKind driver_kind
) {
  if (scene == NULL) {
    return;
  }

  memset(scene, 0, sizeof(*scene));
  scene->root = root;
  scene->driver_kind = driver_kind;
}

static inline bool ZLay_RenderAttach(ZLay_RenderNode* parent, ZLay_RenderNode* child) {
  if (parent == NULL || child == NULL || parent->child_count >= ZLAY_RENDER_MAX_CHILDREN) {
    return false;
  }
  if (child->parent != NULL) {
    return false;
  }

  parent->children[parent->child_count++] = child;
  child->parent = parent;
  parent->state |= ZLAY_RENDER_STATE_DIRTY_LAYOUT | ZLAY_RENDER_STATE_DIRTY_PAINT;

  if (child->mounted_callback != NULL) {
    child->mounted_callback(child, child->user);
  }

  return true;
}

static inline bool ZLay_RenderDetach(ZLay_RenderNode* parent, ZLay_RenderNode* child) {
  if (parent == NULL || child == NULL) {
    return false;
  }

  for (uint32_t i = 0u; i < parent->child_count; ++i) {
    if (parent->children[i] == child) {
      for (uint32_t j = i + 1u; j < parent->child_count; ++j) {
        parent->children[j - 1u] = parent->children[j];
      }
      parent->children[parent->child_count - 1u] = NULL;
      --parent->child_count;
      child->parent = NULL;
      parent->state |= ZLAY_RENDER_STATE_DIRTY_LAYOUT | ZLAY_RENDER_STATE_DIRTY_PAINT;

      if (child->unmounted_callback != NULL) {
        child->unmounted_callback(child, child->user);
      }

      return true;
    }
  }

  return false;
}

static inline void ZLay_RenderNodeSetText(ZLay_RenderNode* node, ZLay_EventString text) {
  if (node == NULL) {
    return;
  }

  node->text = text;
  node->state |= ZLAY_RENDER_STATE_DIRTY_LAYOUT | ZLAY_RENDER_STATE_DIRTY_PAINT;
}

static inline ZLay_RenderMeta* ZLay_RenderNodeFindMeta(
  ZLay_RenderNode* node,
  ZLay_EventString name
) {
  if (node == NULL) {
    return NULL;
  }

  for (uint32_t i = 0u; i < node->meta_count; ++i) {
    if (ZLay_EventStringEquals(node->meta[i].name, name)) {
      return &node->meta[i];
    }
  }

  return NULL;
}

static inline const ZLay_RenderMeta* ZLay_RenderNodeFindMetaConst(
  const ZLay_RenderNode* node,
  ZLay_EventString name
) {
  if (node == NULL) {
    return NULL;
  }

  for (uint32_t i = 0u; i < node->meta_count; ++i) {
    if (ZLay_EventStringEquals(node->meta[i].name, name)) {
      return &node->meta[i];
    }
  }

  return NULL;
}

static inline bool ZLay_RenderNodeSetMeta(
  ZLay_RenderNode* node,
  ZLay_EventString name,
  ZLay_EventString value
) {
  ZLay_RenderMeta* meta;

  if (node == NULL || ZLay_EventStringIsEmpty(name)) {
    return false;
  }

  meta = ZLay_RenderNodeFindMeta(node, name);
  if (meta != NULL) {
    meta->value = value;
    node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_PAINT;
    return true;
  }

  if (node->meta_count >= ZLAY_RENDER_MAX_META) {
    return false;
  }

  node->meta[node->meta_count].name = name;
  node->meta[node->meta_count].value = value;
  ++node->meta_count;
  node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_PAINT;
  return true;
}

static inline ZLay_EventString ZLay_RenderNodeGetMeta(
  const ZLay_RenderNode* node,
  ZLay_EventString name
) {
  const ZLay_RenderMeta* meta = ZLay_RenderNodeFindMetaConst(node, name);
  return meta != NULL ? meta->value : ZLay_EventStringEmpty();
}

static inline bool ZLay_RenderNodeHasMeta(const ZLay_RenderNode* node, ZLay_EventString name) {
  return ZLay_RenderNodeFindMetaConst(node, name) != NULL;
}

static inline bool ZLay_RenderNodeRemoveMeta(ZLay_RenderNode* node, ZLay_EventString name) {
  if (node == NULL) {
    return false;
  }

  for (uint32_t i = 0u; i < node->meta_count; ++i) {
    if (ZLay_EventStringEquals(node->meta[i].name, name)) {
      for (uint32_t j = i + 1u; j < node->meta_count; ++j) {
        node->meta[j - 1u] = node->meta[j];
      }
      memset(&node->meta[node->meta_count - 1u], 0, sizeof(node->meta[0]));
      --node->meta_count;
      node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_PAINT;
      return true;
    }
  }

  return false;
}

static inline bool ZLay_RenderNodeHasLayer(
  const ZLay_RenderNode* node,
  ZLay_EventString layer
) {
  if (node == NULL) {
    return false;
  }

  for (uint32_t i = 0u; i < node->layer_count; ++i) {
    if (ZLay_EventStringEquals(node->layers[i], layer)) {
      return true;
    }
  }

  return false;
}

static inline bool ZLay_RenderNodeAddLayer(ZLay_RenderNode* node, ZLay_EventString layer) {
  if (node == NULL || ZLay_EventStringIsEmpty(layer)) {
    return false;
  }
  if (ZLay_RenderNodeHasLayer(node, layer)) {
    return true;
  }
  if (node->layer_count >= ZLAY_RENDER_MAX_LAYERS) {
    return false;
  }

  node->layers[node->layer_count++] = layer;
  node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_PAINT;
  return true;
}

static inline bool ZLay_RenderNodeRemoveLayer(ZLay_RenderNode* node, ZLay_EventString layer) {
  if (node == NULL) {
    return false;
  }

  for (uint32_t i = 0u; i < node->layer_count; ++i) {
    if (ZLay_EventStringEquals(node->layers[i], layer)) {
      for (uint32_t j = i + 1u; j < node->layer_count; ++j) {
        node->layers[j - 1u] = node->layers[j];
      }
      node->layers[node->layer_count - 1u] = ZLay_EventStringEmpty();
      --node->layer_count;
      node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_PAINT;
      return true;
    }
  }

  return false;
}

static inline bool ZLay_RenderNodeToggleLayer(
  ZLay_RenderNode* node,
  ZLay_EventString layer,
  bool force_on
) {
  if (force_on) {
    return ZLay_RenderNodeAddLayer(node, layer);
  }
  if (ZLay_RenderNodeHasLayer(node, layer)) {
    return ZLay_RenderNodeRemoveLayer(node, layer);
  }
  return ZLay_RenderNodeAddLayer(node, layer);
}

static inline bool ZLay_RenderNodeSetParam(
  ZLay_RenderNode* node,
  ZLay_EventString name,
  ZLay_EventString value
) {
  if (node == NULL || ZLay_EventStringIsEmpty(name)) {
    return false;
  }

  for (uint32_t i = 0u; i < node->style.param_count; ++i) {
    if (ZLay_EventStringEquals(node->style.params[i].name, name)) {
      node->style.params[i].value = value;
      node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_LAYOUT;
      return true;
    }
  }

  if (node->style.param_count >= ZLAY_RENDER_MAX_PARAMS) {
    return false;
  }

  node->style.params[node->style.param_count].name = name;
  node->style.params[node->style.param_count].value = value;
  ++node->style.param_count;
  node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_LAYOUT;
  return true;
}

static inline ZLay_EventString ZLay_RenderNodeGetParam(
  const ZLay_RenderNode* node,
  ZLay_EventString name
) {
  if (node == NULL) {
    return ZLay_EventStringEmpty();
  }

  for (uint32_t i = 0u; i < node->style.param_count; ++i) {
    if (ZLay_EventStringEquals(node->style.params[i].name, name)) {
      return node->style.params[i].value;
    }
  }

  return ZLay_EventStringEmpty();
}

static inline void ZLay_RenderNodeSetStyle(ZLay_RenderNode* node, ZLay_Style style) {
  if (node == NULL) {
    return;
  }

  node->style.layout = style;
  node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_LAYOUT;
}

static inline void ZLay_RenderNodeSetState(ZLay_RenderNode* node, uint32_t state, bool enabled) {
  if (node == NULL) {
    return;
  }

  if (enabled) {
    node->state |= state;
  } else {
    node->state &= ~state;
  }
  node->state |= ZLAY_RENDER_STATE_DIRTY_STYLE | ZLAY_RENDER_STATE_DIRTY_PAINT;
}

static inline ZLay_RenderNode* ZLay_RenderFindById(
  ZLay_RenderNode* root,
  ZLay_EventString id
) {
  if (root == NULL || ZLay_EventStringIsEmpty(id)) {
    return NULL;
  }
  if (ZLay_EventStringEquals(root->id, id)) {
    return root;
  }

  for (uint32_t i = 0u; i < root->child_count; ++i) {
    ZLay_RenderNode* found = ZLay_RenderFindById(root->children[i], id);
    if (found != NULL) {
      return found;
    }
  }

  return NULL;
}

static inline ZLay_RenderNode* ZLay_RenderFindByKind(
  ZLay_RenderNode* root,
  ZLay_EventString kind
) {
  if (root == NULL || ZLay_EventStringIsEmpty(kind)) {
    return NULL;
  }
  if (ZLay_EventStringEquals(root->kind, kind)) {
    return root;
  }

  for (uint32_t i = 0u; i < root->child_count; ++i) {
    ZLay_RenderNode* found = ZLay_RenderFindByKind(root->children[i], kind);
    if (found != NULL) {
      return found;
    }
  }

  return NULL;
}

static inline ZLay_RenderNode* ZLay_RenderFindByLayer(
  ZLay_RenderNode* root,
  ZLay_EventString layer
) {
  if (root == NULL || ZLay_EventStringIsEmpty(layer)) {
    return NULL;
  }
  if (ZLay_RenderNodeHasLayer(root, layer)) {
    return root;
  }

  for (uint32_t i = 0u; i < root->child_count; ++i) {
    ZLay_RenderNode* found = ZLay_RenderFindByLayer(root->children[i], layer);
    if (found != NULL) {
      return found;
    }
  }

  return NULL;
}

static inline void ZLay_RenderEventInit(
  ZLay_RenderEvent* event,
  ZLay_EventString type,
  uint32_t flags,
  void* detail
) {
  if (event == NULL) {
    return;
  }

  memset(event, 0, sizeof(*event));
  event->type = type;
  event->flags = flags;
  event->detail = detail;
}

static inline void ZLay_RenderPreventDefault(ZLay_RenderEvent* event) {
  if (event != NULL && (event->flags & ZLAY_RENDER_EVENT_CANCELABLE) != 0u) {
    event->default_prevented = true;
  }
}

static inline void ZLay_RenderStopRoute(ZLay_RenderEvent* event) {
  if (event != NULL) {
    event->route_stopped = true;
  }
}

static inline void ZLay_RenderStopImmediateRoute(ZLay_RenderEvent* event) {
  if (event != NULL) {
    event->route_stopped = true;
    event->immediate_route_stopped = true;
  }
}

static inline bool ZLay_RenderNodeAddEventListener(
  ZLay_RenderNode* node,
  ZLay_EventString type,
  ZLay_RenderEventListenerFn callback,
  void* user,
  uint32_t options
) {
  if (node == NULL || callback == NULL || ZLay_EventStringIsEmpty(type)) {
    return false;
  }

  for (uint32_t i = 0u; i < node->listener_count; ++i) {
    ZLay_RenderEventListener* listener = &node->listeners[i];
    if (!listener->active) {
      listener->type = type;
      listener->callback = callback;
      listener->user = user;
      listener->options = options;
      listener->active = true;
      return true;
    }
  }

  if (node->listener_count >= ZLAY_RENDER_MAX_EVENT_LISTENERS) {
    return false;
  }

  node->listeners[node->listener_count].type = type;
  node->listeners[node->listener_count].callback = callback;
  node->listeners[node->listener_count].user = user;
  node->listeners[node->listener_count].options = options;
  node->listeners[node->listener_count].active = true;
  ++node->listener_count;
  return true;
}

static inline bool ZLay_RenderNodeRemoveEventListener(
  ZLay_RenderNode* node,
  ZLay_EventString type,
  ZLay_RenderEventListenerFn callback,
  void* user
) {
  if (node == NULL || callback == NULL) {
    return false;
  }

  for (uint32_t i = 0u; i < node->listener_count; ++i) {
    ZLay_RenderEventListener* listener = &node->listeners[i];
    if (listener->active &&
        listener->callback == callback &&
        listener->user == user &&
        ZLay_EventStringEquals(listener->type, type)) {
      listener->active = false;
      return true;
    }
  }

  return false;
}

static inline void ZLay__RenderInvokeListeners(
  ZLay_RenderNode* node,
  ZLay_RenderEvent* event,
  bool descend
) {
  if (node == NULL || event == NULL) {
    return;
  }

  event->current_target = node;

  for (uint32_t i = 0u; i < node->listener_count; ++i) {
    ZLay_RenderEventListener* listener = &node->listeners[i];
    const bool listener_descend = (listener->options & ZLAY_RENDER_LISTENER_DESCEND) != 0u;

    if (!listener->active ||
        listener_descend != descend ||
        !ZLay_EventStringEquals(listener->type, event->type)) {
      continue;
    }

    listener->callback(event, listener->user);

    if ((listener->options & ZLAY_RENDER_LISTENER_ONCE) != 0u) {
      listener->active = false;
    }

    if (event->immediate_route_stopped) {
      return;
    }
  }
}

static inline bool ZLay_RenderDispatchEvent(
  ZLay_RenderNode* target,
  ZLay_RenderEvent* event
) {
  ZLay_RenderNode* path[ZLAY_RENDER_MAX_EVENT_PATH];
  uint32_t path_count = 0u;
  ZLay_RenderNode* cursor;

  if (target == NULL || event == NULL) {
    return false;
  }

  event->target = target;
  event->route = ZLAY_RENDER_EVENT_ROUTE_DESCEND;

  for (cursor = target; cursor != NULL && path_count < ZLAY_RENDER_MAX_EVENT_PATH; cursor = cursor->parent) {
    path[path_count++] = cursor;
  }

  for (uint32_t i = path_count; i > 1u; --i) {
    ZLay__RenderInvokeListeners(path[i - 1u], event, true);
    if (event->route_stopped) {
      event->route = ZLAY_RENDER_EVENT_ROUTE_NONE;
      return !event->default_prevented;
    }
  }

  event->route = ZLAY_RENDER_EVENT_ROUTE_TARGET;
  ZLay__RenderInvokeListeners(target, event, true);
  if (!event->immediate_route_stopped) {
    ZLay__RenderInvokeListeners(target, event, false);
  }

  if ((event->flags & ZLAY_RENDER_EVENT_ASCENDS) != 0u && !event->route_stopped) {
    event->route = ZLAY_RENDER_EVENT_ROUTE_ASCEND;
    for (uint32_t i = 1u; i < path_count; ++i) {
      ZLay__RenderInvokeListeners(path[i], event, false);
      if (event->route_stopped) {
        break;
      }
    }
  }

  event->route = ZLAY_RENDER_EVENT_ROUTE_NONE;
  event->current_target = NULL;
  return !event->default_prevented;
}

static inline void ZLay_RenderNodeMarkClean(ZLay_RenderNode* node) {
  if (node == NULL) {
    return;
  }

  node->state &= ~(ZLAY_RENDER_STATE_DIRTY_STYLE |
                   ZLAY_RENDER_STATE_DIRTY_LAYOUT |
                   ZLAY_RENDER_STATE_DIRTY_PAINT);
  for (uint32_t i = 0u; i < node->child_count; ++i) {
    ZLay_RenderNodeMarkClean(node->children[i]);
  }
}

#ifdef __cplusplus
}
#endif
