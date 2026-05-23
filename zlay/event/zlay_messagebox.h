#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <event/zlay_add_eventlistener.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_MESSAGEBOX_MAX_BUTTONS
#  define ZLAY_MESSAGEBOX_MAX_BUTTONS 4u
#endif

#define ZLAY_EVT_MESSAGEBOX_CLOSE ZLAY_EVENT_STRING_LITERAL("messageboxclose")
#define ZLAY_EVT_MESSAGEBOX_ACTION ZLAY_EVENT_STRING_LITERAL("messageboxaction")

typedef enum ZLay_MessageBoxKind {
  ZLAY_MESSAGEBOX_INFO = 0,
  ZLAY_MESSAGEBOX_SUCCESS,
  ZLAY_MESSAGEBOX_WARNING,
  ZLAY_MESSAGEBOX_ERROR,
  ZLAY_MESSAGEBOX_CONFIRM,
  ZLAY_MESSAGEBOX_PROMPT,
  ZLAY_MESSAGEBOX_TOAST
} ZLay_MessageBoxKind;

typedef enum ZLay_MessageBoxAction {
  ZLAY_MESSAGEBOX_ACTION_NONE = 0,
  ZLAY_MESSAGEBOX_ACTION_OK,
  ZLAY_MESSAGEBOX_ACTION_CANCEL,
  ZLAY_MESSAGEBOX_ACTION_YES,
  ZLAY_MESSAGEBOX_ACTION_NO,
  ZLAY_MESSAGEBOX_ACTION_CUSTOM
} ZLay_MessageBoxAction;

typedef struct ZLay_MessageBoxButton {
  ZLay_EventString label;
  ZLay_MessageBoxAction action;
  bool primary;
  bool destructive;
} ZLay_MessageBoxButton;

typedef struct ZLay_MessageBoxDesc {
  ZLay_MessageBoxKind kind;
  ZLay_EventString title;
  ZLay_EventString message;
  ZLay_EventString placeholder;
  ZLay_EventString default_value;
  ZLay_MessageBoxButton buttons[ZLAY_MESSAGEBOX_MAX_BUTTONS];
  uint32_t button_count;
  bool modal;
  double timeout_ms;
} ZLay_MessageBoxDesc;

typedef void (*ZLay_MessageBoxCallback)(
  const ZLay_MessageBoxDesc* desc,
  ZLay_MessageBoxAction action,
  ZLay_EventString value,
  void* user
);

typedef struct ZLay_MessageBox {
  ZLay_MessageBoxDesc desc;
  ZLay_RenderNode* host;
  ZLay_EventString value;
  ZLay_MessageBoxCallback callback;
  void* user;
  double elapsed_ms;
  bool open;
} ZLay_MessageBox;

static inline ZLay_MessageBoxDesc ZLay_MessageBoxDescDefault(ZLay_MessageBoxKind kind) {
  ZLay_MessageBoxDesc desc;
  memset(&desc, 0, sizeof(desc));
  desc.kind = kind;
  desc.modal = kind != ZLAY_MESSAGEBOX_TOAST;

  if (kind == ZLAY_MESSAGEBOX_CONFIRM) {
    desc.buttons[0].label = ZLAY_EVENT_STRING_LITERAL("Cancel");
    desc.buttons[0].action = ZLAY_MESSAGEBOX_ACTION_CANCEL;
    desc.buttons[1].label = ZLAY_EVENT_STRING_LITERAL("OK");
    desc.buttons[1].action = ZLAY_MESSAGEBOX_ACTION_OK;
    desc.buttons[1].primary = true;
    desc.button_count = 2u;
  } else {
    desc.buttons[0].label = ZLAY_EVENT_STRING_LITERAL("OK");
    desc.buttons[0].action = ZLAY_MESSAGEBOX_ACTION_OK;
    desc.buttons[0].primary = true;
    desc.button_count = 1u;
  }

  if (kind == ZLAY_MESSAGEBOX_TOAST) {
    desc.timeout_ms = 3200.0;
  }

  return desc;
}

static inline bool ZLay_MessageBoxAddButton(
  ZLay_MessageBoxDesc* desc,
  ZLay_EventString label,
  ZLay_MessageBoxAction action,
  bool primary
) {
  if (desc == NULL || desc->button_count >= ZLAY_MESSAGEBOX_MAX_BUTTONS) {
    return false;
  }

  desc->buttons[desc->button_count].label = label;
  desc->buttons[desc->button_count].action = action;
  desc->buttons[desc->button_count].primary = primary;
  desc->buttons[desc->button_count].destructive = false;
  ++desc->button_count;
  return true;
}

static inline void ZLay_MessageBoxOpen(
  ZLay_MessageBox* box,
  ZLay_RenderNode* host,
  const ZLay_MessageBoxDesc* desc,
  ZLay_MessageBoxCallback callback,
  void* user
) {
  if (box == NULL || desc == NULL) {
    return;
  }

  memset(box, 0, sizeof(*box));
  box->desc = *desc;
  box->host = host;
  box->value = desc->default_value;
  box->callback = callback;
  box->user = user;
  box->open = true;
  if (host != NULL) {
    host->state |= ZLAY_RENDER_STATE_DIRTY_LAYOUT | ZLAY_RENDER_STATE_DIRTY_PAINT;
  }
}

static inline void ZLay_MessageBoxSetValue(ZLay_MessageBox* box, ZLay_EventString value) {
  if (box != NULL) {
    box->value = value;
  }
}

static inline void ZLay_MessageBoxClose(ZLay_MessageBox* box, ZLay_MessageBoxAction action) {
  ZLay_RenderEvent event;

  if (box == NULL || !box->open) {
    return;
  }

  box->open = false;
  if (box->callback != NULL) {
    box->callback(&box->desc, action, box->value, box->user);
  }

  if (box->host != NULL) {
    ZLay_RenderEventInit(&event, ZLAY_EVT_MESSAGEBOX_CLOSE, ZLAY_RENDER_EVENT_ASCENDS, box);
    ZLay_RenderDispatchEvent(box->host, &event);
    box->host->state |= ZLAY_RENDER_STATE_DIRTY_LAYOUT | ZLAY_RENDER_STATE_DIRTY_PAINT;
  }
}

static inline void ZLay_MessageBoxActionPress(ZLay_MessageBox* box, ZLay_MessageBoxAction action) {
  ZLay_RenderEvent event;

  if (box == NULL || !box->open) {
    return;
  }

  if (box->host != NULL) {
    ZLay_RenderEventInit(&event, ZLAY_EVT_MESSAGEBOX_ACTION, ZLAY_RENDER_EVENT_ASCENDS, box);
    event.detail = (void*)(uintptr_t)action;
    ZLay_RenderDispatchEvent(box->host, &event);
  }

  ZLay_MessageBoxClose(box, action);
}

static inline void ZLay_MessageBoxTick(ZLay_MessageBox* box, double delta_ms) {
  if (box == NULL || !box->open || box->desc.timeout_ms <= 0.0) {
    return;
  }

  box->elapsed_ms += delta_ms;
  if (box->elapsed_ms >= box->desc.timeout_ms) {
    ZLay_MessageBoxClose(box, ZLAY_MESSAGEBOX_ACTION_NONE);
  }
}

#ifdef __cplusplus
}
#endif
