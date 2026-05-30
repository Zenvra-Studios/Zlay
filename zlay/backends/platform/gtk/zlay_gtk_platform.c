#include <backends/platform/gtk/zlay_gtk_backend.h>

#include <stdlib.h>
#include <string.h>

static ZLay_GTK_State zlay_gtk_state;

ZLay_GTK_State* ZLay_GTK_StateGet(void) {
  return &zlay_gtk_state;
}

static float ZLay_GTK_EnvScale(void) {
  const char* value = getenv("GDK_SCALE");
  char* end = NULL;
  float scale;

  if (value == NULL || value[0] == '\0') {
    return 1.0f;
  }

  scale = strtof(value, &end);
  return end != value && scale > 0.0f ? scale : 1.0f;
}

ZLay_GTKDisplayServer ZLay_GTK_DetectDisplayServer(const ZLay_GTKInitInfo* info) {
  const char* wayland_display;
  const char* x11_display;
  const char* broadway_display;

  if (info != NULL) {
    if (info->display_server != ZLAY_GTK_DISPLAY_UNKNOWN) {
      return info->display_server;
    }
    if (info->wl_display != NULL || info->wl_surface != NULL) {
      return ZLAY_GTK_DISPLAY_WAYLAND;
    }
    if (info->x11_display != NULL || info->x11_window != 0ul) {
      return ZLAY_GTK_DISPLAY_X11;
    }
  }

  broadway_display = getenv("BROADWAY_DISPLAY");
  if (broadway_display != NULL && broadway_display[0] != '\0') {
    return ZLAY_GTK_DISPLAY_BROADWAY;
  }

  wayland_display = getenv("WAYLAND_DISPLAY");
  if (wayland_display != NULL && wayland_display[0] != '\0') {
    return ZLAY_GTK_DISPLAY_WAYLAND;
  }

  x11_display = getenv("DISPLAY");
  if (x11_display != NULL && x11_display[0] != '\0') {
    return ZLAY_GTK_DISPLAY_X11;
  }

  return ZLAY_GTK_DISPLAY_UNKNOWN;
}

ZLay_Platform ZLay_GTK_PlatformForDisplayServer(ZLay_GTKDisplayServer display_server) {
  switch (display_server) {
    case ZLAY_GTK_DISPLAY_WAYLAND:
      return ZLAY_OS_PLATFORM_WAYLAND;
    case ZLAY_GTK_DISPLAY_X11:
      return ZLAY_OS_PLATFORM_X11;
    case ZLAY_GTK_DISPLAY_BROADWAY:
    case ZLAY_GTK_DISPLAY_HEADLESS:
    case ZLAY_GTK_DISPLAY_UNKNOWN:
    default:
      return ZLAY_OS_PLATFORM_GTK;
  }
}

const char* ZLay_GTK_DisplayServerName(ZLay_GTKDisplayServer display_server) {
  switch (display_server) {
    case ZLAY_GTK_DISPLAY_WAYLAND:
      return "Wayland";
    case ZLAY_GTK_DISPLAY_X11:
      return "X11";
    case ZLAY_GTK_DISPLAY_BROADWAY:
      return "Broadway";
    case ZLAY_GTK_DISPLAY_HEADLESS:
      return "Headless";
    case ZLAY_GTK_DISPLAY_UNKNOWN:
    default:
      return "GTK";
  }
}

bool ZLay_GTK_Init(const ZLay_GTKInitInfo* info) {
  zlay_gtk_state.widget = info != NULL ? info->widget : NULL;
  zlay_gtk_state.gdk_display = info != NULL ? info->gdk_display : NULL;
  zlay_gtk_state.gdk_surface = info != NULL ? info->gdk_surface : NULL;
  zlay_gtk_state.wl_display = info != NULL ? info->wl_display : NULL;
  zlay_gtk_state.wl_surface = info != NULL ? info->wl_surface : NULL;
  zlay_gtk_state.x11_display = info != NULL ? info->x11_display : NULL;
  zlay_gtk_state.x11_window = info != NULL ? info->x11_window : 0ul;
  zlay_gtk_state.width = info != NULL ? info->width : 0;
  zlay_gtk_state.height = info != NULL ? info->height : 0;
  zlay_gtk_state.scale = info != NULL && info->scale > 0.0f ? info->scale : ZLay_GTK_EnvScale();
  zlay_gtk_state.display_server = ZLay_GTK_DetectDisplayServer(info);
  zlay_gtk_state.prefer_dark_theme = info == NULL || info->prefer_dark_theme;
  zlay_gtk_state.font_family = info != NULL && info->font_family != NULL ? info->font_family : "Adwaita Sans, Cantarell, Inter, Noto Sans, sans";
  zlay_gtk_state.get_dpi_scale = info != NULL ? info->get_dpi_scale : NULL;
  zlay_gtk_state.get_style_info = info != NULL ? info->get_style_info : NULL;
  zlay_gtk_state.clipboard_set_text = info != NULL ? info->clipboard_set_text : NULL;
  zlay_gtk_state.clipboard_get_text = info != NULL ? info->clipboard_get_text : NULL;
  zlay_gtk_state.set_cursor = info != NULL ? info->set_cursor : NULL;
  zlay_gtk_state.poll_events = info != NULL ? info->poll_events : NULL;
  zlay_gtk_state.user = info != NULL ? info->user : NULL;
  return true;
}

