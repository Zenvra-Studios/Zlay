#ifndef ZLAY_OS_WINRT_ZLAY_WINRT_PICKER_H
#define ZLAY_OS_WINRT_ZLAY_WINRT_PICKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include <zlay.h>

typedef struct ZLay_WinRTPickerDesc {
  void* owner_window;
  const char* title;
  const char* filter;
  char* out_path;
  size_t out_path_capacity;
} ZLay_WinRTPickerDesc;

ZLAY_API bool ZLay_WinRT_PickerIsAvailable(void);
ZLAY_API bool ZLay_WinRT_PickerOpenFile(const ZLay_WinRTPickerDesc* desc);
ZLAY_API bool ZLay_WinRT_PickerSaveFile(const ZLay_WinRTPickerDesc* desc);
ZLAY_API bool ZLay_WinRT_PickerPickFolder(const ZLay_WinRTPickerDesc* desc);

#ifdef __cplusplus
}
#endif

#endif
