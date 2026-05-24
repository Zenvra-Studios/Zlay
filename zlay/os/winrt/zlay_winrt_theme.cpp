#include <os/winrt/zlay_winrt_theme.h>

#include <os/winrt/internal/zlay_winrt_internal.hpp>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#  define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef RRF_RT_REG_DWORD
#  define RRF_RT_REG_DWORD 0x00000010
#endif

typedef HRESULT(WINAPI* ZLay_DwmSetWindowAttributeFn)(HWND hwnd, DWORD attribute, LPCVOID value, DWORD size);
typedef LSTATUS(WINAPI* ZLay_RegGetValueAFn)(HKEY key, LPCSTR subkey, LPCSTR value, DWORD flags, LPDWORD type, PVOID data, LPDWORD data_size);

static bool ZLay_WinRT_ThemeIsHighContrast() {
  HIGHCONTRASTA high_contrast = {};
  high_contrast.cbSize = sizeof(high_contrast);

  if (!SystemParametersInfoA(SPI_GETHIGHCONTRAST, sizeof(high_contrast), &high_contrast, 0u)) {
    return false;
  }

  return (high_contrast.dwFlags & HCF_HIGHCONTRASTON) != 0u;
}

static ZLay_OSStyleInfo ZLay_WinRT_MakeStyleInfo(
  ZLay_OSThemeMode mode,
  ZLay_Color window_bg,
  ZLay_Color panel_bg,
  ZLay_Color text,
  ZLay_Color accent,
  float corner_radius,
  float control_height,
  const char* font_family) {
  ZLay_OSStyleInfo info = {};
  info.mode = mode;
  info.window_bg = window_bg;
  info.panel_bg = panel_bg;
  info.text = text;
  info.accent = accent;
  info.corner_radius = corner_radius;
  info.control_height = control_height;
  info.font_family = font_family;
  return info;
}

ZLay_OSThemeMode ZLay_WinRT_ThemeMode(void) {
  DWORD value = 1u;
  DWORD value_size = sizeof(value);

  if (ZLay_WinRT_ThemeIsHighContrast()) {
    return ZLAY_OS_THEME_HIGH_CONTRAST;
  }

  ZLay_RegGetValueAFn reg_get_value = reinterpret_cast<ZLay_RegGetValueAFn>(
    zlay::winrt_internal::load_proc("advapi32.dll", "RegGetValueA"));
  if (reg_get_value != nullptr &&
      reg_get_value(
        HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        "AppsUseLightTheme",
        RRF_RT_REG_DWORD,
        nullptr,
        &value,
        &value_size) == ERROR_SUCCESS) {
    return value == 0u ? ZLAY_OS_THEME_DARK : ZLAY_OS_THEME_LIGHT;
  }

  return ZLAY_OS_THEME_LIGHT;
}

ZLay_OSStyleInfo ZLay_WinRT_ThemeStyleInfo(void) {
  ZLay_OSThemeMode mode = ZLay_WinRT_ThemeMode();

  if (mode == ZLAY_OS_THEME_LIGHT) {
    return ZLay_WinRT_MakeStyleInfo(
      ZLAY_OS_THEME_LIGHT,
      {0xFA, 0xFA, 0xFA, 0xFF},
      {0xF2, 0xF2, 0xF2, 0xF4},
      {0x1F, 0x1F, 0x1F, 0xFF},
      {0x00, 0x78, 0xD4, 0xFF},
      8.0f,
      32.0f,
      "Segoe UI Variable");
  }

  if (mode == ZLAY_OS_THEME_HIGH_CONTRAST) {
    return ZLay_WinRT_MakeStyleInfo(
      ZLAY_OS_THEME_HIGH_CONTRAST,
      {0x00, 0x00, 0x00, 0xFF},
      {0x00, 0x00, 0x00, 0xFF},
      {0xFF, 0xFF, 0xFF, 0xFF},
      {0xFF, 0xD8, 0x00, 0xFF},
      2.0f,
      34.0f,
      "Segoe UI");
  }

  return ZLay_WinRT_MakeStyleInfo(
    ZLAY_OS_THEME_DARK,
    {0x20, 0x20, 0x20, 0xEE},
    {0x2B, 0x2B, 0x2B, 0xE8},
    {0xF3, 0xF3, 0xF3, 0xFF},
    {0x4C, 0xA3, 0xFF, 0xFF},
    8.0f,
    32.0f,
    "Segoe UI Variable");
}

bool ZLay_WinRT_ThemeIsDarkMode(void) {
  return ZLay_WinRT_ThemeMode() == ZLAY_OS_THEME_DARK;
}

static bool ZLay_WinRT_ThemeSetDwmInt(HWND hwnd, DWORD attribute, int value) {
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

bool ZLay_WinRT_ThemeSetWindowDarkMode(void* hwnd, bool enabled) {
  int value = enabled ? 1 : 0;

  if (ZLay_WinRT_ThemeSetDwmInt(static_cast<HWND>(hwnd), DWMWA_USE_IMMERSIVE_DARK_MODE, value)) {
    return true;
  }

  return ZLay_WinRT_ThemeSetDwmInt(static_cast<HWND>(hwnd), 19u, value);
}