void ZLay_GTK_Shutdown(void) {
  free(zlay_gtk_state.clipboard_text);
  zlay_gtk_state.clipboard_text = NULL;
  zlay_gtk_state = (ZLay_GTK_State){0};
}

void ZLay_GTK_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  zlay_gtk_state.width = width;
  zlay_gtk_state.height = height;
  if (ctx != NULL) {
    ZLay_SetLayoutDimensions(ctx, (ZLay_Dimensions){(float)width, (float)height});
  }
}

float ZLay_GTK_GetDpiScale(void* widget) {
  float scale;

  if (zlay_gtk_state.get_dpi_scale != NULL) {
    scale = zlay_gtk_state.get_dpi_scale(widget != NULL ? widget : zlay_gtk_state.widget, zlay_gtk_state.user);
    if (scale > 0.0f) {
      return scale;
    }
  }

  return zlay_gtk_state.scale > 0.0f ? zlay_gtk_state.scale : 1.0f;
}

ZLay_OSStyleInfo ZLay_GTK_GetStyleInfo(void) {
  if (zlay_gtk_state.get_style_info != NULL) {
    return zlay_gtk_state.get_style_info(zlay_gtk_state.user);
  }

  if (!zlay_gtk_state.prefer_dark_theme) {
    return (ZLay_OSStyleInfo){
      ZLAY_OS_THEME_LIGHT,
      {0xFA, 0xFA, 0xFA, 0xFF},
      {0xFF, 0xFF, 0xFF, 0xFF},
      {0x24, 0x24, 0x24, 0xFF},
      {0x35, 0x84, 0xE4, 0xFF},
      9.0f,
      34.0f,
      zlay_gtk_state.font_family != NULL ? zlay_gtk_state.font_family : ZLAY_DEFAULT_FONT_FAMILY
    };
  }

  return (ZLay_OSStyleInfo){
    ZLAY_OS_THEME_DARK,
    {0x1E, 0x1F, 0x22, 0xFF},
    {0x2B, 0x2D, 0x31, 0xFF},
    {0xF4, 0xF4, 0xF2, 0xFF},
    {0x78, 0xAE, 0xED, 0xFF},
    9.0f,
    34.0f,
    zlay_gtk_state.font_family != NULL ? zlay_gtk_state.font_family : ZLAY_DEFAULT_FONT_FAMILY
  };
}

ZLay_GTKDisplayServer ZLay_GTK_GetDisplayServer(void) {
  return zlay_gtk_state.display_server;
}

void ZLay_GTK_LinkWindow(ZLay_Window* window) {
  if (window == NULL) return;
  window->next = zlay_gtk_state.windows;
  zlay_gtk_state.windows = window;
}

void ZLay_GTK_UnlinkWindow(ZLay_Window* window) {
  ZLay_Window** link = &zlay_gtk_state.windows;

  while (*link != NULL) {
    if (*link == window) {
      *link = window->next;
      window->next = NULL;
      return;
    }
    link = &(*link)->next;
  }
}

ZLay_Window* ZLay_GTK_FirstWindow(void) {
  return zlay_gtk_state.windows;
}

const char* ZLay_OS_BackendName(void) {
  return "GTK";
}

ZLay_OSBackendType ZLay_OS_BackendType(void) {
  return ZLAY_OS_BACKEND_GTK;
}

uint32_t ZLay_OS_Capabilities(void) {
  return ZLAY_OS_CAP_MONOTONIC_TIME |
         ZLAY_OS_CAP_DPI_SCALE |
         ZLAY_OS_CAP_NATIVE_THEME |
         ZLAY_OS_CAP_CLIPBOARD |
         ZLAY_OS_CAP_MOUSE_CURSOR |
         ZLAY_OS_CAP_TEXT_INPUT;
}

uint64_t ZLay_OS_NowNs(void) {
  return ZLay_GTK_NowNs();
}

float ZLay_OS_GetDpiScale(void* native_window) {
  return ZLay_GTK_GetDpiScale(native_window);
}

ZLay_OSStyleInfo ZLay_OS_GetStyleInfo(void) {
  return ZLay_GTK_GetStyleInfo();
}

void ZLay_OS_NewFrame(ZLay_Context* ctx, const ZLay_OSFrameInfo* frame) {
  if (frame == NULL) return;
  ZLay_GTK_NewFrame(ctx, frame->width, frame->height);
}
