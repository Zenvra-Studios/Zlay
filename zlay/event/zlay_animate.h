#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <event/zlay_add_eventlistener.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ZLAY_ANIMATION_MAX_ITEMS
#  define ZLAY_ANIMATION_MAX_ITEMS 128u
#endif

#ifndef ZLAY_TIMER_MAX_ITEMS
#  define ZLAY_TIMER_MAX_ITEMS 128u
#endif

typedef enum ZLay_Easing {
  ZLAY_EASING_LINEAR = 0,
  ZLAY_EASING_EASE_IN,
  ZLAY_EASING_EASE_OUT,
  ZLAY_EASING_EASE_IN_OUT
} ZLay_Easing;

typedef enum ZLay_AnimationProperty {
  ZLAY_ANIMATE_OPACITY = 0,
  ZLAY_ANIMATE_X,
  ZLAY_ANIMATE_Y,
  ZLAY_ANIMATE_WIDTH,
  ZLAY_ANIMATE_HEIGHT,
  ZLAY_ANIMATE_RADIUS,
  ZLAY_ANIMATE_BACKGROUND_R,
  ZLAY_ANIMATE_BACKGROUND_G,
  ZLAY_ANIMATE_BACKGROUND_B,
  ZLAY_ANIMATE_BACKGROUND_A
} ZLay_AnimationProperty;

typedef void (*ZLay_TimerFn)(double now_ms, void* user);
typedef void (*ZLay_AnimationFinishFn)(ZLay_RenderNode* target, void* user);

typedef struct ZLay_Animation {
  uint32_t id;
  ZLay_RenderNode* target;
  ZLay_AnimationProperty property;
  float from;
  float to;
  double delay_ms;
  double duration_ms;
  double elapsed_ms;
  ZLay_Easing easing;
  bool running;
  bool finished;
  ZLay_AnimationFinishFn on_finish;
  void* user;
} ZLay_Animation;

typedef struct ZLay_Timer {
  uint32_t id;
  double due_ms;
  double interval_ms;
  bool repeats;
  bool active;
  ZLay_TimerFn callback;
  void* user;
} ZLay_Timer;

typedef struct ZLay_Animator {
  ZLay_Animation animations[ZLAY_ANIMATION_MAX_ITEMS];
  ZLay_Timer timers[ZLAY_TIMER_MAX_ITEMS];
  uint32_t next_animation_id;
  uint32_t next_timer_id;
  double now_ms;
} ZLay_Animator;

static inline void ZLay_AnimatorInit(ZLay_Animator* animator) {
  if (animator == NULL) {
    return;
  }

  memset(animator, 0, sizeof(*animator));
  animator->next_animation_id = 1u;
  animator->next_timer_id = 1u;
}

static inline float ZLay_Ease(ZLay_Easing easing, float t) {
  if (t <= 0.0f) {
    return 0.0f;
  }
  if (t >= 1.0f) {
    return 1.0f;
  }

  switch (easing) {
    case ZLAY_EASING_EASE_IN:
      return t * t;
    case ZLAY_EASING_EASE_OUT:
      return 1.0f - ((1.0f - t) * (1.0f - t));
    case ZLAY_EASING_EASE_IN_OUT:
      return t < 0.5f ? 2.0f * t * t : 1.0f - ((-2.0f * t + 2.0f) * (-2.0f * t + 2.0f) * 0.5f);
    case ZLAY_EASING_LINEAR:
    default:
      return t;
  }
}

static inline void ZLay__AnimationApply(ZLay_Animation* animation, float value) {
  ZLay_RenderNode* target;

  if (animation == NULL || animation->target == NULL) {
    return;
  }

  target = animation->target;

  switch (animation->property) {
    case ZLAY_ANIMATE_X:
      target->layout.x = value;
      break;
    case ZLAY_ANIMATE_Y:
      target->layout.y = value;
      break;
    case ZLAY_ANIMATE_WIDTH:
      target->layout.width = value;
      target->style.layout.width = ZLay_Px(value);
      break;
    case ZLAY_ANIMATE_HEIGHT:
      target->layout.height = value;
      target->style.layout.height = ZLay_Px(value);
      break;
    case ZLAY_ANIMATE_RADIUS:
      target->style.layout.radius = value;
      break;
    case ZLAY_ANIMATE_BACKGROUND_R:
      target->style.layout.background.r = (uint8_t)(value < 0.0f ? 0.0f : value > 255.0f ? 255.0f : value);
      break;
    case ZLAY_ANIMATE_BACKGROUND_G:
      target->style.layout.background.g = (uint8_t)(value < 0.0f ? 0.0f : value > 255.0f ? 255.0f : value);
      break;
    case ZLAY_ANIMATE_BACKGROUND_B:
      target->style.layout.background.b = (uint8_t)(value < 0.0f ? 0.0f : value > 255.0f ? 255.0f : value);
      break;
    case ZLAY_ANIMATE_BACKGROUND_A:
    case ZLAY_ANIMATE_OPACITY:
      target->style.layout.background.a = (uint8_t)(value < 0.0f ? 0.0f : value > 255.0f ? 255.0f : value);
      break;
    default:
      break;
  }

  target->state |= ZLAY_RENDER_STATE_DIRTY_LAYOUT | ZLAY_RENDER_STATE_DIRTY_PAINT;
}

