#include <os/window/zlay_window.h>

#include <os/event/zlay_event.h>
#include <os/event/zlay_keycode.h>
#include <os/event/zlay_mouse.h>
#include <os/system/zlay_clipboard.h>
#include <os/system/zlay_timer.h>
#include <os/zlay_os_backend.h>
#include <os/window/zlay_cursor.h>
#include <os/window/zlay_monitor.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#ifndef ZLAY_X11_EVENT_QUEUE_CAPACITY
#  define ZLAY_X11_EVENT_QUEUE_CAPACITY 256u
#endif

struct ZLay_Window {
  Display* display;
  int screen;
  Window window;
  Atom wm_delete_window;
  Cursor current_cursor;
  uint32_t width;
  uint32_t height;
  float scale;
  bool should_close;
  void* user_data;
  ZLay_Event events[ZLAY_X11_EVENT_QUEUE_CAPACITY];
  uint32_t event_head;
  uint32_t event_count;
  int last_mouse_x;
  int last_mouse_y;
  bool has_last_mouse;
  bool owns_display;
  bool owns_window;
  struct ZLay_Window* next;
};

static Display* zlay_x11_display;
static uint32_t zlay_x11_ref_count;
static ZLay_Window* zlay_x11_windows;
static Atom zlay_x11_clipboard_atom;
static Atom zlay_x11_utf8_atom;
static Atom zlay_x11_targets_atom;
static char* zlay_x11_clipboard_text;
static char* zlay_x11_clipboard_receive_text;

static void ZLay_X11DispatchEvent(XEvent* native_event);

static double ZLay_X11EventTimestamp(void) {
  return ZLay_TimeNow();
}

static void ZLay_X11InitAtoms(Display* display) {
  if (display == NULL || zlay_x11_clipboard_atom != None) {
    return;
  }

  zlay_x11_clipboard_atom = XInternAtom(display, "CLIPBOARD", False);
  zlay_x11_utf8_atom = XInternAtom(display, "UTF8_STRING", False);
  zlay_x11_targets_atom = XInternAtom(display, "TARGETS", False);
}

static float ZLay_X11ScreenScale(Display* display, int screen) {
  const int width_px = DisplayWidth(display, screen);
  const int width_mm = DisplayWidthMM(display, screen);

  if (width_px <= 0 || width_mm <= 0) {
    return 1.0f;
  }

  {
    const float dpi = (float)width_px * 25.4f / (float)width_mm;
    const float scale = dpi / 96.0f;
    return scale > 0.25f ? scale : 1.0f;
  }
}

static ZLay_Window* ZLay_X11FirstWindow(void) {
  return zlay_x11_windows;
}

static char* ZLay_X11StringDuplicate(const char* text) {
  size_t length;
  char* copy;

  if (text == NULL) {
    text = "";
  }

  length = strlen(text);
  copy = (char*)malloc(length + 1u);
  if (copy == NULL) {
    return NULL;
  }

  memcpy(copy, text, length + 1u);
  return copy;
}

static void ZLay_X11LinkWindow(ZLay_Window* window) {
  window->next = zlay_x11_windows;
  zlay_x11_windows = window;
}

static void ZLay_X11UnlinkWindow(ZLay_Window* window) {
  ZLay_Window** link = &zlay_x11_windows;

  while (*link != NULL) {
    if (*link == window) {
      *link = window->next;
      window->next = NULL;
      return;
    }
    link = &(*link)->next;
  }
}

static ZLay_Window* ZLay_X11FindWindow(Window native_window) {
  ZLay_Window* current = zlay_x11_windows;

  while (current != NULL) {
    if (current->window == native_window) {
      return current;
    }
    current = current->next;
  }

  return NULL;
}

static uint32_t ZLay_X11EventQueueIndex(const ZLay_Window* window, uint32_t offset) {
  return (window->event_head + offset) % ZLAY_X11_EVENT_QUEUE_CAPACITY;
}

