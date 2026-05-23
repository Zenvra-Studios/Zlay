#pragma once

#include <stdbool.h>
#include <string.h>

#include <event/zlay_add_eventlistener.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZLay_HoverUI {
  ZLay_RenderScene* scene;
  ZLay_RenderNode* hovered;
  ZLay_RenderNode* active;
  float last_x;
  float last_y;
  int pointer_id;
} ZLay_HoverUI;

static inline bool ZLay_HoverUIRectContains(ZLay_Rect rect, float x, float y) {
  return x >= rect.x && y >= rect.y && x < rect.x + rect.width && y < rect.y + rect.height;
}

static inline ZLay_RenderNode* ZLay_HoverUIHitTest(ZLay_RenderNode* root, float x, float y) {
  if (root == NULL || (root->state & ZLAY_RENDER_STATE_HIDDEN) != 0u) {
    return NULL;
  }

  for (uint32_t i = root->child_count; i > 0u; --i) {
    ZLay_RenderNode* child = root->children[i - 1u];
    ZLay_RenderNode* hit = ZLay_HoverUIHitTest(child, x, y);
    if (hit != NULL) {
      return hit;
    }
  }

  if (ZLay_HoverUIRectContains(root->layout, x, y)) {
    return root;
  }

  return NULL;
}

static inline void ZLay_HoverUIInit(ZLay_HoverUI* hover, ZLay_RenderScene* scene) {
  if (hover == NULL) {
    return;
  }

  memset(hover, 0, sizeof(*hover));
  hover->scene = scene;
  hover->pointer_id = 1;
}

static inline void ZLay_HoverUISetHovered(
  ZLay_HoverUI* hover,
  ZLay_RenderNode* node,
  float x,
  float y
) {
  ZLay_RenderEvent event;

  if (hover == NULL || hover->hovered == node) {
    return;
  }

  if (hover->hovered != NULL) {
    ZLay_RenderNodeSetState(hover->hovered, ZLAY_RENDER_STATE_HOVERED, false);
    ZLay_RenderEventInit(&event, ZLAY_EVT_POINTER_LEAVE, ZLAY_RENDER_EVENT_NONE, NULL);
    event.pointer_id = hover->pointer_id;
    event.client_x = x;
    event.client_y = y;
    ZLay_RenderDispatchEvent(hover->hovered, &event);
  }

  hover->hovered = node;

  if (hover->scene != NULL) {
    hover->scene->hovered_node = node;
  }

  if (node != NULL) {
    ZLay_RenderNodeSetState(node, ZLAY_RENDER_STATE_HOVERED, true);
    ZLay_RenderEventInit(&event, ZLAY_EVT_POINTER_ENTER, ZLAY_RENDER_EVENT_NONE, NULL);
    event.pointer_id = hover->pointer_id;
    event.client_x = x;
    event.client_y = y;
    ZLay_RenderDispatchEvent(node, &event);
  }
}

static inline ZLay_RenderNode* ZLay_HoverUIMove(ZLay_HoverUI* hover, float x, float y) {
  ZLay_RenderNode* root;
  ZLay_RenderNode* hit;

  if (hover == NULL || hover->scene == NULL) {
    return NULL;
  }

  root = hover->scene->root;
  hit = hover->scene->pointer_capture_node != NULL
    ? hover->scene->pointer_capture_node
    : ZLay_HoverUIHitTest(root, x, y);

  hover->last_x = x;
  hover->last_y = y;
  ZLay_HoverUISetHovered(hover, hit, x, y);

  if (hit != NULL) {
    ZLay_DispatchPointerEvent(hit, ZLAY_EVT_POINTER_MOVE, hover->pointer_id, x, y);
  }

  return hit;
}

static inline bool ZLay_HoverUIPointerDown(ZLay_HoverUI* hover, float x, float y) {
  ZLay_RenderNode* hit = ZLay_HoverUIMove(hover, x, y);
  if (hit == NULL) {
    return false;
  }

  hover->active = hit;
  ZLay_RenderNodeSetState(hit, ZLAY_RENDER_STATE_ACTIVE, true);
  return ZLay_DispatchPointerEvent(hit, ZLAY_EVT_POINTER_DOWN, hover->pointer_id, x, y);
}

static inline bool ZLay_HoverUIPointerUp(ZLay_HoverUI* hover, float x, float y) {
  ZLay_RenderNode* active;
  ZLay_RenderNode* hit;
  bool result = false;

  if (hover == NULL) {
    return false;
  }

  active = hover->active;
  hit = ZLay_HoverUIMove(hover, x, y);

  if (active != NULL) {
    ZLay_RenderNodeSetState(active, ZLAY_RENDER_STATE_ACTIVE, false);
    result = ZLay_DispatchPointerEvent(active, ZLAY_EVT_POINTER_UP, hover->pointer_id, x, y);
    if (active == hit) {
      result = ZLay_DispatchClick(active, x, y) && result;
    }
  }

  hover->active = NULL;
  return result;
}

static inline void ZLay_HoverUICapturePointer(ZLay_HoverUI* hover, ZLay_RenderNode* node) {
  if (hover != NULL && hover->scene != NULL) {
    hover->scene->pointer_capture_node = node;
  }
}

static inline void ZLay_HoverUIReleasePointer(ZLay_HoverUI* hover, ZLay_RenderNode* node) {
  if (hover != NULL &&
      hover->scene != NULL &&
      (node == NULL || hover->scene->pointer_capture_node == node)) {
    hover->scene->pointer_capture_node = NULL;
  }
}

#ifdef __cplusplus
}
#endif
