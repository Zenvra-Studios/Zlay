#include <simply_gui/gui_win32.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <windowsx.h>

#include <zlay_shell.h>
#include <zlay_ui.h>

#define ZLAY_WIN32_BUTTON_DEMO_CLASS "ZLayWin32ButtonDemo"

static COLORREF ZLay_Win32ColorRef(ZLay_Color color) {
  return RGB(color.r, color.g, color.b);
}

static bool ZLay_Win32RectContains(ZLay_Rect rect, int x, int y) {
  return (float)x >= rect.x &&
         (float)y >= rect.y &&
         (float)x < rect.x + rect.width &&
         (float)y < rect.y + rect.height;
}

static RECT ZLay_Win32RectFromBounds(ZLay_Rect bounds) {
  RECT rect;
  rect.left = (LONG)bounds.x;
  rect.top = (LONG)bounds.y;
  rect.right = (LONG)(bounds.x + bounds.width);
  rect.bottom = (LONG)(bounds.y + bounds.height);

  if (rect.right < rect.left) {
    rect.right = rect.left;
  }
  if (rect.bottom < rect.top) {
    rect.bottom = rect.top;
  }

  return rect;
}

static ZLay_Dimensions ZLay_Win32MeasureText(ZLay_String text, void* user) {
  ZLay_Win32ButtonDemo* demo = (ZLay_Win32ButtonDemo*)user;
  const int length = text.length > (uint32_t)INT32_MAX ? INT32_MAX : (int)text.length;
  SIZE size;
  HDC dc;
  HGDIOBJ old_font;

  if (demo == NULL || demo->window == NULL || text.chars == NULL || length <= 0) {
    return (ZLay_Dimensions){8.0f * (float)text.length, 16.0f};
  }

  dc = GetDC(demo->window);
  if (dc == NULL) {
    return (ZLay_Dimensions){8.0f * (float)text.length, 16.0f};
  }

  old_font = demo->font != NULL ? SelectObject(dc, demo->font) : NULL;
  size.cx = 0;
  size.cy = 0;
  if (!GetTextExtentPoint32A(dc, text.chars, length, &size)) {
    size.cx = 8 * length;
    size.cy = 16;
  }

  if (old_font != NULL) {
    SelectObject(dc, old_font);
  }
  ReleaseDC(demo->window, dc);

  return (ZLay_Dimensions){(float)size.cx, (float)size.cy};
}

static void ZLay_Win32DrawRect(HDC dc, const ZLay_RenderCommand* command) {
  RECT rect;
  HBRUSH brush;

  if (command->color.a == 0u) {
    return;
  }

  rect = ZLay_Win32RectFromBounds(command->bounds);
  brush = CreateSolidBrush(ZLay_Win32ColorRef(command->color));
  if (brush == NULL) {
    return;
  }

  FillRect(dc, &rect, brush);
  DeleteObject(brush);
}

static void ZLay_Win32DrawText(ZLay_Win32ButtonDemo* demo, HDC dc, const ZLay_RenderCommand* command) {
  const int length = command->text.length > (uint32_t)INT32_MAX ? INT32_MAX : (int)command->text.length;
  RECT rect;
  HGDIOBJ old_font;

  if (command->text.chars == NULL || command->text.length == 0u) {
    return;
  }

  rect = ZLay_Win32RectFromBounds(command->bounds);
  old_font = demo->font != NULL ? SelectObject(dc, demo->font) : NULL;
  SetBkMode(dc, TRANSPARENT);
  SetTextColor(dc, ZLay_Win32ColorRef(command->color));
  DrawTextA(dc, command->text.chars, length, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

  if (old_font != NULL) {
    SelectObject(dc, old_font);
  }
}

static void ZLay_Win32RenderCommands(
  ZLay_Win32ButtonDemo* demo,
  HDC target_dc,
  ZLay_RenderCommandList commands
) {
  const int width = demo->width > 0 ? demo->width : 1;
  const int height = demo->height > 0 ? demo->height : 1;
  HDC draw_dc = target_dc;
  HDC buffer_dc = NULL;
  HBITMAP buffer_bitmap = NULL;
  HGDIOBJ old_bitmap = NULL;

  buffer_dc = CreateCompatibleDC(target_dc);
  if (buffer_dc != NULL) {
    buffer_bitmap = CreateCompatibleBitmap(target_dc, width, height);
    if (buffer_bitmap != NULL) {
      old_bitmap = SelectObject(buffer_dc, buffer_bitmap);
      draw_dc = buffer_dc;
    } else {
      DeleteDC(buffer_dc);
      buffer_dc = NULL;
    }
  }

  demo->button_bounds = (ZLay_Rect){0};

  for (uint32_t i = 0; i < commands.count; ++i) {
    const ZLay_RenderCommand* command = &commands.commands[i];

    if (command->type == ZLAY_CMD_RECT) {
      const bool root_rect =
        command->bounds.x == 0.0f &&
        command->bounds.y == 0.0f &&
        command->bounds.width == (float)demo->width &&
        command->bounds.height == (float)demo->height;

      if (!root_rect) {
        demo->button_bounds = command->bounds;
      }

      ZLay_Win32DrawRect(draw_dc, command);
    } else if (command->type == ZLAY_CMD_TEXT) {
      ZLay_Win32DrawText(demo, draw_dc, command);
    }
  }

  if (buffer_dc != NULL) {
    BitBlt(target_dc, 0, 0, width, height, buffer_dc, 0, 0, SRCCOPY);
    if (old_bitmap != NULL) {
      SelectObject(buffer_dc, old_bitmap);
    }
    DeleteObject(buffer_bitmap);
    DeleteDC(buffer_dc);
  }
}

static ZLay_RenderCommandList ZLay_Win32BuildButtonLayout(ZLay_Win32ButtonDemo* demo) {
  ZLay_Theme theme = ZLay_ThemeDark();
  ZLay_Style root_style = ZLay_StyleDefault();
  ZLay_ButtonProps button = ZLay_ButtonPropsDefault(&theme);
  ZLay_NodeDeclarationBuilder root;

  root_style.width = ZLay_Px((float)demo->width);
  root_style.height = ZLay_Px((float)demo->height);
  root_style.padding_left = 32.0f;
  root_style.padding_top = 32.0f;
  root_style.background = theme.canvas;

  button.size = ZLAY_COMPONENT_SIZE_LG;
  if (demo->hovered) {
    button.state |= ZLAY_COMPONENT_STATE_HOVERED;
  }
  if (demo->pressed) {
    button.state |= ZLAY_COMPONENT_STATE_ACTIVE;
  }

  ZLay_SetLayoutDimensions(demo->ctx, (ZLay_Dimensions){(float)demo->width, (float)demo->height});
  ZLay_BeginLayout(demo->ctx);

  root = ZLay_BoxDecl(ZLAY_ID("Root"), root_style);
  ZLAY(demo->ctx, ZLAY_ID("Root"), root) {
    ZLay_Button(demo->ctx, ZLAY_ID("OnlyButton"), ZLAY_STRING_LITERAL("ZLay Button"), &button);
  }

  return ZLay_EndLayout(demo->ctx);
}

static void ZLay_Win32Redraw(ZLay_Win32ButtonDemo* demo) {
  HDC dc;
  ZLay_RenderCommandList commands;

  if (demo == NULL || demo->window == NULL || demo->ctx == NULL) {
    return;
  }

  dc = GetDC(demo->window);
  if (dc == NULL) {
    return;
  }

  commands = ZLay_Win32BuildButtonLayout(demo);
  ZLay_Win32RenderCommands(demo, dc, commands);
  ReleaseDC(demo->window, dc);
}

static void ZLay_Win32Paint(ZLay_Win32ButtonDemo* demo, HDC dc) {
  ZLay_RenderCommandList commands;

  if (demo == NULL || demo->ctx == NULL || dc == NULL) {
    return;
  }

  commands = ZLay_Win32BuildButtonLayout(demo);
  ZLay_Win32RenderCommands(demo, dc, commands);
}

static void ZLay_Win32UpdateHover(ZLay_Win32ButtonDemo* demo, int x, int y) {
  const bool hovered = ZLay_Win32RectContains(demo->button_bounds, x, y);

  if (demo->hovered != hovered) {
    demo->hovered = hovered;
    ZLay_Win32Redraw(demo);
  }
}

static LRESULT CALLBACK ZLay_Win32ButtonDemoProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
  ZLay_Win32ButtonDemo* demo = (ZLay_Win32ButtonDemo*)GetWindowLongPtrA(window, GWLP_USERDATA);

  if (message == WM_NCCREATE) {
    CREATESTRUCTA* create = (CREATESTRUCTA*)lparam;
    demo = (ZLay_Win32ButtonDemo*)create->lpCreateParams;
    SetWindowLongPtrA(window, GWLP_USERDATA, (LONG_PTR)demo);
    if (demo != NULL) {
      demo->window = window;
    }
  }

  switch (message) {
    case WM_SIZE:
      if (demo != NULL) {
        demo->width = LOWORD(lparam);
        demo->height = HIWORD(lparam);
        ZLay_Win32Redraw(demo);
      }
      return 0;

    case WM_PAINT:
      if (demo != NULL) {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(window, &paint);
        ZLay_Win32Paint(demo, dc);
        EndPaint(window, &paint);
        return 0;
      }
      break;

    case WM_MOUSEMOVE:
      if (demo != NULL) {
        TRACKMOUSEEVENT track;
        track.cbSize = sizeof(track);
        track.dwFlags = TME_LEAVE;
        track.hwndTrack = window;
        track.dwHoverTime = 0;
        TrackMouseEvent(&track);
        ZLay_Win32UpdateHover(demo, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
      }
      return 0;

    case WM_MOUSELEAVE:
      if (demo != NULL && (demo->hovered || demo->pressed)) {
        demo->hovered = false;
        demo->pressed = false;
        ZLay_Win32Redraw(demo);
      }
      return 0;

    case WM_LBUTTONDOWN:
      if (demo != NULL &&
          ZLay_Win32RectContains(demo->button_bounds, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam))) {
        SetCapture(window);
        demo->pressed = true;
        demo->hovered = true;
        ZLay_Win32Redraw(demo);
      }
      return 0;

    case WM_LBUTTONUP:
      if (demo != NULL) {
        if (GetCapture() == window) {
          ReleaseCapture();
        }
        demo->pressed = false;
        demo->hovered = ZLay_Win32RectContains(
          demo->button_bounds,
          GET_X_LPARAM(lparam),
          GET_Y_LPARAM(lparam)
        );
        ZLay_Win32Redraw(demo);
      }
      return 0;

    case WM_KEYDOWN:
      if (wparam == VK_ESCAPE) {
        DestroyWindow(window);
        return 0;
      }
      break;

    case WM_CLOSE:
      DestroyWindow(window);
      return 0;

    case WM_DESTROY:
      if (demo != NULL) {
        demo->running = false;
      }
      PostQuitMessage(0);
      return 0;

    case WM_NCDESTROY:
      if (demo != NULL) {
        demo->window = NULL;
      }
      SetWindowLongPtrA(window, GWLP_USERDATA, 0);
      break;

    default:
      break;
  }

  return DefWindowProcA(window, message, wparam, lparam);
}