static uint32_t ZLay_X11Modifiers(unsigned int state) {
  uint32_t modifiers = ZLAY_KEY_MOD_NONE;

  if ((state & ShiftMask) != 0u) {
    modifiers |= ZLAY_KEY_MOD_SHIFT;
  }
  if ((state & ControlMask) != 0u) {
    modifiers |= ZLAY_KEY_MOD_CONTROL;
  }
  if ((state & Mod1Mask) != 0u) {
    modifiers |= ZLAY_KEY_MOD_ALT;
  }
  if ((state & Mod4Mask) != 0u) {
    modifiers |= ZLAY_KEY_MOD_SUPER;
  }

  return modifiers;
}

static int ZLay_X11MapButton(unsigned int button) {
  switch (button) {
    case Button1:
      return ZLAY_MOUSE_BUTTON_LEFT;
    case Button3:
      return ZLAY_MOUSE_BUTTON_RIGHT;
    case Button2:
      return ZLAY_MOUSE_BUTTON_MIDDLE;
    case 8:
      return ZLAY_MOUSE_BUTTON_4;
    case 9:
      return ZLAY_MOUSE_BUTTON_5;
    default:
      return -1;
  }
}

static int ZLay_X11MapKey(KeySym keysym) {
  if (keysym >= XK_a && keysym <= XK_z) {
    return ZLAY_KEY_A + (int)(keysym - XK_a);
  }
  if (keysym >= XK_A && keysym <= XK_Z) {
    return ZLAY_KEY_A + (int)(keysym - XK_A);
  }
  if (keysym >= XK_0 && keysym <= XK_9) {
    return ZLAY_KEY_0 + (int)(keysym - XK_0);
  }
  if (keysym >= XK_F1 && keysym <= XK_F12) {
    return ZLAY_KEY_F1 + (int)(keysym - XK_F1);
  }

  switch (keysym) {
    case XK_Escape:
      return ZLAY_KEY_ESCAPE;
    case XK_Return:
    case XK_KP_Enter:
      return ZLAY_KEY_ENTER;
    case XK_Tab:
      return ZLAY_KEY_TAB;
    case XK_BackSpace:
      return ZLAY_KEY_BACKSPACE;
    case XK_space:
      return ZLAY_KEY_SPACE;
    case XK_Left:
      return ZLAY_KEY_LEFT;
    case XK_Right:
      return ZLAY_KEY_RIGHT;
    case XK_Up:
      return ZLAY_KEY_UP;
    case XK_Down:
      return ZLAY_KEY_DOWN;
    case XK_Shift_L:
    case XK_Shift_R:
      return ZLAY_KEY_SHIFT;
    case XK_Control_L:
    case XK_Control_R:
      return ZLAY_KEY_CONTROL;
    case XK_Alt_L:
    case XK_Alt_R:
    case XK_Meta_L:
    case XK_Meta_R:
      return ZLAY_KEY_ALT;
    case XK_Super_L:
    case XK_Super_R:
      return ZLAY_KEY_SUPER;
    default:
      return ZLAY_KEY_UNKNOWN;
  }
}

static void ZLay_X11SelectEvents(ZLay_Window* window) {
  XSelectInput(
    window->display,
    window->window,
    ExposureMask |
      StructureNotifyMask |
      FocusChangeMask |
      EnterWindowMask |
      LeaveWindowMask |
      PointerMotionMask |
      ButtonPressMask |
      ButtonReleaseMask |
      KeyPressMask |
      KeyReleaseMask
  );
}

