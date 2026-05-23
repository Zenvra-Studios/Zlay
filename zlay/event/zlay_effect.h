#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <event/zlay_event_handle.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_EFFECT_MAX_EFFECTS
#  define ZLAY_EFFECT_MAX_EFFECTS 64u
#endif

typedef enum ZLay_SignalType {
  ZLAY_SIGNAL_EMPTY = 0,
  ZLAY_SIGNAL_BOOL,
  ZLAY_SIGNAL_NUMBER,
  ZLAY_SIGNAL_STRING,
  ZLAY_SIGNAL_POINTER
} ZLay_SignalType;

typedef struct ZLay_Signal {
  ZLay_SignalType type;
  uint32_t version;
  bool bool_value;
  double number_value;
  ZLay_EventString string_value;
  void* pointer_value;
} ZLay_Signal;

typedef void (*ZLay_EffectFn)(ZLay_RenderScene* scene, void* user);

typedef struct ZLay_Effect {
  ZLay_EffectFn callback;
  void* user;
  uint32_t observed_version;
  bool active;
} ZLay_Effect;

typedef struct ZLay_EffectScope {
  ZLay_RenderScene* scene;
  ZLay_Effect effects[ZLAY_EFFECT_MAX_EFFECTS];
} ZLay_EffectScope;

static inline void ZLay_SignalInit(ZLay_Signal* signal) {
  if (signal != NULL) {
    memset(signal, 0, sizeof(*signal));
  }
}

static inline void ZLay_SignalSetBool(ZLay_Signal* signal, bool value) {
  if (signal == NULL) {
    return;
  }

  signal->type = ZLAY_SIGNAL_BOOL;
  signal->bool_value = value;
  ++signal->version;
}

static inline void ZLay_SignalSetNumber(ZLay_Signal* signal, double value) {
  if (signal == NULL) {
    return;
  }

  signal->type = ZLAY_SIGNAL_NUMBER;
  signal->number_value = value;
  ++signal->version;
}

static inline void ZLay_SignalSetString(ZLay_Signal* signal, ZLay_EventString value) {
  if (signal == NULL) {
    return;
  }

  signal->type = ZLAY_SIGNAL_STRING;
  signal->string_value = value;
  ++signal->version;
}

static inline void ZLay_SignalSetPointer(ZLay_Signal* signal, void* value) {
  if (signal == NULL) {
    return;
  }

  signal->type = ZLAY_SIGNAL_POINTER;
  signal->pointer_value = value;
  ++signal->version;
}

static inline void ZLay_EffectScopeInit(ZLay_EffectScope* scope, ZLay_RenderScene* scene) {
  if (scope == NULL) {
    return;
  }

  memset(scope, 0, sizeof(*scope));
  scope->scene = scene;
}

static inline bool ZLay_EffectAdd(ZLay_EffectScope* scope, ZLay_EffectFn callback, void* user) {
  if (scope == NULL || callback == NULL) {
    return false;
  }

  for (uint32_t i = 0u; i < ZLAY_EFFECT_MAX_EFFECTS; ++i) {
    if (!scope->effects[i].active) {
      scope->effects[i].callback = callback;
      scope->effects[i].user = user;
      scope->effects[i].active = true;
      callback(scope->scene, user);
      return true;
    }
  }

  return false;
}

static inline void ZLay_EffectRunAll(ZLay_EffectScope* scope) {
  if (scope == NULL) {
    return;
  }

  for (uint32_t i = 0u; i < ZLAY_EFFECT_MAX_EFFECTS; ++i) {
    if (scope->effects[i].active && scope->effects[i].callback != NULL) {
      scope->effects[i].callback(scope->scene, scope->effects[i].user);
    }
  }
}

static inline void ZLay_EffectRunWhenSignalChanged(
  ZLay_EffectScope* scope,
  const ZLay_Signal* signal
) {
  if (scope == NULL || signal == NULL) {
    return;
  }

  for (uint32_t i = 0u; i < ZLAY_EFFECT_MAX_EFFECTS; ++i) {
    if (!scope->effects[i].active || scope->effects[i].callback == NULL) {
      continue;
    }

    if (scope->effects[i].observed_version != signal->version) {
      scope->effects[i].observed_version = signal->version;
      scope->effects[i].callback(scope->scene, scope->effects[i].user);
    }
  }
}

static inline void ZLay_BindTextToSignal(ZLay_RenderNode* node, const ZLay_Signal* signal) {
  if (node == NULL || signal == NULL || signal->type != ZLAY_SIGNAL_STRING) {
    return;
  }

  ZLay_RenderNodeSetText(node, signal->string_value);
}

static inline void ZLay_BindVisibleToSignal(ZLay_RenderNode* node, const ZLay_Signal* signal) {
  if (node == NULL || signal == NULL || signal->type != ZLAY_SIGNAL_BOOL) {
    return;
  }

  ZLay_RenderNodeSetState(node, ZLAY_RENDER_STATE_HIDDEN, !signal->bool_value);
}

#ifdef __cplusplus
}
#endif
