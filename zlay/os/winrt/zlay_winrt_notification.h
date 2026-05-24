#ifndef ZLAY_OS_WINRT_ZLAY_WINRT_NOTIFICATION_H
#define ZLAY_OS_WINRT_ZLAY_WINRT_NOTIFICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>
#include <os/winrt/zlay_winrt_modern.h>

typedef struct ZLay_WinRTToastDesc {
  const char* title;
  const char* body;
  const char* tag;
} ZLay_WinRTToastDesc;

ZLAY_API bool ZLay_WinRT_NotificationIsAvailable(void);
ZLAY_API bool ZLay_WinRT_NotificationShowToast(const ZLay_WinRTToastDesc* desc);
ZLAY_API bool ZLay_WinRT_NotificationTrayAdd(const ZLay_WinRTTrayIconDesc* desc);
ZLAY_API bool ZLay_WinRT_NotificationTrayUpdate(const ZLay_WinRTTrayIconDesc* desc);
ZLAY_API bool ZLay_WinRT_NotificationTraySetTooltip(void* hwnd, uint32_t id, const char* tooltip);
ZLAY_API bool ZLay_WinRT_NotificationTrayRemove(void* hwnd, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif
