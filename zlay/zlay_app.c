#include <zlay.h>

#include <string.h>

static bool ZLay__StringEquals(ZLay_String a, ZLay_String b) {
  if (a.length != b.length) {
    return false;
  }

  if (a.length == 0u) {
    return true;
  }

  if (a.chars == NULL || b.chars == NULL) {
    return false;
  }

  return memcmp(a.chars, b.chars, a.length) == 0;
}

static bool ZLay__StateValueEquals(ZLay_StateValue a, ZLay_StateValue b) {
  if (a.type != b.type) {
    return false;
  }

  switch (a.type) {
    case ZLAY_STATE_NONE:
      return true;
    case ZLAY_STATE_BOOL:
      return a.as.bool_value == b.as.bool_value;
    case ZLAY_STATE_INT:
      return a.as.int_value == b.as.int_value;
    case ZLAY_STATE_FLOAT:
      return a.as.float_value == b.as.float_value;
    case ZLAY_STATE_STRING:
      return ZLay__StringEquals(a.as.string_value, b.as.string_value);
    case ZLAY_STATE_POINTER:
      return a.as.pointer_value == b.as.pointer_value;
  }

  return false;
}

static ZLay_Route* ZLay__NavigatorMutableCurrent(ZLay_App* app) {
  if (app == NULL || app->navigator.count == 0u || app->navigator.stack == NULL) {
    return NULL;
  }

  return &app->navigator.stack[app->navigator.count - 1u];
}

static const ZLay_Route* ZLay__AppCurrentOrHome(const ZLay_App* app) {
  if (app == NULL) {
    return NULL;
  }

  if (app->navigator.count > 0u && app->navigator.stack != NULL) {
    return &app->navigator.stack[app->navigator.count - 1u];
  }

  if (app->home_route.build != NULL) {
    return &app->home_route;
  }

  return NULL;
}

ZLay_AppConfig ZLay_AppConfigDefault(void) {
  ZLay_AppConfig config;
  memset(&config, 0, sizeof(config));
  config.title = ZLAY_STRING_LITERAL("ZLay App");
  config.theme = *ZLay_ThemeDefault();
  return config;
}

bool ZLay_AppInit(ZLay_App* app, ZLay_Context* layout, ZLay_AppConfig config) {
  if (app == NULL || layout == NULL) {
    return false;
  }

  memset(app, 0, sizeof(*app));
  app->layout = layout;
  app->config = config;
  app->theme = config.theme.space_1 > 0.0f ? config.theme : *ZLay_ThemeDefault();
  app->navigator.stack = config.route_stack;
  app->navigator.capacity = config.route_capacity;
  app->navigator.count = 0u;
  app->home_route = (ZLay_Route){ZLAY_STRING_LITERAL("/"), config.home, config.home_user};
  app->lifecycle_state = ZLAY_APP_DETACHED;
  app->state_revision = 1u;
  app->needs_build = true;

  if (config.state_slots != NULL && config.state_capacity > 0u) {
    memset(config.state_slots, 0, sizeof(config.state_slots[0]) * (size_t)config.state_capacity);
  }

  if (config.home != NULL && config.route_stack != NULL && config.route_capacity > 0u) {
    app->navigator.stack[0] = app->home_route;
    app->navigator.count = 1u;
  }

  return ZLay_AppSetLifecycleState(app, ZLAY_APP_RESUMED);
}

bool ZLay_AppSetLifecycleState(ZLay_App* app, ZLay_AppLifecycleState state) {
  if (app == NULL) {
    return false;
  }

  if (app->lifecycle_state == state) {
    return true;
  }

  app->lifecycle_state = state;
  if (app->config.lifecycle != NULL) {
    app->config.lifecycle(app, state, app->config.user);
  }

  return true;
}

void ZLay_AppSetTheme(ZLay_App* app, ZLay_Theme theme) {
  if (app == NULL) {
    return;
  }

  app->theme = theme;
  ZLay_AppMarkNeedsBuild(app);
}

const ZLay_Theme* ZLay_AppTheme(const ZLay_App* app) {
  if (app != NULL) {
    return &app->theme;
  }

  return ZLay_ThemeDefault();
}

void ZLay_AppMarkNeedsBuild(ZLay_App* app) {
  if (app != NULL) {
    app->needs_build = true;
  }
}

bool ZLay_AppNeedsBuild(const ZLay_App* app) {
  return app != NULL && app->needs_build;
}

