#include <os/libx11/zlay_impl_x11.h>
#include  <zlay_os.h>

#include <time.h>

typedef struct ZLay_ImplX11_State {
  void* display;
  unsigned long window;
  int32_t width;
  int32_t height;
} ZLay_ImplX11_State;

static ZLay_ImplX11_State zlay_x11_state;

static uint64_t ZLay_ImplUnix_NowNs(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ((uint64_t)ts.tv_sec * 1000000000ull) + (uint64_t)ts.tv_nsec;
}

bool ZLay_ImplX11_Init(const ZLay_ImplX11_InitInfo* info) {
  zlay_x11_state.display = info ? info->display : 0;
  zlay_x11_state.window = info ? info->window : 0;
  zlay_x11_state.width = info ? info->width : 0;
  zlay_x11_state.height = info ? info->height : 0;
  return true;
}

bool ZLay_X11_Init(const ZLay_X11InitInfo* info) {
  ZLay_ImplX11_InitInfo impl;
  impl.display = info ? info->display : 0;
  impl.window = info ? info->window : 0;
  impl.width = info ? info->width : 0;
  impl.height = info ? info->height : 0;
  return ZLay_ImplX11_Init(&impl);
}

void ZLay_ImplX11_Shutdown(void) {
  zlay_x11_state = (ZLay_ImplX11_State){0};
}

void ZLay_X11_Shutdown(void) {
  ZLay_ImplX11_Shutdown();
}

void ZLay_ImplX11_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  zlay_x11_state.width = width;
  zlay_x11_state.height = height;
  if (ctx) ZLay_SetLayoutDimensions(ctx, (ZLay_Dimensions){(float)width, (float)height});
}

void ZLay_X11_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  ZLay_ImplX11_NewFrame(ctx, width, height);
}

float ZLay_ImplX11_GetDpiScale(void* display) {
  (void)display;
  return 1.0f;
}

float ZLay_X11_GetDpiScale(void* display) {
  return ZLay_ImplX11_GetDpiScale(display);
}

ZLay_OSStyleInfo ZLay_ImplX11_GetStyleInfo(void) {
  return (ZLay_OSStyleInfo){
    ZLAY_OS_THEME_DARK,
    {0x12, 0x14, 0x18, 0xFF},
    {0x1A, 0x1D, 0x24, 0xFF},
    {0xF2, 0xF4, 0xF8, 0xFF},
    {0x5E, 0x9B, 0xFF, 0xFF},
    8.0f,
    32.0f,
    "Inter, Noto Sans, DejaVu Sans"
  };
}

ZLay_OSStyleInfo ZLay_X11_GetStyleInfo(void) {
  return ZLay_ImplX11_GetStyleInfo();
}

const char* ZLay_OS_BackendName(void) {
  return "X11";
}

ZLay_OSBackendType ZLay_OS_BackendType(void) {
  return ZLAY_OS_BACKEND_X11;
}

uint32_t ZLay_OS_Capabilities(void) {
  return ZLAY_OS_CAP_MONOTONIC_TIME | ZLAY_OS_CAP_DPI_SCALE | ZLAY_OS_CAP_NATIVE_THEME | ZLAY_OS_CAP_MOUSE_CURSOR | ZLAY_OS_CAP_TEXT_INPUT;
}

uint64_t ZLay_OS_NowNs(void) {
  return ZLay_ImplUnix_NowNs();
}

float ZLay_OS_GetDpiScale(void* native_window) {
  (void)native_window;
  return ZLay_ImplX11_GetDpiScale(zlay_x11_state.display);
}

ZLay_OSStyleInfo ZLay_OS_GetStyleInfo(void) {
  return ZLay_ImplX11_GetStyleInfo();
}

void ZLay_OS_NewFrame(ZLay_Context* ctx, const ZLay_OSFrameInfo* frame) {
  if (!frame) return;
  ZLay_ImplX11_NewFrame(ctx, frame->width, frame->height);
}
