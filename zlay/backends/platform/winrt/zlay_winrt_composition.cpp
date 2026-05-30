#include <backends/platform/winrt/zlay_winrt_composition.h>

#include <backends/platform/winrt/internal/zlay_winrt_internal.hpp>

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#  define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

typedef HRESULT(WINAPI* ZLay_DwmSetWindowAttributeFn)(HWND hwnd, DWORD attribute, LPCVOID value, DWORD size);
typedef BOOL(WINAPI* ZLay_SetWindowCompositionAttributeFn)(HWND hwnd, void* data);

typedef enum ZLay_WinRTAccentState {
  ZLAY_WINRT_ACCENT_DISABLED = 0,
  ZLAY_WINRT_ACCENT_ENABLE_BLURBEHIND = 3,
  ZLAY_WINRT_ACCENT_ENABLE_ACRYLICBLURBEHIND = 4
} ZLay_WinRTAccentState;

typedef struct ZLay_WinRTAccentPolicy {
  int accent_state;
  int accent_flags;
  int gradient_color;
  int animation_id;
} ZLay_WinRTAccentPolicy;

typedef struct ZLay_WinRTCompositionAttribData {
  int attribute;
  void* data;
  size_t size;
} ZLay_WinRTCompositionAttribData;

static bool ZLay_WinRT_CompositionSetDwmInt(HWND hwnd, DWORD attribute, int value) {
  if (hwnd == nullptr) {
    return false;
  }

  ZLay_DwmSetWindowAttributeFn set_attribute = reinterpret_cast<ZLay_DwmSetWindowAttributeFn>(
    zlay::winrt_internal::load_proc("dwmapi.dll", "DwmSetWindowAttribute"));
  if (set_attribute == nullptr) {
    return false;
  }

  return SUCCEEDED(set_attribute(hwnd, attribute, &value, sizeof(value)));
}

static bool ZLay_WinRT_CompositionSetAccent(HWND hwnd, int accent_state) {
  if (hwnd == nullptr) {
    return false;
  }

  ZLay_SetWindowCompositionAttributeFn set_attribute =
    reinterpret_cast<ZLay_SetWindowCompositionAttributeFn>(
      zlay::winrt_internal::load_proc("user32.dll", "SetWindowCompositionAttribute"));
  if (set_attribute == nullptr) {
    return false;
  }

  ZLay_WinRTAccentPolicy accent = {};
  accent.accent_state = accent_state;
  accent.gradient_color = 0xCC000000;

  ZLay_WinRTCompositionAttribData data = {};
  data.attribute = 19;
  data.data = &accent;
  data.size = sizeof(accent);
  return set_attribute(hwnd, &data) != 0;
}

bool ZLay_WinRT_CompositionSetBackdrop(void* hwnd, ZLay_WinRTBackdrop backdrop) {
  int backdrop_value = 0;

  switch (backdrop) {
    case ZLAY_WINRT_BACKDROP_NONE:
      backdrop_value = 1;
      break;
    case ZLAY_WINRT_BACKDROP_MICA:
      backdrop_value = 2;
      break;
    case ZLAY_WINRT_BACKDROP_ACRYLIC:
      backdrop_value = 3;
      break;
    case ZLAY_WINRT_BACKDROP_TABBED:
      backdrop_value = 4;
      break;
    case ZLAY_WINRT_BACKDROP_DEFAULT:
      backdrop_value = 0;
      break;
    case ZLAY_WINRT_BACKDROP_BLUR:
      return ZLay_WinRT_CompositionSetAccent(
        static_cast<HWND>(hwnd),
        ZLAY_WINRT_ACCENT_ENABLE_BLURBEHIND);
    default:
      backdrop_value = 0;
      break;
  }

  if (ZLay_WinRT_CompositionSetDwmInt(static_cast<HWND>(hwnd), DWMWA_SYSTEMBACKDROP_TYPE, backdrop_value)) {
    return true;
  }

  if (backdrop == ZLAY_WINRT_BACKDROP_ACRYLIC) {
    return ZLay_WinRT_CompositionSetAccent(
      static_cast<HWND>(hwnd),
      ZLAY_WINRT_ACCENT_ENABLE_ACRYLICBLURBEHIND);
  }

  if (backdrop == ZLAY_WINRT_BACKDROP_NONE) {
    return ZLay_WinRT_CompositionSetAccent(
      static_cast<HWND>(hwnd),
      ZLAY_WINRT_ACCENT_DISABLED);
  }

  return false;
}

bool ZLay_WinRT_CompositionEnableBlur(void* hwnd, bool enabled) {
  return ZLay_WinRT_CompositionSetAccent(
    static_cast<HWND>(hwnd),
    enabled ? ZLAY_WINRT_ACCENT_ENABLE_BLURBEHIND : ZLAY_WINRT_ACCENT_DISABLED);
}