bool ZLay_AppBeginFrame(ZLay_App* app, ZLay_Dimensions viewport, double delta_ms) {
  if (app == NULL || app->layout == NULL) {
    return false;
  }

  app->viewport = viewport;
  app->delta_ms = delta_ms;
  ++app->frame_index;
  ZLay_SetLayoutDimensions(app->layout, viewport);
  ZLay_BeginLayout(app->layout);
  return true;
}

bool ZLay_AppBuild(ZLay_App* app) {
  const ZLay_Route* route;

  if (app == NULL || app->layout == NULL) {
    return false;
  }

  route = ZLay__AppCurrentOrHome(app);
  if (route == NULL || route->build == NULL) {
    return false;
  }

  app->build_context.app = app;
  app->build_context.layout = app->layout;
  app->build_context.theme = &app->theme;
  app->build_context.route = route;
  app->build_context.viewport = app->viewport;
  app->build_context.delta_ms = app->delta_ms;
  app->build_context.frame_index = app->frame_index;
  app->build_context.user = app->config.user;

  route->build(&app->build_context, route->user);
  app->needs_build = false;
  return true;
}

ZLay_RenderCommandList ZLay_AppEndFrame(ZLay_App* app) {
  if (app == NULL || app->layout == NULL) {
    return (ZLay_RenderCommandList){0};
  }

  return ZLay_EndLayout(app->layout);
}

ZLay_RenderCommandList ZLay_AppRunFrame(ZLay_App* app, ZLay_Dimensions viewport, double delta_ms) {
  if (!ZLay_AppBeginFrame(app, viewport, delta_ms)) {
    return (ZLay_RenderCommandList){0};
  }

  (void)ZLay_AppBuild(app);
  return ZLay_AppEndFrame(app);
}

bool ZLay_NavigatorPush(ZLay_App* app, ZLay_Route route) {
  if (app == NULL || app->navigator.stack == NULL || app->navigator.count >= app->navigator.capacity) {
    return false;
  }

  app->navigator.stack[app->navigator.count++] = route;
  ZLay_AppMarkNeedsBuild(app);
  return true;
}

bool ZLay_NavigatorReplace(ZLay_App* app, ZLay_Route route) {
  ZLay_Route* current;

  if (app == NULL || app->navigator.stack == NULL || app->navigator.capacity == 0u) {
    return false;
  }

  current = ZLay__NavigatorMutableCurrent(app);
  if (current == NULL) {
    app->navigator.stack[0] = route;
    app->navigator.count = 1u;
  } else {
    *current = route;
  }

  ZLay_AppMarkNeedsBuild(app);
  return true;
}

bool ZLay_NavigatorPop(ZLay_App* app) {
  if (!ZLay_NavigatorCanPop(app)) {
    return false;
  }

  --app->navigator.count;
  ZLay_AppMarkNeedsBuild(app);
  return true;
}

bool ZLay_NavigatorCanPop(const ZLay_App* app) {
  return app != NULL && app->navigator.count > 1u;
}

const ZLay_Route* ZLay_NavigatorCurrent(const ZLay_App* app) {
  return ZLay__AppCurrentOrHome(app);
}

ZLay_StateValue ZLay_StateNone(void) {
  return (ZLay_StateValue){0};
}

ZLay_StateValue ZLay_StateBool(bool value) {
  ZLay_StateValue state = ZLay_StateNone();
  state.type = ZLAY_STATE_BOOL;
  state.as.bool_value = value;
  return state;
}

ZLay_StateValue ZLay_StateInt(int64_t value) {
  ZLay_StateValue state = ZLay_StateNone();
  state.type = ZLAY_STATE_INT;
  state.as.int_value = value;
  return state;
}

ZLay_StateValue ZLay_StateFloat(double value) {
  ZLay_StateValue state = ZLay_StateNone();
  state.type = ZLAY_STATE_FLOAT;
  state.as.float_value = value;
  return state;
}

ZLay_StateValue ZLay_StateString(ZLay_String value) {
  ZLay_StateValue state = ZLay_StateNone();
  state.type = ZLAY_STATE_STRING;
  state.as.string_value = value;
  return state;
}

ZLay_StateValue ZLay_StatePointer(void* value) {
  ZLay_StateValue state = ZLay_StateNone();
  state.type = ZLAY_STATE_POINTER;
  state.as.pointer_value = value;
  return state;
}

ZLay_StateSlot* ZLay_AppUseState(ZLay_App* app, ZLay_Id key, ZLay_StateValue initial_value) {
  ZLay_StateSlot* free_slot = NULL;

  if (app == NULL || app->config.state_slots == NULL || app->config.state_capacity == 0u || key.hash == 0u) {
    return NULL;
  }

  for (uint32_t i = 0u; i < app->config.state_capacity; ++i) {
    ZLay_StateSlot* slot = &app->config.state_slots[i];
    if (slot->active && slot->key.hash == key.hash) {
      return slot;
    }
    if (!slot->active && free_slot == NULL) {
      free_slot = slot;
    }
  }

  if (free_slot == NULL) {
    return NULL;
  }

  free_slot->key = key;
  free_slot->value = initial_value;
  free_slot->revision = app->state_revision++;
  free_slot->active = true;
  free_slot->dirty = false;
  return free_slot;
}

const ZLay_StateSlot* ZLay_AppFindState(const ZLay_App* app, ZLay_Id key) {
  if (app == NULL || app->config.state_slots == NULL || key.hash == 0u) {
    return NULL;
  }

  for (uint32_t i = 0u; i < app->config.state_capacity; ++i) {
    const ZLay_StateSlot* slot = &app->config.state_slots[i];
    if (slot->active && slot->key.hash == key.hash) {
      return slot;
    }
  }

  return NULL;
}

bool ZLay_AppSetStateValue(ZLay_App* app, ZLay_Id key, ZLay_StateValue value) {
  ZLay_StateSlot* slot = ZLay_AppUseState(app, key, value);
  if (slot == NULL) {
    return false;
  }

  if (!ZLay__StateValueEquals(slot->value, value)) {
    slot->value = value;
    slot->revision = app->state_revision++;
    slot->dirty = true;
    ZLay_AppMarkNeedsBuild(app);
  }

  return true;
}

bool ZLay_AppClearState(ZLay_App* app, ZLay_Id key) {
  if (app == NULL || app->config.state_slots == NULL || key.hash == 0u) {
    return false;
  }

  for (uint32_t i = 0u; i < app->config.state_capacity; ++i) {
    ZLay_StateSlot* slot = &app->config.state_slots[i];
    if (slot->active && slot->key.hash == key.hash) {
      *slot = (ZLay_StateSlot){0};
      ZLay_AppMarkNeedsBuild(app);
      return true;
    }
  }

  return false;
}

void ZLay_AnimationControllerInit(ZLay_AnimationController* controller, double duration_ms) {
  if (controller == NULL) {
    return;
  }

  controller->duration_ms = duration_ms > 0.0 ? duration_ms : 1.0;
  controller->elapsed_ms = 0.0;
  controller->value = 0.0f;
  controller->status = ZLAY_ANIMATION_DISMISSED;
  controller->repeat = false;
}

void ZLay_AnimationForward(ZLay_AnimationController* controller) {
  if (controller == NULL) {
    return;
  }

  controller->status = ZLAY_ANIMATION_FORWARD;
}

void ZLay_AnimationReverse(ZLay_AnimationController* controller) {
  if (controller == NULL) {
    return;
  }

  controller->status = ZLAY_ANIMATION_REVERSE;
}

void ZLay_AnimationStop(ZLay_AnimationController* controller) {
  if (controller != NULL) {
    controller->status = ZLAY_ANIMATION_STOPPED;
  }
}

void ZLay_AnimationReset(ZLay_AnimationController* controller) {
  if (controller == NULL) {
    return;
  }

  controller->elapsed_ms = 0.0;
  controller->value = 0.0f;
  controller->status = ZLAY_ANIMATION_DISMISSED;
}

bool ZLay_AnimationTick(ZLay_AnimationController* controller, double delta_ms) {
  double step;

  if (controller == NULL || delta_ms <= 0.0) {
    return false;
  }

  if (controller->status != ZLAY_ANIMATION_FORWARD && controller->status != ZLAY_ANIMATION_REVERSE) {
    return false;
  }

  step = delta_ms / controller->duration_ms;
  if (controller->status == ZLAY_ANIMATION_FORWARD) {
    controller->value += (float)step;
    if (controller->value >= 1.0f) {
      if (controller->repeat) {
        controller->value = 0.0f;
      } else {
        controller->value = 1.0f;
        controller->status = ZLAY_ANIMATION_COMPLETED;
      }
    }
  } else {
    controller->value -= (float)step;
    if (controller->value <= 0.0f) {
      if (controller->repeat) {
        controller->value = 1.0f;
      } else {
        controller->value = 0.0f;
        controller->status = ZLAY_ANIMATION_DISMISSED;
      }
    }
  }

  controller->elapsed_ms += delta_ms;
  return true;
}

float ZLay_AnimationValue(const ZLay_AnimationController* controller) {
  if (controller == NULL) {
    return 0.0f;
  }

  return controller->value;
}
