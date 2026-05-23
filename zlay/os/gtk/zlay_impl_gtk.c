#include <os/gtk/zlay_impl_gtk.h>
#include <os/zlay_os.h>

#include <time.h>

typedef struct ZLay_ImplGTK_State {
  void* widget;
  int32_t width;
  int32_t height;
} ZLay_ImplGTK_State;

static ZLay_ImplGTK_State zlay_gtk_state;

static uint64_t ZLay_ImplGTK_NowNs(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ((uint64_t)ts.tv_sec * 1000000000ull) + (uint64_t)ts.tv_nsec;
}

bool ZLay_ImplGTK_Init(const ZLay_ImplGTK_InitInfo* info) {
  zlay_gtk_state.widget = info ? info->widget : 0;
  zlay_gtk_state.width = info ? info->width : 0;
  zlay_gtk_state.height = info ? info->height : 0;
  return true;
}

bool ZLay_GTK_Init(const ZLay_GTKInitInfo* info) {
  ZLay_ImplGTK_InitInfo impl;
  impl.widget = info ? info->widget : 0;
  impl.width = info ? info->width : 0;
  impl.height = info ? info->height : 0;
  return ZLay_ImplGTK_Init(&impl);
}

void ZLay_ImplGTK_Shutdown(void) {
  zlay_gtk_state = (ZLay_ImplGTK_State){0};
}

void ZLay_GTK_Shutdown(void) {
  ZLay_ImplGTK_Shutdown();
}

void ZLay_ImplGTK_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  zlay_gtk_state.width = width;
  zlay_gtk_state.height = height;
  if (ctx) ZLay_SetLayoutDimensions(ctx, (ZLay_Dimensions){(float)width, (float)height});
}

void ZLay_GTK_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  ZLay_ImplGTK_NewFrame(ctx, width, height);
}

float ZLay_ImplGTK_GetDpiScale(void* widget) {
  (void)widget;
  return 1.0f;
}

float ZLay_GTK_GetDpiScale(void* widget) {
  return ZLay_ImplGTK_GetDpiScale(widget);
}

ZLay_OSStyleInfo ZLay_ImplGTK_GetStyleInfo(void) {
  return (ZLay_OSStyleInfo){
    ZLAY_OS_THEME_DARK,
    {0x1E, 0x1F, 0x22, 0xFF},
    {0x2B, 0x2D, 0x31, 0xFF},
    {0xF4, 0xF4, 0xF2, 0xFF},
    {0x78, 0xAE, 0xED, 0xFF},
    9.0f,
    34.0f,
    "Adwaita Sans, Cantarell, Noto Sans"
  };
}

ZLay_OSStyleInfo ZLay_GTK_GetStyleInfo(void) {
  return ZLay_ImplGTK_GetStyleInfo();
}

const char* ZLay_OS_BackendName(void) {
  return "GTK";
}

ZLay_OSBackendType ZLay_OS_BackendType(void) {
  return ZLAY_OS_BACKEND_GTK;
}

uint32_t ZLay_OS_Capabilities(void) {
  return ZLAY_OS_CAP_MONOTONIC_TIME | ZLAY_OS_CAP_DPI_SCALE | ZLAY_OS_CAP_NATIVE_THEME | ZLAY_OS_CAP_CLIPBOARD | ZLAY_OS_CAP_MOUSE_CURSOR | ZLAY_OS_CAP_TEXT_INPUT;
}

uint64_t ZLay_OS_NowNs(void) {
  return ZLay_ImplGTK_NowNs();
}

float ZLay_OS_GetDpiScale(void* native_window) {
  return ZLay_ImplGTK_GetDpiScale(native_window ? native_window : zlay_gtk_state.widget);
}

ZLay_OSStyleInfo ZLay_OS_GetStyleInfo(void) {
  return ZLay_ImplGTK_GetStyleInfo();
}

void ZLay_OS_NewFrame(ZLay_Context* ctx, const ZLay_OSFrameInfo* frame) {
  if (!frame) return;
  ZLay_ImplGTK_NewFrame(ctx, frame->width, frame->height);
}
