#ifndef ZLAY_OS_WINRT_ZLAY_WINRT_THEME_H
#define ZLAY_OS_WINRT_ZLAY_WINRT_THEME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <os/zlay_os.h>

ZLAY_API ZLay_OSThemeMode ZLay_WinRT_ThemeMode(void);
ZLAY_API ZLay_OSStyleInfo ZLay_WinRT_ThemeStyleInfo(void);
ZLAY_API bool ZLay_WinRT_ThemeIsDarkMode(void);
ZLAY_API bool ZLay_WinRT_ThemeSetWindowDarkMode(void* hwnd, bool enabled);

#ifdef __cplusplus
}
#endif

#endif
