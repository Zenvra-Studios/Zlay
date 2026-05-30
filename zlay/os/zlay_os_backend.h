#ifndef ZLAY_OS_ZLAY_OS_BACKEND_H
#define ZLAY_OS_ZLAY_OS_BACKEND_H

#include <stdbool.h>
#include <stdint.h>

#include <os/event/zlay_event.h>
#include <os/system/zlay_clipboard.h>
#include <os/window/zlay_cursor.h>
#include <os/window/zlay_monitor.h>
#include <os/window/zlay_window.h>

#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
ZLay_Window* ZLay_Win32_WindowAttachNative(const ZLay_WindowAttachDesc* desc);
void ZLay_Win32_WindowDetach(ZLay_Window* window);
void ZLay_Win32_WindowShow(ZLay_Window* window);
void ZLay_Win32_WindowHide(ZLay_Window* window);
void ZLay_Win32_WindowSetTitle(ZLay_Window* window, const char* title);
bool ZLay_Win32_WindowShouldClose(ZLay_Window* window);
void ZLay_Win32_WindowRequestClose(ZLay_Window* window);
void ZLay_Win32_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
void ZLay_Win32_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
float ZLay_Win32_WindowGetScale(ZLay_Window* window);
ZLay_NativeWindowHandle ZLay_Win32_WindowGetNativeHandle(ZLay_Window* window);
void ZLay_Win32_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor);
void ZLay_Win32_PollEvents(void);
bool ZLay_Win32_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event);
void ZLay_Win32_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event);
uint32_t ZLay_Win32_GetMonitorCount(void);
bool ZLay_Win32_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info);
bool ZLay_Win32_ClipboardSetText(const char* text);
const char* ZLay_Win32_ClipboardGetText(void);
#endif

#if defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
ZLay_Window* ZLay_Cocoa_WindowAttachNative(const ZLay_WindowAttachDesc* desc);
void ZLay_Cocoa_WindowDetach(ZLay_Window* window);
void ZLay_Cocoa_WindowShow(ZLay_Window* window);
void ZLay_Cocoa_WindowHide(ZLay_Window* window);
void ZLay_Cocoa_WindowSetTitle(ZLay_Window* window, const char* title);
bool ZLay_Cocoa_WindowShouldClose(ZLay_Window* window);
void ZLay_Cocoa_WindowRequestClose(ZLay_Window* window);
void ZLay_Cocoa_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
void ZLay_Cocoa_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
float ZLay_Cocoa_WindowGetScale(ZLay_Window* window);
ZLay_NativeWindowHandle ZLay_Cocoa_WindowGetNativeHandle(ZLay_Window* window);
void ZLay_Cocoa_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor);
void ZLay_Cocoa_PollEvents(void);
bool ZLay_Cocoa_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event);
void ZLay_Cocoa_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event);
uint32_t ZLay_Cocoa_GetMonitorCount(void);
bool ZLay_Cocoa_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info);
bool ZLay_Cocoa_ClipboardSetText(const char* text);
const char* ZLay_Cocoa_ClipboardGetText(void);
#endif

#if defined(ZLAY_OS_BACKEND_SELECTED_GTK)
#include <backends/platform/gtk/zlay_gtk_backend.h>
#endif

#if defined(ZLAY_OS_BACKEND_SELECTED_X11)
ZLay_Window* ZLay_X11_WindowAttachNative(const ZLay_WindowAttachDesc* desc);
void ZLay_X11_WindowDetach(ZLay_Window* window);
void ZLay_X11_WindowShow(ZLay_Window* window);
void ZLay_X11_WindowHide(ZLay_Window* window);
void ZLay_X11_WindowSetTitle(ZLay_Window* window, const char* title);
bool ZLay_X11_WindowShouldClose(ZLay_Window* window);
void ZLay_X11_WindowRequestClose(ZLay_Window* window);
void ZLay_X11_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
void ZLay_X11_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
float ZLay_X11_WindowGetScale(ZLay_Window* window);
ZLay_NativeWindowHandle ZLay_X11_WindowGetNativeHandle(ZLay_Window* window);
void ZLay_X11_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor);
void ZLay_X11_PollEvents(void);
bool ZLay_X11_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event);
void ZLay_X11_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event);
uint32_t ZLay_X11_GetMonitorCount(void);
bool ZLay_X11_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info);
bool ZLay_X11_ClipboardSetText(const char* text);
const char* ZLay_X11_ClipboardGetText(void);
#endif

#endif
