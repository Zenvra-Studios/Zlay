#include <backends/platform/winrt/zlay_winrt_notification.h>

#include <backends/platform/winrt/internal/zlay_winrt_internal.hpp>

#include <shellapi.h>

#ifndef NOTIFYICON_VERSION_4
#  define NOTIFYICON_VERSION_4 4
#endif

typedef BOOL(WINAPI* ZLay_ShellNotifyIconAFn)(DWORD message, PNOTIFYICONDATAA data);

bool ZLay_WinRT_NotificationIsAvailable(void) {
  return zlay::winrt_internal::is_windows_modern_available();
}

bool ZLay_WinRT_NotificationShowToast(const ZLay_WinRTToastDesc* desc) {
  (void)desc;
  return false;
}

static bool ZLay_WinRT_NotificationFillTrayData(NOTIFYICONDATAA* data, const ZLay_WinRTTrayIconDesc* desc) {
  if (data == nullptr || desc == nullptr || desc->hwnd == nullptr) {
    return false;
  }

  *data = {};
  data->cbSize = sizeof(*data);
  data->hWnd = static_cast<HWND>(desc->hwnd);
  data->uID = desc->id;
  data->uFlags = NIF_MESSAGE;
  data->uCallbackMessage = desc->callback_message;

  if (desc->icon != nullptr) {
    data->uFlags |= NIF_ICON;
    data->hIcon = static_cast<HICON>(desc->icon);
  }

  if (desc->tooltip != nullptr) {
    data->uFlags |= NIF_TIP;
    lstrcpynA(data->szTip, desc->tooltip, sizeof(data->szTip));
  }

  return true;
}

static bool ZLay_WinRT_NotificationTrayCall(DWORD message, NOTIFYICONDATAA* data) {
  ZLay_ShellNotifyIconAFn shell_notify_icon = reinterpret_cast<ZLay_ShellNotifyIconAFn>(
    zlay::winrt_internal::load_proc("shell32.dll", "Shell_NotifyIconA"));

  if (shell_notify_icon == nullptr || data == nullptr) {
    return false;
  }

  return shell_notify_icon(message, data) != 0;
}

bool ZLay_WinRT_NotificationTrayAdd(const ZLay_WinRTTrayIconDesc* desc) {
  NOTIFYICONDATAA data = {};

  if (!ZLay_WinRT_NotificationFillTrayData(&data, desc)) {
    return false;
  }

  if (!ZLay_WinRT_NotificationTrayCall(NIM_ADD, &data)) {
    return false;
  }

  data.uVersion = NOTIFYICON_VERSION_4;
  ZLay_WinRT_NotificationTrayCall(NIM_SETVERSION, &data);
  return true;
}

bool ZLay_WinRT_NotificationTrayUpdate(const ZLay_WinRTTrayIconDesc* desc) {
  NOTIFYICONDATAA data = {};

  if (!ZLay_WinRT_NotificationFillTrayData(&data, desc)) {
    return false;
  }

  return ZLay_WinRT_NotificationTrayCall(NIM_MODIFY, &data);
}

bool ZLay_WinRT_NotificationTraySetTooltip(void* hwnd, uint32_t id, const char* tooltip) {
  if (hwnd == nullptr) {
    return false;
  }

  NOTIFYICONDATAA data = {};
  data.cbSize = sizeof(data);
  data.hWnd = static_cast<HWND>(hwnd);
  data.uID = id;
  data.uFlags = NIF_TIP;
  if (tooltip != nullptr) {
    lstrcpynA(data.szTip, tooltip, sizeof(data.szTip));
  }

  return ZLay_WinRT_NotificationTrayCall(NIM_MODIFY, &data);
}

bool ZLay_WinRT_NotificationTrayRemove(void* hwnd, uint32_t id) {
  if (hwnd == nullptr) {
    return false;
  }

  NOTIFYICONDATAA data = {};
  data.cbSize = sizeof(data);
  data.hWnd = static_cast<HWND>(hwnd);
  data.uID = id;
  return ZLay_WinRT_NotificationTrayCall(NIM_DELETE, &data);
}