ZLay_Window* ZLay_X11_WindowAttachNative(const ZLay_WindowAttachDesc* desc) {
  Display* display;
  Window native_window;
  ZLay_Window* window;

  if (desc == NULL || desc->native.platform != ZLAY_OS_PLATFORM_X11) {
    return NULL;
  }

  display = (Display*)desc->native.display;
  native_window = (Window)desc->native.raw0;
  if (native_window == 0 && desc->native.window != NULL) {
    native_window = (Window)(uintptr_t)desc->native.window;
  }

  if (display == NULL || native_window == 0) {
    return NULL;
  }

  window = (ZLay_Window*)calloc(1u, sizeof(ZLay_Window));
  if (window == NULL) {
    return NULL;
  }

  if (zlay_x11_display == NULL) {
    zlay_x11_display = display;
  }

  window->display = display;
  window->screen = DefaultScreen(window->display);
  window->window = native_window;
  window->width = desc->width;
  window->height = desc->height;
  window->scale = desc->scale > 0.0f ? desc->scale : ZLay_X11ScreenScale(window->display, window->screen);
  window->user_data = desc->user_data;
  ZLay_X11InitAtoms(window->display);

  window->wm_delete_window = XInternAtom(window->display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(window->display, window->window, &window->wm_delete_window, 1);

  if (desc->select_events) {
    ZLay_X11SelectEvents(window);
  }

  ZLay_X11LinkWindow(window);
  zlay_x11_ref_count += 1u;

  if (zlay_x11_clipboard_text != NULL) {
    XSetSelectionOwner(window->display, zlay_x11_clipboard_atom, window->window, CurrentTime);
  }

  XFlush(window->display);
  return window;
}

void ZLay_X11_WindowDetach(ZLay_Window* window) {
  ZLay_Window* next_owner;

  if (window == NULL) {
    return;
  }

  ZLay_X11UnlinkWindow(window);
  next_owner = ZLay_X11FirstWindow();
  if (zlay_x11_clipboard_text != NULL && next_owner != NULL) {
    XSetSelectionOwner(next_owner->display, zlay_x11_clipboard_atom, next_owner->window, CurrentTime);
  }

  if (window->display != NULL && window->window != 0) {
    if (window->current_cursor != 0) {
      XFreeCursor(window->display, window->current_cursor);
    }
    if (window->owns_window) {
      XDestroyWindow(window->display, window->window);
    }
  }

  if (zlay_x11_ref_count > 0u) {
    zlay_x11_ref_count -= 1u;
  }
  if (window->owns_display && zlay_x11_ref_count == 0u && zlay_x11_display != NULL) {
    XCloseDisplay(zlay_x11_display);
    zlay_x11_display = NULL;
  } else if (zlay_x11_ref_count == 0u) {
    zlay_x11_display = NULL;
  }

  free(window);
}

void ZLay_X11_WindowShow(ZLay_Window* window) {
  if (window == NULL) {
    return;
  }

  XMapWindow(window->display, window->window);
  XFlush(window->display);
}

void ZLay_X11_WindowHide(ZLay_Window* window) {
  if (window == NULL) {
    return;
  }

  XUnmapWindow(window->display, window->window);
  XFlush(window->display);
}

void ZLay_X11_WindowSetTitle(ZLay_Window* window, const char* title) {
  if (window == NULL) {
    return;
  }

  XStoreName(window->display, window->window, title != NULL ? title : "");
  XFlush(window->display);
}

bool ZLay_X11_WindowShouldClose(ZLay_Window* window) {
  return window == NULL || window->should_close;
}

void ZLay_X11_WindowRequestClose(ZLay_Window* window) {
  ZLay_Event event;

  if (window == NULL) {
    return;
  }

  memset(&event, 0, sizeof(event));
  event.type = ZLAY_EVENT_WINDOW_CLOSE;
  event.timestamp = ZLay_X11EventTimestamp();
  ZLay_X11_WindowPushEvent(window, &event);
}

void ZLay_X11_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
  if (width != NULL) {
    *width = window != NULL ? window->width : 0u;
  }
  if (height != NULL) {
    *height = window != NULL ? window->height : 0u;
  }
}

void ZLay_X11_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
  const float scale = window != NULL ? window->scale : 1.0f;

  if (width != NULL) {
    *width = window != NULL ? (uint32_t)((float)window->width * scale + 0.5f) : 0u;
  }
  if (height != NULL) {
    *height = window != NULL ? (uint32_t)((float)window->height * scale + 0.5f) : 0u;
  }
}

float ZLay_X11_WindowGetScale(ZLay_Window* window) {
  return window != NULL ? window->scale : 1.0f;
}

static Cursor ZLay_X11CreateHiddenCursor(Display* display, Window window) {
  static const char empty_bits[] = {0};
  Pixmap bitmap;
  XColor color;
  Cursor cursor;

  memset(&color, 0, sizeof(color));
  bitmap = XCreateBitmapFromData(display, window, empty_bits, 1u, 1u);
  if (bitmap == 0) {
    return 0;
  }

  cursor = XCreatePixmapCursor(display, bitmap, bitmap, &color, &color, 0u, 0u);
  XFreePixmap(display, bitmap);
  return cursor;
}

static Cursor ZLay_X11CreateCursor(ZLay_Window* window, ZLay_Cursor cursor) {
  unsigned int shape = XC_left_ptr;

  switch (cursor) {
    case ZLAY_CURSOR_HAND:
      shape = XC_hand2;
      break;
    case ZLAY_CURSOR_TEXT:
      shape = XC_xterm;
      break;
    case ZLAY_CURSOR_RESIZE_HORIZONTAL:
      shape = XC_sb_h_double_arrow;
      break;
    case ZLAY_CURSOR_RESIZE_VERTICAL:
      shape = XC_sb_v_double_arrow;
      break;
    case ZLAY_CURSOR_CROSSHAIR:
      shape = XC_crosshair;
      break;
    case ZLAY_CURSOR_HIDDEN:
      return ZLay_X11CreateHiddenCursor(window->display, window->window);
    case ZLAY_CURSOR_ARROW:
    default:
      shape = XC_left_ptr;
      break;
  }

  return XCreateFontCursor(window->display, shape);
}

void ZLay_X11_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor) {
  Cursor native_cursor;

  if (window == NULL) {
    return;
  }

  native_cursor = ZLay_X11CreateCursor(window, cursor);
  if (native_cursor == 0) {
    return;
  }

  XDefineCursor(window->display, window->window, native_cursor);
  XFlush(window->display);

  if (window->current_cursor != 0) {
    XFreeCursor(window->display, window->current_cursor);
  }
  window->current_cursor = native_cursor;
}

ZLay_NativeWindowHandle ZLay_X11_WindowGetNativeHandle(ZLay_Window* window) {
  if (window == NULL) {
    return (ZLay_NativeWindowHandle){0};
  }

  return (ZLay_NativeWindowHandle){
    ZLAY_OS_PLATFORM_X11,
    window->display,
    (void*)(uintptr_t)window->window,
    NULL,
    NULL,
    (uintptr_t)window->window,
    (uintptr_t)ConnectionNumber(window->display),
    0u
  };
}

void ZLay_X11_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event) {
  uint32_t index;

  if (window == NULL || event == NULL) {
    return;
  }

  if (window->event_count == ZLAY_X11_EVENT_QUEUE_CAPACITY) {
    window->event_head = ZLay_X11EventQueueIndex(window, 1u);
    window->event_count -= 1u;
  }

  index = ZLay_X11EventQueueIndex(window, window->event_count);
  window->events[index] = *event;
  window->event_count += 1u;

  if (event->type == ZLAY_EVENT_WINDOW_CLOSE) {
    window->should_close = true;
  }
}

bool ZLay_X11_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event) {
  if (window == NULL || out_event == NULL || window->event_count == 0u) {
    return false;
  }

  *out_event = window->events[window->event_head];
  window->event_head = ZLay_X11EventQueueIndex(window, 1u);
  window->event_count -= 1u;
  return true;
}

static void ZLay_X11PushResize(ZLay_Window* window, const XConfigureEvent* configure) {
  ZLay_Event event;

  window->width = configure->width > 0 ? (uint32_t)configure->width : 0u;
  window->height = configure->height > 0 ? (uint32_t)configure->height : 0u;

  memset(&event, 0, sizeof(event));
  event.type = ZLAY_EVENT_WINDOW_RESIZE;
  event.timestamp = ZLay_X11EventTimestamp();
  event.resize.width = window->width;
  event.resize.height = window->height;
  ZLay_X11_WindowPushEvent(window, &event);
}

static void ZLay_X11PushMove(ZLay_Window* window, const XConfigureEvent* configure) {
  ZLay_Event event;

  memset(&event, 0, sizeof(event));
  event.type = ZLAY_EVENT_WINDOW_MOVE;
  event.timestamp = ZLay_X11EventTimestamp();
  event.move.x = configure->x;
  event.move.y = configure->y;
  ZLay_X11_WindowPushEvent(window, &event);
}

static void ZLay_X11PushKey(ZLay_Window* window, XKeyEvent* key, bool down) {
  ZLay_Event event;
  KeySym keysym = NoSymbol;
  char text[8];
  int text_length;

  memset(text, 0, sizeof(text));
  text_length = XLookupString(key, text, (int)sizeof(text), &keysym, NULL);

  memset(&event, 0, sizeof(event));
  event.type = down ? ZLAY_EVENT_KEY_DOWN : ZLAY_EVENT_KEY_UP;
  event.timestamp = ZLay_X11EventTimestamp();
  event.key.key = ZLay_X11MapKey(keysym);
  event.key.scancode = (int)key->keycode;
  event.key.repeat = false;
  event.key.modifiers = ZLay_X11Modifiers(key->state);
  ZLay_X11_WindowPushEvent(window, &event);

  if (down && text_length == 1 && (unsigned char)text[0] >= 32u) {
    memset(&event, 0, sizeof(event));
    event.type = ZLAY_EVENT_TEXT_INPUT;
    event.timestamp = ZLay_X11EventTimestamp();
    event.text.codepoint = (uint32_t)(unsigned char)text[0];
    ZLay_X11_WindowPushEvent(window, &event);
  }
}

static void ZLay_X11PushMouseMove(ZLay_Window* window, const XMotionEvent* motion) {
  ZLay_Event event;
  const float dx = window->has_last_mouse ? (float)(motion->x - window->last_mouse_x) : 0.0f;
  const float dy = window->has_last_mouse ? (float)(motion->y - window->last_mouse_y) : 0.0f;

  window->last_mouse_x = motion->x;
  window->last_mouse_y = motion->y;
  window->has_last_mouse = true;

  memset(&event, 0, sizeof(event));
  event.type = ZLAY_EVENT_MOUSE_MOVE;
  event.timestamp = ZLay_X11EventTimestamp();
  event.mouse_move.x = (float)motion->x;
  event.mouse_move.y = (float)motion->y;
  event.mouse_move.dx = dx;
  event.mouse_move.dy = dy;
  ZLay_X11_WindowPushEvent(window, &event);
}

static void ZLay_X11PushMouseButton(ZLay_Window* window, const XButtonEvent* button, bool down) {
  ZLay_Event event;
  const int mapped_button = ZLay_X11MapButton(button->button);

  if (down && (button->button == Button4 || button->button == Button5 || button->button == 6 || button->button == 7)) {
    memset(&event, 0, sizeof(event));
    event.type = ZLAY_EVENT_MOUSE_WHEEL;
    event.timestamp = ZLay_X11EventTimestamp();
    event.wheel.x = button->button == 6 ? -1.0f : (button->button == 7 ? 1.0f : 0.0f);
    event.wheel.y = button->button == Button4 ? 1.0f : (button->button == Button5 ? -1.0f : 0.0f);
    ZLay_X11_WindowPushEvent(window, &event);
    return;
  }

  if (mapped_button < 0) {
    return;
  }

  memset(&event, 0, sizeof(event));
  event.type = down ? ZLAY_EVENT_MOUSE_BUTTON_DOWN : ZLAY_EVENT_MOUSE_BUTTON_UP;
  event.timestamp = ZLay_X11EventTimestamp();
  event.mouse_button.button = mapped_button;
  event.mouse_button.x = (float)button->x;
  event.mouse_button.y = (float)button->y;
  ZLay_X11_WindowPushEvent(window, &event);
}

static void ZLay_X11PushSimpleWindowEvent(ZLay_Window* window, ZLay_EventType type) {
  ZLay_Event event;

  memset(&event, 0, sizeof(event));
  event.type = type;
  event.timestamp = ZLay_X11EventTimestamp();
  ZLay_X11_WindowPushEvent(window, &event);
}