static inline uint32_t ZLay_Animate(
  ZLay_Animator* animator,
  ZLay_RenderNode* target,
  ZLay_AnimationProperty property,
  float from,
  float to,
  double duration_ms,
  ZLay_Easing easing
) {
  if (animator == NULL || target == NULL || duration_ms <= 0.0) {
    return 0u;
  }

  for (uint32_t i = 0u; i < ZLAY_ANIMATION_MAX_ITEMS; ++i) {
    ZLay_Animation* animation = &animator->animations[i];
    if (!animation->running) {
      memset(animation, 0, sizeof(*animation));
      animation->id = animator->next_animation_id++;
      animation->target = target;
      animation->property = property;
      animation->from = from;
      animation->to = to;
      animation->duration_ms = duration_ms;
      animation->easing = easing;
      animation->running = true;
      ZLay__AnimationApply(animation, from);
      return animation->id;
    }
  }

  return 0u;
}

static inline bool ZLay_CancelAnimation(ZLay_Animator* animator, uint32_t id) {
  if (animator == NULL || id == 0u) {
    return false;
  }

  for (uint32_t i = 0u; i < ZLAY_ANIMATION_MAX_ITEMS; ++i) {
    if (animator->animations[i].id == id && animator->animations[i].running) {
      animator->animations[i].running = false;
      animator->animations[i].finished = true;
      return true;
    }
  }

  return false;
}

static inline uint32_t ZLay_SetTimeout(
  ZLay_Animator* animator,
  ZLay_TimerFn callback,
  double delay_ms,
  void* user
) {
  if (animator == NULL || callback == NULL) {
    return 0u;
  }

  for (uint32_t i = 0u; i < ZLAY_TIMER_MAX_ITEMS; ++i) {
    ZLay_Timer* timer = &animator->timers[i];
    if (!timer->active) {
      memset(timer, 0, sizeof(*timer));
      timer->id = animator->next_timer_id++;
      timer->due_ms = animator->now_ms + delay_ms;
      timer->active = true;
      timer->callback = callback;
      timer->user = user;
      return timer->id;
    }
  }

  return 0u;
}

static inline uint32_t ZLay_SetInterval(
  ZLay_Animator* animator,
  ZLay_TimerFn callback,
  double interval_ms,
  void* user
) {
  uint32_t id = ZLay_SetTimeout(animator, callback, interval_ms, user);
  if (id == 0u) {
    return 0u;
  }

  for (uint32_t i = 0u; i < ZLAY_TIMER_MAX_ITEMS; ++i) {
    if (animator->timers[i].id == id) {
      animator->timers[i].interval_ms = interval_ms;
      animator->timers[i].repeats = true;
      break;
    }
  }

  return id;
}

static inline bool ZLay_ClearTimer(ZLay_Animator* animator, uint32_t id) {
  if (animator == NULL || id == 0u) {
    return false;
  }

  for (uint32_t i = 0u; i < ZLAY_TIMER_MAX_ITEMS; ++i) {
    if (animator->timers[i].id == id && animator->timers[i].active) {
      animator->timers[i].active = false;
      return true;
    }
  }

  return false;
}

static inline uint32_t ZLay_QueueFrameTimer(
  ZLay_Animator* animator,
  ZLay_TimerFn callback,
  void* user
) {
  return ZLay_SetTimeout(animator, callback, 0.0, user);
}

static inline void ZLay_AnimatorTick(ZLay_Animator* animator, double delta_ms) {
  if (animator == NULL) {
    return;
  }

  animator->now_ms += delta_ms;

  for (uint32_t i = 0u; i < ZLAY_TIMER_MAX_ITEMS; ++i) {
    ZLay_Timer* timer = &animator->timers[i];
    if (timer->active && animator->now_ms >= timer->due_ms) {
      timer->callback(animator->now_ms, timer->user);
      if (timer->repeats) {
        timer->due_ms += timer->interval_ms;
      } else {
        timer->active = false;
      }
    }
  }

  for (uint32_t i = 0u; i < ZLAY_ANIMATION_MAX_ITEMS; ++i) {
    ZLay_Animation* animation = &animator->animations[i];
    float t;
    float value;

    if (!animation->running) {
      continue;
    }

    animation->elapsed_ms += delta_ms;
    if (animation->elapsed_ms < animation->delay_ms) {
      continue;
    }

    t = (float)((animation->elapsed_ms - animation->delay_ms) / animation->duration_ms);
    t = ZLay_Ease(animation->easing, t);
    value = animation->from + ((animation->to - animation->from) * t);
    ZLay__AnimationApply(animation, value);

    if (animation->elapsed_ms - animation->delay_ms >= animation->duration_ms) {
      ZLay_RenderEvent event;
      ZLay__AnimationApply(animation, animation->to);
      animation->running = false;
      animation->finished = true;
      if (animation->on_finish != NULL) {
        animation->on_finish(animation->target, animation->user);
      }
      ZLay_RenderEventInit(&event, ZLAY_EVT_ANIMATION_END, ZLAY_RENDER_EVENT_ASCENDS, animation);
      ZLay_RenderDispatchEvent(animation->target, &event);
    }
  }
}

#ifdef __cplusplus
}
#endif
