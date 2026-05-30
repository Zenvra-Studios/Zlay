#include <backends/platform/gtk/zlay_gtk_backend.h>

static uint32_t ZLay_GTK_EventQueueIndex(const ZLay_Window* window, uint32_t offset) {
  return (window->event_head + offset) % ZLAY_GTK_EVENT_QUEUE_CAPACITY;
}

void ZLay_GTK_PollEvents(void) {
  ZLay_GTK_State* state = ZLay_GTK_StateGet();

  if (state->poll_events != NULL) {
    state->poll_events(state->user);
  }
}

void ZLay_GTK_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event) {
  uint32_t index;

  if (window == NULL || event == NULL) {
    return;
  }

  if (event->type == ZLAY_EVENT_WINDOW_RESIZE) {
    window->width = event->resize.width;
    window->height = event->resize.height;
  } else if (event->type == ZLAY_EVENT_WINDOW_DPI_CHANGED && event->dpi.scale > 0.0f) {
    window->scale = event->dpi.scale;
  } else if (event->type == ZLAY_EVENT_MOUSE_MOVE) {
    window->last_mouse_x = event->mouse_move.x;
    window->last_mouse_y = event->mouse_move.y;
    window->has_last_mouse = true;
  } else if (event->type == ZLAY_EVENT_MOUSE_LEAVE) {
    window->has_last_mouse = false;
  }

  if (window->event_count == ZLAY_GTK_EVENT_QUEUE_CAPACITY) {
    window->event_head = ZLay_GTK_EventQueueIndex(window, 1u);
    window->event_count -= 1u;
  }

  index = ZLay_GTK_EventQueueIndex(window, window->event_count);
  window->events[index] = *event;
  window->event_count += 1u;

  if (event->type == ZLAY_EVENT_WINDOW_CLOSE) {
    window->should_close = true;
  }
}

bool ZLay_GTK_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event) {
  if (window == NULL || out_event == NULL || window->event_count == 0u) {
    return false;
  }

  *out_event = window->events[window->event_head];
  window->event_head = ZLay_GTK_EventQueueIndex(window, 1u);
  window->event_count -= 1u;
  return true;
}
