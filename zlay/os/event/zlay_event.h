#ifndef ZLAY_OS_EVENT_ZLAY_EVENT_H
#define ZLAY_OS_EVENT_ZLAY_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef struct ZLay_Window ZLay_Window;

typedef enum ZLay_EventType {
  ZLAY_EVENT_NONE = 0,

  ZLAY_EVENT_WINDOW_CLOSE,
  ZLAY_EVENT_WINDOW_RESIZE,
  ZLAY_EVENT_WINDOW_MOVE,
  ZLAY_EVENT_WINDOW_FOCUS,
  ZLAY_EVENT_WINDOW_BLUR,
  ZLAY_EVENT_WINDOW_DPI_CHANGED,

  ZLAY_EVENT_MOUSE_MOVE,
  ZLAY_EVENT_MOUSE_BUTTON_DOWN,
  ZLAY_EVENT_MOUSE_BUTTON_UP,
  ZLAY_EVENT_MOUSE_WHEEL,
  ZLAY_EVENT_MOUSE_ENTER,
  ZLAY_EVENT_MOUSE_LEAVE,

  ZLAY_EVENT_KEY_DOWN,
  ZLAY_EVENT_KEY_UP,
  ZLAY_EVENT_TEXT_INPUT,

  ZLAY_EVENT_TOUCH_DOWN,
  ZLAY_EVENT_TOUCH_MOVE,
  ZLAY_EVENT_TOUCH_UP,

  ZLAY_EVENT_DROP_FILE
} ZLay_EventType;

typedef struct ZLay_Event {
  ZLay_EventType type;
  double timestamp;

  union {
    struct {
      uint32_t width;
      uint32_t height;
    } resize;

    struct {
      int32_t x;
      int32_t y;
    } move;

    struct {
      float scale;
    } dpi;

    struct {
      float x;
      float y;
      float dx;
      float dy;
    } mouse_move;

    struct {
      int button;
      float x;
      float y;
    } mouse_button;

    struct {
      float x;
      float y;
    } wheel;

    struct {
      int key;
      int scancode;
      bool repeat;
      uint32_t modifiers;
    } key;

    struct {
      uint32_t codepoint;
    } text;

    struct {
      uint64_t id;
      float x;
      float y;
    } touch;

    struct {
      const char* path;
    } drop_file;
  };
} ZLay_Event;

ZLAY_API void ZLay_PollEvents(void);
ZLAY_API bool ZLay_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event);
ZLAY_API void ZLay_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event);

#ifdef __cplusplus
}
#endif

#endif
