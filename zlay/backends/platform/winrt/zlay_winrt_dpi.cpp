#include <backends/platform/winrt/zlay_winrt_dpi.h>

#include <os/zlay_dpi.h>
#include <backends/platform/winrt/internal/zlay_winrt_internal.hpp>

typedef UINT(WINAPI* ZLay_GetDpiForWindowFn)(HWND hwnd);

float ZLay_WinRT_DpiScaleForWindow(void* hwnd) {
  ZLay_GetDpiForWindowFn get_dpi_for_window = reinterpret_cast<ZLay_GetDpiForWindowFn>(
    zlay::winrt_internal::load_proc("user32.dll", "GetDpiForWindow"));

  if (get_dpi_for_window == nullptr || hwnd == nullptr) {
    return 1.0f;
  }

  return ZLay_DpiScaleFromDpi(static_cast<float>(get_dpi_for_window(static_cast<HWND>(hwnd))));
}

uint32_t ZLay_WinRT_DpiScalePixels(uint32_t pixels, float scale) {
  return ZLay_DpiScalePixels(pixels, scale);
}

ZLay_Dimensions ZLay_WinRT_DpiScaleDimensions(ZLay_Dimensions dimensions, float scale) {
  ZLay_Dimensions scaled = {};
  scaled.width = dimensions.width * scale;
  scaled.height = dimensions.height * scale;
  return scaled;
}
