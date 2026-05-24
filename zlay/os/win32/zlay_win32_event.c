#include <os/win32/zlay_win32_backend.h>

#include <windowsx.h>

static double ZLay_Win32_EventTimestamp(void) {
  return ZLay_TimeNow();
}

uint32_t ZLay_Win32_Modifiers(void) {
  uint32_t modifiers = ZLAY_KEY_MOD_NONE;

  if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) modifiers |= ZLAY_KEY_MOD_SHIFT;
  if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) modifiers |= ZLAY_KEY_MOD_CONTROL;
  if ((GetKeyState(VK_MENU) & 0x8000) != 0) modifiers |= ZLAY_KEY_MOD_ALT;
  if ((GetKeyState(VK_LWIN) & 0x8000) != 0 || (GetKeyState(VK_RWIN) & 0x8000) != 0) modifiers |= ZLAY_KEY_MOD_SUPER;

  return modifiers;
}

int ZLay_Win32_KeyFromVK(WPARAM vk) {
  if (vk >= 'A' && vk <= 'Z') return ZLAY_KEY_A + (int)(vk - 'A');
  if (vk >= '0' && vk <= '9') return ZLAY_KEY_0 + (int)(vk - '0');
  if (vk >= VK_F1 && vk <= VK_F12) return ZLAY_KEY_F1 + (int)(vk - VK_F1);

  switch (vk) {
    case VK_ESCAPE: return ZLAY_KEY_ESCAPE;
    case VK_RETURN: return ZLAY_KEY_ENTER;
    case VK_TAB: return ZLAY_KEY_TAB;
    case VK_BACK: return ZLAY_KEY_BACKSPACE;
    case VK_SPACE: return ZLAY_KEY_SPACE;
    case VK_LEFT: return ZLAY_KEY_LEFT;
    case VK_RIGHT: return ZLAY_KEY_RIGHT;
    case VK_UP: return ZLAY_KEY_UP;
    case VK_DOWN: return ZLAY_KEY_DOWN;
    case VK_SHIFT:
    case VK_LSHIFT:
    case VK_RSHIFT: return ZLAY_KEY_SHIFT;
    case VK_CONTROL:
    case VK_LCONTROL:
    case VK_RCONTROL: return ZLAY_KEY_CONTROL;
    case VK_MENU:
    case VK_LMENU:
    case VK_RMENU: return ZLAY_KEY_ALT;
    case VK_LWIN:
    case VK_RWIN: return ZLAY_KEY_SUPER;
    default: return ZLAY_KEY_UNKNOWN;
  }
}

static int ZLay_Win32_MouseButton(UINT message) {
  switch (message) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP: return ZLAY_MOUSE_BUTTON_LEFT;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: return ZLAY_MOUSE_BUTTON_RIGHT;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP: return ZLAY_MOUSE_BUTTON_MIDDLE;
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP: return ZLAY_MOUSE_BUTTON_4;
    default: return -1;
  }
}

bool ZLay_Win32_ProcessMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, LRESULT* out_result) {
  ZLay_Window* window = ZLay_Win32_FindWindow(hwnd);
  ZLay_Event event;

  if (out_result != NULL) {
    *out_result = 0;
  }
  if (window == NULL) {
    return false;
  }

  event = (ZLay_Event){0};
  event.timestamp = ZLay_Win32_EventTimestamp();

  switch (message) {
    case WM_CLOSE:
      event.type = ZLAY_EVENT_WINDOW_CLOSE;
      ZLay_Win32_WindowPushEvent(window, &event);
      return true;

    case WM_SIZE:
      window->width = (uint32_t)LOWORD(lparam);
      window->height = (uint32_t)HIWORD(lparam);
      event.type = ZLAY_EVENT_WINDOW_RESIZE;
      event.resize.width = window->width;
      event.resize.height = window->height;
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_MOVE:
      event.type = ZLAY_EVENT_WINDOW_MOVE;
      event.move.x = (int16_t)LOWORD(lparam);
      event.move.y = (int16_t)HIWORD(lparam);
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_SETFOCUS:
      event.type = ZLAY_EVENT_WINDOW_FOCUS;
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_KILLFOCUS:
      event.type = ZLAY_EVENT_WINDOW_BLUR;
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_DPICHANGED:
      window->scale = ZLay_DpiScaleFromDpi((float)HIWORD(wparam));
      event.type = ZLAY_EVENT_WINDOW_DPI_CHANGED;
      event.dpi.scale = window->scale;
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_MOUSEMOVE:
      if (!window->tracking_mouse) {
        TRACKMOUSEEVENT track;
        track.cbSize = sizeof(track);
        track.dwFlags = TME_LEAVE;
        track.hwndTrack = hwnd;
        track.dwHoverTime = 0;
        TrackMouseEvent(&track);
        window->tracking_mouse = true;
      }
      event.type = ZLAY_EVENT_MOUSE_MOVE;
      event.mouse_move.x = (float)GET_X_LPARAM(lparam);
      event.mouse_move.y = (float)GET_Y_LPARAM(lparam);
      event.mouse_move.dx = window->has_last_mouse ? event.mouse_move.x - (float)window->last_mouse_x : 0.0f;
      event.mouse_move.dy = window->has_last_mouse ? event.mouse_move.y - (float)window->last_mouse_y : 0.0f;
      window->last_mouse_x = GET_X_LPARAM(lparam);
      window->last_mouse_y = GET_Y_LPARAM(lparam);
      window->has_last_mouse = true;
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_MOUSELEAVE:
      window->tracking_mouse = false;
      window->has_last_mouse = false;
      event.type = ZLAY_EVENT_MOUSE_LEAVE;
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
      event.type = (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN)
        ? ZLAY_EVENT_MOUSE_BUTTON_DOWN
        : ZLAY_EVENT_MOUSE_BUTTON_UP;
      event.mouse_button.button = ZLay_Win32_MouseButton(message);
      if ((message == WM_XBUTTONDOWN || message == WM_XBUTTONUP) && GET_XBUTTON_WPARAM(wparam) == XBUTTON2) {
        event.mouse_button.button = ZLAY_MOUSE_BUTTON_5;
      }
      event.mouse_button.x = (float)GET_X_LPARAM(lparam);
      event.mouse_button.y = (float)GET_Y_LPARAM(lparam);
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_MOUSEWHEEL:
      event.type = ZLAY_EVENT_MOUSE_WHEEL;
      event.wheel.y = (float)GET_WHEEL_DELTA_WPARAM(wparam) / (float)WHEEL_DELTA;
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_MOUSEHWHEEL:
      event.type = ZLAY_EVENT_MOUSE_WHEEL;
      event.wheel.x = (float)GET_WHEEL_DELTA_WPARAM(wparam) / (float)WHEEL_DELTA;
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
      event.type = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) ? ZLAY_EVENT_KEY_DOWN : ZLAY_EVENT_KEY_UP;
      event.key.key = ZLay_Win32_KeyFromVK(wparam);
      event.key.scancode = (int)((lparam >> 16) & 0xFF);
      event.key.repeat = (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) && ((lparam & (1L << 30)) != 0);
      event.key.modifiers = ZLay_Win32_Modifiers();
      ZLay_Win32_WindowPushEvent(window, &event);
      return false;

    case WM_CHAR:
      if (wparam >= 32u) {
        event.type = ZLAY_EVENT_TEXT_INPUT;
        event.text.codepoint = (uint32_t)wparam;
        ZLay_Win32_WindowPushEvent(window, &event);
      }
      return false;

    default:
      return false;
  }
}

void ZLay_Win32_PollEvents(void) {
  MSG message;

  while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
}
