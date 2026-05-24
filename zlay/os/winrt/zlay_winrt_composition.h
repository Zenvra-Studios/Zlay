#ifndef ZLAY_OS_WINRT_ZLAY_WINRT_COMPOSITION_H
#define ZLAY_OS_WINRT_ZLAY_WINRT_COMPOSITION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <os/winrt/zlay_winrt_modern.h>

ZLAY_API bool ZLay_WinRT_CompositionSetBackdrop(void* hwnd, ZLay_WinRTBackdrop backdrop);
ZLAY_API bool ZLay_WinRT_CompositionEnableBlur(void* hwnd, bool enabled);

#ifdef __cplusplus
}
#endif

#endif
