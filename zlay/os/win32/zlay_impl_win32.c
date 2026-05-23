#include <os/win32/zlay_impl_win32.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct ZLay_ImplWin32_State {
  void* hwnd;
  int32_t width;
  int32_t height;
} ZLay_ImplWin32_State;

static ZLay_ImplWin32_State zlay_win32_state;

typedef UINT(WINAPI* ZLay_GetDpiForWindowFn)(HWND hwnd);
typedef BOOL(WINAPI* ZLay_SetProcessDPIAwareFn)(void);

static uint64_t ZLay_ImplWin32_NowNs(void) {
  static LARGE_INTEGER freq = {0};
  if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);

  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return ((uint64_t)now.QuadPart * 1000000000ull) / (uint64_t)freq.QuadPart;
}

bool ZLay_ImplWin32_Init(const ZLay_ImplWin32_InitInfo* info) {
  zlay_win32_state.hwnd = info ? info->hwnd : 0;
  zlay_win32_state.width = info ? info->width : 0;
  zlay_win32_state.height = info ? info->height : 0;
  return true;
}

void ZLay_ImplWin32_Shutdown(void) {
  zlay_win32_state = (ZLay_ImplWin32_State){0};
}

void ZLay_ImplWin32_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  zlay_win32_state.width = width;
  zlay_win32_state.height = height;
  if (ctx) ZLay_SetLayoutDimensions(ctx, (ZLay_Dimensions){(float)width, (float)height});
}

void ZLay_ImplWin32_EnableDpiAwareness(void) {
  HMODULE user32 = LoadLibraryA("user32.dll");
  if (!user32) return;

  ZLay_SetProcessDPIAwareFn set_process_dpi_aware = (ZLay_SetProcessDPIAwareFn)GetProcAddress(user32, "SetProcessDPIAware");
  if (set_process_dpi_aware) set_process_dpi_aware();
  FreeLibrary(user32);
}

float ZLay_ImplWin32_GetDpiScaleForHwnd(void* hwnd) {
  HMODULE user32 = LoadLibraryA("user32.dll");
  if (user32) {
    ZLay_GetDpiForWindowFn get_dpi_for_window = (ZLay_GetDpiForWindowFn)GetProcAddress(user32, "GetDpiForWindow");
    if (get_dpi_for_window && hwnd) {
      const UINT dpi = get_dpi_for_window((HWND)hwnd);
      FreeLibrary(user32);
      return dpi > 0 ? ((float)dpi / 96.0f) : 1.0f;
    }
    FreeLibrary(user32);
  }
  return 1.0f;
}

ZLay_OSStyleInfo ZLay_ImplWin32_GetStyleInfo(void) {
  return (ZLay_OSStyleInfo){
    ZLAY_OS_THEME_DARK,
    {0x10, 0x10, 0x10, 0xFF},
    {0x20, 0x20, 0x20, 0xFF},
    {0xF5, 0xF5, 0xF5, 0xFF},
    {0x60, 0xA5, 0xFA, 0xFF},
    6.0f,
    32.0f,
    "Segoe UI Variable, Segoe UI"
  };
}

const char* ZLay_OS_BackendName(void) {
  return "Win32";
}

ZLay_OSBackendType ZLay_OS_BackendType(void) {
  return ZLAY_OS_BACKEND_WIN32;
}

uint32_t ZLay_OS_Capabilities(void) {
  return ZLAY_OS_CAP_MONOTONIC_TIME | ZLAY_OS_CAP_DPI_SCALE | ZLAY_OS_CAP_NATIVE_THEME | ZLAY_OS_CAP_CLIPBOARD | ZLAY_OS_CAP_MOUSE_CURSOR | ZLAY_OS_CAP_TEXT_INPUT;
}

uint64_t ZLay_OS_NowNs(void) {
  return ZLay_ImplWin32_NowNs();
}

float ZLay_OS_GetDpiScale(void* native_window) {
  return ZLay_ImplWin32_GetDpiScaleForHwnd(native_window ? native_window : zlay_win32_state.hwnd);
}

ZLay_OSStyleInfo ZLay_OS_GetStyleInfo(void) {
  return ZLay_ImplWin32_GetStyleInfo();
}

void ZLay_OS_NewFrame(ZLay_Context* ctx, const ZLay_OSFrameInfo* frame) {
  if (!frame) return;
  ZLay_ImplWin32_NewFrame(ctx, frame->width, frame->height);
}
