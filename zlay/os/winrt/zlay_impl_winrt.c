#include <os/winrt/zlay_impl_winrt.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct ZLay_ImplWinRT_State {
  void* core_window;
  int32_t width;
  int32_t height;
} ZLay_ImplWinRT_State;

static ZLay_ImplWinRT_State zlay_winrt_state;

static uint64_t ZLay_ImplWinRT_NowNs(void) {
  static LARGE_INTEGER freq = {0};
  if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);

  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return ((uint64_t)now.QuadPart * 1000000000ull) / (uint64_t)freq.QuadPart;
}

bool ZLay_ImplWinRT_Init(const ZLay_ImplWinRT_InitInfo* info) {
  zlay_winrt_state.core_window = info ? info->core_window : 0;
  zlay_winrt_state.width = info ? info->width : 0;
  zlay_winrt_state.height = info ? info->height : 0;
  return true;
}

void ZLay_ImplWinRT_Shutdown(void) {
  zlay_winrt_state = (ZLay_ImplWinRT_State){0};
}

void ZLay_ImplWinRT_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  zlay_winrt_state.width = width;
  zlay_winrt_state.height = height;
  if (ctx) ZLay_SetLayoutDimensions(ctx, (ZLay_Dimensions){(float)width, (float)height});
}

float ZLay_ImplWinRT_GetDpiScale(void* core_window) {
  (void)core_window;
  return 1.0f;
}

ZLay_OSStyleInfo ZLay_ImplWinRT_GetStyleInfo(void) {
  return (ZLay_OSStyleInfo){
    ZLAY_OS_THEME_DARK,
    {0x1B, 0x1B, 0x1F, 0xFF},
    {0x2B, 0x2B, 0x31, 0xFF},
    {0xF4, 0xF4, 0xF7, 0xFF},
    {0x7D, 0xB7, 0xFF, 0xFF},
    8.0f,
    34.0f,
    "Segoe UI Variable, Segoe UI"
  };
}

const char* ZLay_OS_BackendName(void) {
  return "WinRT";
}

ZLay_OSBackendType ZLay_OS_BackendType(void) {
  return ZLAY_OS_BACKEND_WINRT;
}

uint32_t ZLay_OS_Capabilities(void) {
  return ZLAY_OS_CAP_MONOTONIC_TIME | ZLAY_OS_CAP_DPI_SCALE | ZLAY_OS_CAP_NATIVE_THEME | ZLAY_OS_CAP_CLIPBOARD | ZLAY_OS_CAP_MOUSE_CURSOR | ZLAY_OS_CAP_TEXT_INPUT;
}

uint64_t ZLay_OS_NowNs(void) {
  return ZLay_ImplWinRT_NowNs();
}

float ZLay_OS_GetDpiScale(void* native_window) {
  return ZLay_ImplWinRT_GetDpiScale(native_window ? native_window : zlay_winrt_state.core_window);
}

ZLay_OSStyleInfo ZLay_OS_GetStyleInfo(void) {
  return ZLay_ImplWinRT_GetStyleInfo();
}

void ZLay_OS_NewFrame(ZLay_Context* ctx, const ZLay_OSFrameInfo* frame) {
  if (!frame) return;
  ZLay_ImplWinRT_NewFrame(ctx, frame->width, frame->height);
}
