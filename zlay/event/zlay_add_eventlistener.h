#pragma once

#include <stdbool.h>

#include <event/zlay_event_handle.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZLAY_EVT_CLICK ZLAY_EVENT_STRING_LITERAL("click")
#define ZLAY_EVT_DBLCLICK ZLAY_EVENT_STRING_LITERAL("dblclick")
#define ZLAY_EVT_POINTER_DOWN ZLAY_EVENT_STRING_LITERAL("pointerdown")
#define ZLAY_EVT_POINTER_UP ZLAY_EVENT_STRING_LITERAL("pointerup")
#define ZLAY_EVT_POINTER_MOVE ZLAY_EVENT_STRING_LITERAL("pointermove")
#define ZLAY_EVT_POINTER_ENTER ZLAY_EVENT_STRING_LITERAL("pointerenter")
#define ZLAY_EVT_POINTER_LEAVE ZLAY_EVENT_STRING_LITERAL("pointerleave")
#define ZLAY_EVT_KEY_DOWN ZLAY_EVENT_STRING_LITERAL("keydown")
#define ZLAY_EVT_KEY_UP ZLAY_EVENT_STRING_LITERAL("keyup")
#define ZLAY_EVT_INPUT ZLAY_EVENT_STRING_LITERAL("input")
#define ZLAY_EVT_CHANGE ZLAY_EVENT_STRING_LITERAL("change")
#define ZLAY_EVT_SUBMIT ZLAY_EVENT_STRING_LITERAL("submit")
#define ZLAY_EVT_FOCUS ZLAY_EVENT_STRING_LITERAL("focus")
#define ZLAY_EVT_BLUR ZLAY_EVENT_STRING_LITERAL("blur")
#define ZLAY_EVT_FRAME_TICK ZLAY_EVENT_STRING_LITERAL("frametick")
#define ZLAY_EVT_ANIMATION_END ZLAY_EVENT_STRING_LITERAL("animationend")
#define ZLAY_EVT_NETWORK_LOAD ZLAY_EVENT_STRING_LITERAL("networkload")
#define ZLAY_EVT_NETWORK_ERROR ZLAY_EVENT_STRING_LITERAL("networkerror")

static inline bool ZLay_AddEventListener(
  ZLay_RenderNode* node,
  ZLay_EventString type,
  ZLay_RenderEventListenerFn callback,
  void* user
) {
  return ZLay_RenderNodeAddEventListener(node, type, callback, user, ZLAY_RENDER_LISTENER_NONE);
}

static inline bool ZLay_AddEventListenerEx(
  ZLay_RenderNode* node,
  ZLay_EventString type,
  ZLay_RenderEventListenerFn callback,
  void* user,
  uint32_t options
) {
  return ZLay_RenderNodeAddEventListener(node, type, callback, user, options);
}

static inline bool ZLay_RemoveEventListener(
  ZLay_RenderNode* node,
  ZLay_EventString type,
  ZLay_RenderEventListenerFn callback,
  void* user
) {
  return ZLay_RenderNodeRemoveEventListener(node, type, callback, user);
}

static inline bool ZLay_OnClick(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_CLICK, callback, user);
}

static inline bool ZLay_OnPointerDown(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_POINTER_DOWN, callback, user);
}

static inline bool ZLay_OnPointerUp(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_POINTER_UP, callback, user);
}

static inline bool ZLay_OnPointerMove(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_POINTER_MOVE, callback, user);
}

static inline bool ZLay_OnPointerEnter(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_POINTER_ENTER, callback, user);
}

static inline bool ZLay_OnPointerLeave(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_POINTER_LEAVE, callback, user);
}

static inline bool ZLay_OnInput(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_INPUT, callback, user);
}

static inline bool ZLay_OnChange(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_CHANGE, callback, user);
}

static inline bool ZLay_OnSubmit(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_SUBMIT, callback, user);
}

static inline bool ZLay_OnKeyDown(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_KEY_DOWN, callback, user);
}

static inline bool ZLay_OnKeyUp(ZLay_RenderNode* node, ZLay_RenderEventListenerFn callback, void* user) {
  return ZLay_AddEventListener(node, ZLAY_EVT_KEY_UP, callback, user);
}

static inline bool ZLay_DispatchSimpleEvent(
  ZLay_RenderNode* target,
  ZLay_EventString type,
  uint32_t flags,
  void* detail
) {
  ZLay_RenderEvent event;
  ZLay_RenderEventInit(&event, type, flags, detail);
  return ZLay_RenderDispatchEvent(target, &event);
}

static inline bool ZLay_DispatchClick(ZLay_RenderNode* target, float x, float y) {
  ZLay_RenderEvent event;
  ZLay_RenderEventInit(&event, ZLAY_EVT_CLICK, ZLAY_RENDER_EVENT_ASCENDS | ZLAY_RENDER_EVENT_CANCELABLE, NULL);
  event.client_x = x;
  event.client_y = y;
  return ZLay_RenderDispatchEvent(target, &event);
}

static inline bool ZLay_DispatchPointerEvent(
  ZLay_RenderNode* target,
  ZLay_EventString type,
  int pointer_id,
  float x,
  float y
) {
  ZLay_RenderEvent event;
  ZLay_RenderEventInit(&event, type, ZLAY_RENDER_EVENT_ASCENDS | ZLAY_RENDER_EVENT_CANCELABLE, NULL);
  event.pointer_id = pointer_id;
  event.client_x = x;
  event.client_y = y;
  return ZLay_RenderDispatchEvent(target, &event);
}

static inline bool ZLay_DispatchKeyboardEvent(
  ZLay_RenderNode* target,
  ZLay_EventString type,
  int key_code,
  uint32_t key_mods
) {
  ZLay_RenderEvent event;
  ZLay_RenderEventInit(&event, type, ZLAY_RENDER_EVENT_ASCENDS | ZLAY_RENDER_EVENT_CANCELABLE, NULL);
  event.key_code = key_code;
  event.key_mods = key_mods;
  return ZLay_RenderDispatchEvent(target, &event);
}

#ifdef __cplusplus
}
#endif