bool ZLay_Win32ButtonDemo_Init(ZLay_Win32ButtonDemo* demo, HINSTANCE instance, int show_command) {
  ZLay_Config config;
  size_t memory_size;
  WNDCLASSEXA window_class;
  RECT window_rect;
  DWORD style = WS_OVERLAPPEDWINDOW;
  HDC screen_dc;
  int font_height;

  if (demo == NULL) {
    return false;
  }

  *demo = (ZLay_Win32ButtonDemo){0};
  demo->instance = instance != NULL ? instance : GetModuleHandleA(NULL);
  demo->width = 480;
  demo->height = 240;
  demo->running = true;

  window_class = (WNDCLASSEXA){0};
  window_class.cbSize = sizeof(window_class);
  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = ZLay_Win32ButtonDemoProc;
  window_class.hInstance = demo->instance;
  window_class.hCursor = LoadCursorA(NULL, IDC_ARROW);
  window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  window_class.lpszClassName = ZLAY_WIN32_BUTTON_DEMO_CLASS;

  if (RegisterClassExA(&window_class) == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
    return false;
  }

  window_rect.left = 0;
  window_rect.top = 0;
  window_rect.right = demo->width;
  window_rect.bottom = demo->height;
  AdjustWindowRectEx(&window_rect, style, FALSE, 0);

  demo->window = CreateWindowExA(
    0,
    ZLAY_WIN32_BUTTON_DEMO_CLASS,
    "ZLay Win32 Button Layout",
    style,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    window_rect.right - window_rect.left,
    window_rect.bottom - window_rect.top,
    NULL,
    NULL,
    demo->instance,
    demo
  );
  if (demo->window == NULL) {
    return false;
  }

  screen_dc = GetDC(NULL);
  font_height = screen_dc != NULL ? -MulDiv(14, GetDeviceCaps(screen_dc, LOGPIXELSY), 72) : -16;
  if (screen_dc != NULL) {
    ReleaseDC(NULL, screen_dc);
  }
  demo->font = CreateFontA(
    font_height,
    0,
    0,
    0,
    FW_NORMAL,
    FALSE,
    FALSE,
    FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    CLEARTYPE_QUALITY,
    DEFAULT_PITCH | FF_SWISS,
    "Inter"
  );

  config = ZLay_ConfigDefault();
  memory_size = ZLay_MinMemorySize(config);
  demo->memory = calloc(1u, memory_size);
  if (demo->memory == NULL) {
    ZLay_Win32ButtonDemo_Shutdown(demo);
    return false;
  }

  demo->arena = ZLay_CreateArenaWithCapacityAndMemory(memory_size, demo->memory);
  demo->ctx = ZLay_Initialize(demo->arena, config);
  if (demo->ctx == NULL) {
    ZLay_Win32ButtonDemo_Shutdown(demo);
    return false;
  }

  ZLay_SetMeasureTextFunction(demo->ctx, ZLay_Win32MeasureText, demo);
  ShowWindow(demo->window, show_command != 0 ? show_command : SW_SHOWNORMAL);
  UpdateWindow(demo->window);
  return true;
}

