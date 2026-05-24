#include <os/cocoa/zlay_cocoa_backend.h>
#import <os/cocoa/zlay_cocoa_platform.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char* zlay_cocoa_clipboard_text;
static char zlay_cocoa_monitor_names[16][64];

static char* ZLay_Cocoa_DuplicateText(const char* text) {
  if (text == NULL) {
    text = "";
  }

  const size_t length = strlen(text) + 1u;
  char* copy = (char*)malloc(length);
  if (copy == NULL) {
    return NULL;
  }

  memcpy(copy, text, length);
  return copy;
}

void ZLay_Cocoa_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor) {
  (void)window;

#if ZLAY_COCOA_TARGET_IOS
  (void)cursor;
#else
  if (cursor == ZLAY_CURSOR_HIDDEN) {
    [NSCursor hide];
    return;
  }

  [NSCursor unhide];
  switch (cursor) {
    case ZLAY_CURSOR_HAND:
      [[NSCursor pointingHandCursor] set];
      break;
    case ZLAY_CURSOR_TEXT:
      [[NSCursor IBeamCursor] set];
      break;
    case ZLAY_CURSOR_RESIZE_HORIZONTAL:
      [[NSCursor resizeLeftRightCursor] set];
      break;
    case ZLAY_CURSOR_RESIZE_VERTICAL:
      [[NSCursor resizeUpDownCursor] set];
      break;
    case ZLAY_CURSOR_CROSSHAIR:
      [[NSCursor crosshairCursor] set];
      break;
    case ZLAY_CURSOR_ARROW:
    default:
      [[NSCursor arrowCursor] set];
      break;
  }
#endif
}

uint32_t ZLay_Cocoa_GetMonitorCount(void) {
#if ZLAY_COCOA_TARGET_IOS
  return (uint32_t)[[UIScreen screens] count];
#else
  return (uint32_t)[[NSScreen screens] count];
#endif
}

bool ZLay_Cocoa_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info) {
  if (out_info == NULL) {
    return false;
  }

#if ZLAY_COCOA_TARGET_IOS
  NSArray<UIScreen*>* screens = [UIScreen screens];
  if (index >= screens.count) {
    return false;
  }

  UIScreen* screen = screens[index];
  const CGRect bounds = screen.bounds;
  out_info->x = (int32_t)bounds.origin.x;
  out_info->y = (int32_t)bounds.origin.y;
  out_info->width = (uint32_t)bounds.size.width;
  out_info->height = (uint32_t)bounds.size.height;
  out_info->scale_x = (float)screen.scale;
  out_info->scale_y = (float)screen.scale;
  out_info->refresh_rate = 60u;
  snprintf(zlay_cocoa_monitor_names[index % 16u], sizeof(zlay_cocoa_monitor_names[0]), "UIScreen %u", index);
  out_info->name = zlay_cocoa_monitor_names[index % 16u];
  return true;
#else
  NSArray<NSScreen*>* screens = [NSScreen screens];
  if (index >= screens.count) {
    return false;
  }

  NSScreen* screen = screens[index];
  const NSRect frame = screen.frame;
  const float scale = (float)screen.backingScaleFactor;
  out_info->x = (int32_t)frame.origin.x;
  out_info->y = (int32_t)frame.origin.y;
  out_info->width = (uint32_t)frame.size.width;
  out_info->height = (uint32_t)frame.size.height;
  out_info->scale_x = scale;
  out_info->scale_y = scale;
  out_info->refresh_rate = 60u;
  snprintf(zlay_cocoa_monitor_names[index % 16u], sizeof(zlay_cocoa_monitor_names[0]), "NSScreen %u", index);
  out_info->name = zlay_cocoa_monitor_names[index % 16u];
  return true;
#endif
}

bool ZLay_Cocoa_ClipboardSetText(const char* text) {
  char* copy = ZLay_Cocoa_DuplicateText(text);
  if (copy == NULL) {
    return false;
  }

#if ZLAY_COCOA_TARGET_IOS
  [UIPasteboard generalPasteboard].string = [NSString stringWithUTF8String:copy];
#else
  NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
  [pasteboard clearContents];
  [pasteboard setString:[NSString stringWithUTF8String:copy] forType:NSPasteboardTypeString];
#endif

  free(zlay_cocoa_clipboard_text);
  zlay_cocoa_clipboard_text = copy;
  return true;
}

const char* ZLay_Cocoa_ClipboardGetText(void) {
#if ZLAY_COCOA_TARGET_IOS
  NSString* string = [UIPasteboard generalPasteboard].string;
#else
  NSString* string = [[NSPasteboard generalPasteboard] stringForType:NSPasteboardTypeString];
#endif

  if (string == nil) {
    return zlay_cocoa_clipboard_text != NULL ? zlay_cocoa_clipboard_text : "";
  }

  const char* utf8 = string.UTF8String;
  if (utf8 == NULL) {
    return zlay_cocoa_clipboard_text != NULL ? zlay_cocoa_clipboard_text : "";
  }

  char* copy = ZLay_Cocoa_DuplicateText(utf8);
  if (copy == NULL) {
    return zlay_cocoa_clipboard_text != NULL ? zlay_cocoa_clipboard_text : "";
  }

  free(zlay_cocoa_clipboard_text);
  zlay_cocoa_clipboard_text = copy;
  return zlay_cocoa_clipboard_text;
}
