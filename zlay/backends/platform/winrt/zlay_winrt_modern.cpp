#include <backends/platform/winrt/zlay_winrt_modern.h>

#include <backends/platform/winrt/zlay_winrt_composition.h>
#include <backends/platform/winrt/zlay_winrt_dpi.h>
#include <backends/platform/winrt/zlay_winrt_notification.h>
#include <backends/platform/winrt/zlay_winrt_picker.h>
#include <backends/platform/winrt/zlay_winrt_theme.h>
#include <backends/platform/winrt/internal/zlay_winrt_internal.hpp>

typedef struct ZLay_WinRTModernState {
  void* native_window;
  uint32_t flags;
  bool active;
  bool apartment_initialized;
} ZLay_WinRTModernState;

static ZLay_WinRTModernState zlay_winrt_modern_state;

static bool ZLay_WinRT_ModernWants(uint32_t flags, uint32_t feature) {
  return flags == ZLAY_WINRT_MODERN_ALL || (flags & feature) != 0u;
}

bool ZLay_WinRTModernEnable(const ZLay_WinRTModernDesc* desc) {
  uint32_t flags;
  bool ok = true;
  ZLay_WinRTBackdrop backdrop = ZLAY_WINRT_BACKDROP_DEFAULT;

  if (desc == nullptr || desc->native_window == nullptr || !ZLay_WinRTIsAvailable()) {
    return false;
  }

  flags = desc->flags != ZLAY_WINRT_MODERN_NONE ? desc->flags : ZLAY_WINRT_DEFAULT_MODERN_FLAGS;

  zlay::winrt_internal::initialize_apartment();
  zlay_winrt_modern_state.apartment_initialized = true;
  zlay_winrt_modern_state.native_window = desc->native_window;
  zlay_winrt_modern_state.flags = flags;
  zlay_winrt_modern_state.active = true;

  if (ZLay_WinRT_ModernWants(flags, ZLAY_WINRT_MODERN_THEME) && desc->enable_dark_mode) {
    ok = ZLay_WinRT_ThemeSetWindowDarkMode(desc->native_window, ZLay_WinRT_ThemeMode() != ZLAY_OS_THEME_LIGHT) && ok;
  }

  if (ZLay_WinRT_ModernWants(flags, ZLAY_WINRT_MODERN_DPI)) {
    ok = ZLay_WinRT_DpiScaleForWindow(desc->native_window) > 0.0f && ok;
  }

  if (ZLay_WinRT_ModernWants(flags, ZLAY_WINRT_MODERN_COMPOSITION)) {
    if (desc->enable_acrylic) {
      backdrop = ZLAY_WINRT_BACKDROP_ACRYLIC;
    } else if (desc->enable_mica) {
      backdrop = ZLAY_WINRT_BACKDROP_MICA;
    }

    if (backdrop != ZLAY_WINRT_BACKDROP_DEFAULT) {
      ok = ZLay_WinRT_CompositionSetBackdrop(desc->native_window, backdrop) && ok;
    }
  }

  return ok;
}

void ZLay_WinRTModernDisable(void) {
  if (zlay_winrt_modern_state.active && zlay_winrt_modern_state.native_window != nullptr) {
    if (ZLay_WinRT_ModernWants(zlay_winrt_modern_state.flags, ZLAY_WINRT_MODERN_COMPOSITION)) {
      ZLay_WinRT_CompositionSetBackdrop(zlay_winrt_modern_state.native_window, ZLAY_WINRT_BACKDROP_NONE);
    }
  }

  if (zlay_winrt_modern_state.apartment_initialized) {
    zlay::winrt_internal::uninitialize_apartment();
  }

  zlay_winrt_modern_state = {};
}

bool ZLay_WinRTIsAvailable(void) {
  return zlay::winrt_internal::is_windows_modern_available();
}

ZLay_OSThemeMode ZLay_WinRT_GetSystemThemeMode(void) {
  return ZLay_WinRT_ThemeMode();
}

ZLay_OSStyleInfo ZLay_WinRT_GetSystemStyleInfo(void) {
  return ZLay_WinRT_ThemeStyleInfo();
}

bool ZLay_WinRT_IsSystemDarkMode(void) {
  return ZLay_WinRT_ThemeIsDarkMode();
}

bool ZLay_WinRT_ApplyWindowSystemStyle(void* hwnd, ZLay_WinRTBackdrop backdrop) {
  bool ok;

  if (hwnd == nullptr) {
    return false;
  }

  ok = ZLay_WinRT_ThemeSetWindowDarkMode(hwnd, ZLay_WinRT_ThemeMode() != ZLAY_OS_THEME_LIGHT);
  if (backdrop != ZLAY_WINRT_BACKDROP_DEFAULT) {
    ok = ZLay_WinRT_CompositionSetBackdrop(hwnd, backdrop) && ok;
  }
  return ok;
}

bool ZLay_WinRT_SetWindowDarkMode(void* hwnd, bool enabled) {
  return ZLay_WinRT_ThemeSetWindowDarkMode(hwnd, enabled);
}

bool ZLay_WinRT_SetWindowBackdrop(void* hwnd, ZLay_WinRTBackdrop backdrop) {
  return ZLay_WinRT_CompositionSetBackdrop(hwnd, backdrop);
}

bool ZLay_WinRT_EnableWindowBlur(void* hwnd, bool enabled) {
  return ZLay_WinRT_CompositionEnableBlur(hwnd, enabled);
}

bool ZLay_WinRT_TrayAdd(const ZLay_WinRTTrayIconDesc* desc) {
  return ZLay_WinRT_NotificationTrayAdd(desc);
}

bool ZLay_WinRT_TrayUpdate(const ZLay_WinRTTrayIconDesc* desc) {
  return ZLay_WinRT_NotificationTrayUpdate(desc);
}

bool ZLay_WinRT_TraySetTooltip(void* hwnd, uint32_t id, const char* tooltip) {
  return ZLay_WinRT_NotificationTraySetTooltip(hwnd, id, tooltip);
}

bool ZLay_WinRT_TrayRemove(void* hwnd, uint32_t id) {
  return ZLay_WinRT_NotificationTrayRemove(hwnd, id);
}