void ZLay_Win32ButtonDemo_Run(ZLay_Win32ButtonDemo* demo) {
  MSG message;

  if (demo == NULL || demo->window == NULL) {
    return;
  }

  while (demo->running && GetMessageA(&message, NULL, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessageA(&message);
  }
}

void ZLay_Win32ButtonDemo_Shutdown(ZLay_Win32ButtonDemo* demo) {
  if (demo == NULL) {
    return;
  }

  if (demo->window != NULL) {
    DestroyWindow(demo->window);
  }
  if (demo->font != NULL) {
    DeleteObject(demo->font);
  }

  free(demo->memory);
  *demo = (ZLay_Win32ButtonDemo){0};
}

int main(void) {
  ZLay_Win32ButtonDemo demo;

  if (!ZLay_Win32ButtonDemo_Init(&demo, GetModuleHandleA(NULL), SW_SHOWNORMAL)) {
    return 1;
  }

  ZLay_ShellPrintRule(stdout, "ZLay Win32 Button", ZLAY_SHELL_TONE_ACCENT);
  ZLay_ShellPrintBadge(stdout, "layout", ZLAY_SHELL_TONE_SUCCESS);
  fputs(" one button rendered from ZLay commands. Esc closes the window.\n", stdout);

  ZLay_Win32ButtonDemo_Run(&demo);
  ZLay_Win32ButtonDemo_Shutdown(&demo);
  return 0;
}
