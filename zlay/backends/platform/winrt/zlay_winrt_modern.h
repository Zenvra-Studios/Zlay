#ifndef ZLAY_BACKENDS_PLATFORM_WINRT_ZLAY_WINRT_MODERN_H
#define ZLAY_BACKENDS_PLATFORM_WINRT_ZLAY_WINRT_MODERN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <os/zlay_os.h>
#include <backends/platform/winrt/zlay_winrt_config.h>

typedef enum ZLay_WinRTModernFlags {
  ZLAY_WINRT_MODERN_NONE = 0,
  ZLAY_WINRT_MODERN_DPI = 1u << 0,
  ZLAY_WINRT_MODERN_THEME = 1u << 1,
  ZLAY_WINRT_MODERN_COMPOSITION = 1u << 2,
  ZLAY_WINRT_MODERN_NOTIFICATION = 1u << 3,
  ZLAY_WINRT_MODERN_PICKER = 1u << 4,
  ZLAY_WINRT_MODERN_ALL = 0xFFFFFFFFu
} ZLay_WinRTModernFlags;

typedef enum ZLay_WinRTBackdrop {
  ZLAY_WINRT_BACKDROP_DEFAULT = 0,
  ZLAY_WINRT_BACKDROP_NONE,
  ZLAY_WINRT_BACKDROP_BLUR,
  ZLAY_WINRT_BACKDROP_ACRYLIC,
  ZLAY_WINRT_BACKDROP_MICA,
  ZLAY_WINRT_BACKDROP_TABBED
} ZLay_WinRTBackdrop;

typedef struct ZLay_WinRTTrayIconDesc {
  void* hwnd;
  uint32_t id;
  void* icon;
  uint32_t callback_message;
  const char* tooltip;
} ZLay_WinRTTrayIconDesc;

typedef struct ZLay_WinRTModernDesc {
  void* native_window;
  uint32_t flags;
  bool enable_mica;
  bool enable_acrylic;
  bool enable_dark_mode;
  bool enable_notifications;
  bool enable_picker;
} ZLay_WinRTModernDesc;

ZLAY_API bool ZLay_WinRTModernEnable(const ZLay_WinRTModernDesc* desc);
ZLAY_API void ZLay_WinRTModernDisable(void);
ZLAY_API bool ZLay_WinRTIsAvailable(void);
ZLAY_API ZLay_OSThemeMode ZLay_WinRT_GetSystemThemeMode(void);
ZLAY_API ZLay_OSStyleInfo ZLay_WinRT_GetSystemStyleInfo(void);
ZLAY_API bool ZLay_WinRT_IsSystemDarkMode(void);
ZLAY_API bool ZLay_WinRT_ApplyWindowSystemStyle(void* hwnd, ZLay_WinRTBackdrop backdrop);
ZLAY_API bool ZLay_WinRT_SetWindowDarkMode(void* hwnd, bool enabled);
ZLAY_API bool ZLay_WinRT_SetWindowBackdrop(void* hwnd, ZLay_WinRTBackdrop backdrop);
ZLAY_API bool ZLay_WinRT_EnableWindowBlur(void* hwnd, bool enabled);
ZLAY_API bool ZLay_WinRT_TrayAdd(const ZLay_WinRTTrayIconDesc* desc);
ZLAY_API bool ZLay_WinRT_TrayUpdate(const ZLay_WinRTTrayIconDesc* desc);
ZLAY_API bool ZLay_WinRT_TraySetTooltip(void* hwnd, uint32_t id, const char* tooltip);
ZLAY_API bool ZLay_WinRT_TrayRemove(void* hwnd, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif
