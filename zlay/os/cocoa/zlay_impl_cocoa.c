#include <os/cocoa/zlay_impl_cocoa.h>
#include <zlay_os.h>

#include <mach/mach_time.h>

typedef struct ZLay_ImplCocoa_State {
  void* ns_window;
  void* ns_view;
  int32_t width;
  int32_t height;
} ZLay_ImplCocoa_State;

static ZLay_ImplCocoa_State zlay_cocoa_state;

static uint64_t ZLay_ImplCocoa_NowNs(void) {
  static mach_timebase_info_data_t tb = {0};
  if (tb.denom == 0) mach_timebase_info(&tb);
  const uint64_t t = mach_absolute_time();
  return (t * (uint64_t)tb.numer) / (uint64_t)tb.denom;
}

bool ZLay_ImplCocoa_Init(const ZLay_ImplCocoa_InitInfo* info) {
  zlay_cocoa_state.ns_window = info ? info->ns_window : 0;
  zlay_cocoa_state.ns_view = info ? info->ns_view : 0;
  zlay_cocoa_state.width = info ? info->width : 0;
  zlay_cocoa_state.height = info ? info->height : 0;
  return true;
}

bool ZLay_Cocoa_Init(const ZLay_CocoaInitInfo* info) {
  ZLay_ImplCocoa_InitInfo impl;
  impl.ns_window = info ? info->ns_window : 0;
  impl.ns_view = info ? info->ns_view : 0;
  impl.width = info ? info->width : 0;
  impl.height = info ? info->height : 0;
  return ZLay_ImplCocoa_Init(&impl);
}

void ZLay_ImplCocoa_Shutdown(void) {
  zlay_cocoa_state = (ZLay_ImplCocoa_State){0};
}

void ZLay_Cocoa_Shutdown(void) {
  ZLay_ImplCocoa_Shutdown();
}

void ZLay_ImplCocoa_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  zlay_cocoa_state.width = width;
  zlay_cocoa_state.height = height;
  if (ctx) ZLay_SetLayoutDimensions(ctx, (ZLay_Dimensions){(float)width, (float)height});
}

void ZLay_Cocoa_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  ZLay_ImplCocoa_NewFrame(ctx, width, height);
}

float ZLay_ImplCocoa_GetDpiScale(void* ns_view) {
  (void)ns_view;
  return 1.0f;
}

float ZLay_Cocoa_GetDpiScale(void* ns_view) {
  return ZLay_ImplCocoa_GetDpiScale(ns_view);
}

ZLay_OSStyleInfo ZLay_ImplCocoa_GetStyleInfo(void) {
  return (ZLay_OSStyleInfo){
    ZLAY_OS_THEME_DARK,
    {0x18, 0x18, 0x1A, 0xFF},
    {0x24, 0x24, 0x27, 0xFF},
    {0xF5, 0xF5, 0xF7, 0xFF},
    {0x0A, 0x84, 0xFF, 0xFF},
    10.0f,
    30.0f,
    "SF Pro, Helvetica Neue"
  };
}

ZLay_OSStyleInfo ZLay_Cocoa_GetStyleInfo(void) {
  return ZLay_ImplCocoa_GetStyleInfo();
}

const char* ZLay_OS_BackendName(void) {
  return "Cocoa";
}

ZLay_OSBackendType ZLay_OS_BackendType(void) {
  return ZLAY_OS_BACKEND_COCOA;
}

uint32_t ZLay_OS_Capabilities(void) {
  return ZLAY_OS_CAP_MONOTONIC_TIME | ZLAY_OS_CAP_DPI_SCALE | ZLAY_OS_CAP_NATIVE_THEME | ZLAY_OS_CAP_CLIPBOARD | ZLAY_OS_CAP_MOUSE_CURSOR | ZLAY_OS_CAP_TEXT_INPUT;
}

uint64_t ZLay_OS_NowNs(void) {
  return ZLay_ImplCocoa_NowNs();
}

float ZLay_OS_GetDpiScale(void* native_window) {
  return ZLay_ImplCocoa_GetDpiScale(native_window ? native_window : zlay_cocoa_state.ns_view);
}

ZLay_OSStyleInfo ZLay_OS_GetStyleInfo(void) {
  return ZLay_ImplCocoa_GetStyleInfo();
}

void ZLay_OS_NewFrame(ZLay_Context* ctx, const ZLay_OSFrameInfo* frame) {
  if (!frame) return;
  ZLay_ImplCocoa_NewFrame(ctx, frame->width, frame->height);
}