static void ZLay_X11HandleSelectionRequest(XSelectionRequestEvent* request) {
  XSelectionEvent response;
  Atom property = request->property;

  memset(&response, 0, sizeof(response));
  response.type = SelectionNotify;
  response.display = request->display;
  response.requestor = request->requestor;
  response.selection = request->selection;
  response.target = request->target;
  response.time = request->time;
  response.property = None;

  if (property == None) {
    property = request->target;
  }

  if (zlay_x11_clipboard_text == NULL) {
    XSendEvent(request->display, request->requestor, False, 0, (XEvent*)&response);
    XFlush(request->display);
    return;
  }

  if (request->target == zlay_x11_targets_atom) {
    Atom targets[3];
    targets[0] = zlay_x11_targets_atom;
    targets[1] = zlay_x11_utf8_atom;
    targets[2] = XA_STRING;
    XChangeProperty(
      request->display,
      request->requestor,
      property,
      XA_ATOM,
      32,
      PropModeReplace,
      (const unsigned char*)targets,
      3
    );
    response.property = property;
  } else if (request->target == zlay_x11_utf8_atom || request->target == XA_STRING) {
    XChangeProperty(
      request->display,
      request->requestor,
      property,
      request->target,
      8,
      PropModeReplace,
      (const unsigned char*)zlay_x11_clipboard_text,
      (int)strlen(zlay_x11_clipboard_text)
    );
    response.property = property;
  }

  XSendEvent(request->display, request->requestor, False, 0, (XEvent*)&response);
  XFlush(request->display);
}

static void ZLay_X11DispatchEvent(XEvent* native_event) {
  ZLay_Window* window = ZLay_X11FindWindow(native_event->xany.window);

  if (native_event->type == SelectionRequest) {
    ZLay_X11HandleSelectionRequest(&native_event->xselectionrequest);
    return;
  }

  if (window == NULL) {
    return;
  }

  switch (native_event->type) {
    case ConfigureNotify:
      if (native_event->xconfigure.width != (int)window->width ||
          native_event->xconfigure.height != (int)window->height) {
        ZLay_X11PushResize(window, &native_event->xconfigure);
      }
      ZLay_X11PushMove(window, &native_event->xconfigure);
      break;

    case MotionNotify:
      ZLay_X11PushMouseMove(window, &native_event->xmotion);
      break;

    case ButtonPress:
      ZLay_X11PushMouseButton(window, &native_event->xbutton, true);
      break;

    case ButtonRelease:
      ZLay_X11PushMouseButton(window, &native_event->xbutton, false);
      break;

    case KeyPress:
      ZLay_X11PushKey(window, &native_event->xkey, true);
      break;

    case KeyRelease:
      ZLay_X11PushKey(window, &native_event->xkey, false);
      break;

    case EnterNotify:
      window->last_mouse_x = native_event->xcrossing.x;
      window->last_mouse_y = native_event->xcrossing.y;
      window->has_last_mouse = true;
      ZLay_X11PushSimpleWindowEvent(window, ZLAY_EVENT_MOUSE_ENTER);
      break;

    case LeaveNotify:
      window->has_last_mouse = false;
      ZLay_X11PushSimpleWindowEvent(window, ZLAY_EVENT_MOUSE_LEAVE);
      break;

    case FocusIn:
      ZLay_X11PushSimpleWindowEvent(window, ZLAY_EVENT_WINDOW_FOCUS);
      break;

    case FocusOut:
      ZLay_X11PushSimpleWindowEvent(window, ZLAY_EVENT_WINDOW_BLUR);
      break;

    case ClientMessage:
      if ((Atom)native_event->xclient.data.l[0] == window->wm_delete_window) {
        ZLay_X11PushSimpleWindowEvent(window, ZLAY_EVENT_WINDOW_CLOSE);
      }
      break;

    case SelectionClear:
      free(zlay_x11_clipboard_text);
      zlay_x11_clipboard_text = NULL;
      break;

    default:
      break;
  }
}

void ZLay_X11_PollEvents(void) {
  if (zlay_x11_display == NULL) {
    return;
  }

  while (XPending(zlay_x11_display) > 0) {
    XEvent native_event;
    XNextEvent(zlay_x11_display, &native_event);
    ZLay_X11DispatchEvent(&native_event);
  }
}

uint32_t ZLay_X11_GetMonitorCount(void) {
  Display* display = zlay_x11_display;
  bool close_display = false;

  if (display == NULL) {
    display = XOpenDisplay(NULL);
    close_display = display != NULL;
  }

  if (display == NULL) {
    return 0u;
  }

  if (close_display) {
    XCloseDisplay(display);
  }
  return 1u;
}

bool ZLay_X11_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info) {
  Display* display = zlay_x11_display;
  bool close_display = false;
  int screen;

  if (index != 0u || out_info == NULL) {
    return false;
  }

  if (display == NULL) {
    display = XOpenDisplay(NULL);
    close_display = display != NULL;
  }

  if (display == NULL) {
    return false;
  }

  screen = DefaultScreen(display);
  *out_info = (ZLay_MonitorInfo){
    0,
    0,
    (uint32_t)DisplayWidth(display, screen),
    (uint32_t)DisplayHeight(display, screen),
    ZLay_X11ScreenScale(display, screen),
    ZLay_X11ScreenScale(display, screen),
    0u,
    "X11 Default Screen"
  };

  if (close_display) {
    XCloseDisplay(display);
  }
  return true;
}

bool ZLay_X11_ClipboardSetText(const char* text) {
  ZLay_Window* owner = ZLay_X11FirstWindow();
  char* copy = ZLay_X11StringDuplicate(text);

  if (copy == NULL) {
    return false;
  }

  free(zlay_x11_clipboard_text);
  zlay_x11_clipboard_text = copy;

  if (owner != NULL) {
    ZLay_X11InitAtoms(owner->display);
    XSetSelectionOwner(owner->display, zlay_x11_clipboard_atom, owner->window, CurrentTime);
    XFlush(owner->display);
  }

  return true;
}

static const char* ZLay_X11ReceiveClipboard(ZLay_Window* window) {
  Atom property;
  double start_time;

  if (window == NULL) {
    return zlay_x11_clipboard_text != NULL ? zlay_x11_clipboard_text : "";
  }

  ZLay_X11InitAtoms(window->display);
  property = XInternAtom(window->display, "ZLAY_CLIPBOARD_DATA", False);
  XConvertSelection(
    window->display,
    zlay_x11_clipboard_atom,
    zlay_x11_utf8_atom,
    property,
    window->window,
    CurrentTime
  );
  XFlush(window->display);

  start_time = ZLay_TimeNow();
  while (ZLay_TimeNow() - start_time < 0.25) {
    XEvent event;

    if (XPending(window->display) == 0) {
      ZLay_SleepMilliseconds(1u);
      continue;
    }

    XNextEvent(window->display, &event);
    if (event.type == SelectionNotify && event.xselection.requestor == window->window) {
      Atom actual_type;
      int actual_format;
      unsigned long item_count;
      unsigned long bytes_after;
      unsigned char* data = NULL;

      if (event.xselection.property == None) {
        break;
      }

      if (XGetWindowProperty(
            window->display,
            window->window,
            property,
            0,
            1024 * 1024,
            True,
            AnyPropertyType,
            &actual_type,
            &actual_format,
            &item_count,
            &bytes_after,
            &data) == Success &&
          data != NULL &&
          actual_format == 8) {
        char* copy = (char*)malloc(item_count + 1u);
        if (copy != NULL) {
          memcpy(copy, data, item_count);
          copy[item_count] = '\0';
          free(zlay_x11_clipboard_receive_text);
          zlay_x11_clipboard_receive_text = copy;
        }
        XFree(data);
      } else if (data != NULL) {
        XFree(data);
      }
      break;
    }

    ZLay_X11DispatchEvent(&event);
  }

  return zlay_x11_clipboard_receive_text != NULL ? zlay_x11_clipboard_receive_text : "";
}

const char* ZLay_X11_ClipboardGetText(void) {
  ZLay_Window* window = ZLay_X11FirstWindow();

  if (window == NULL) {
    return zlay_x11_clipboard_text != NULL ? zlay_x11_clipboard_text : "";
  }

  ZLay_X11InitAtoms(window->display);
  if (XGetSelectionOwner(window->display, zlay_x11_clipboard_atom) == window->window) {
    return zlay_x11_clipboard_text != NULL ? zlay_x11_clipboard_text : "";
  }

  return ZLay_X11ReceiveClipboard(window);
}
