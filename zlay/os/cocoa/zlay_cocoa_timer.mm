#include <os/cocoa/zlay_cocoa_backend.h>
#import <os/cocoa/zlay_cocoa_platform.h>

#include <mach/mach_time.h>

typedef struct ZLay_CocoaState {
  void* native_window;
  void* native_view;
  int32_t width;
  int32_t height;
} ZLay_CocoaState;

typedef struct ZLay_CocoaInitInfo {
  void* ns_window;
  void* ns_view;
  int32_t width;
  int32_t height;
} ZLay_CocoaInitInfo;

static ZLay_CocoaState zlay_cocoa_state;

uint64_t ZLay_Cocoa_NowNs(void) {
  static mach_timebase_info_data_t timebase = {0};
  if (timebase.denom == 0u) {
    mach_timebase_info(&timebase);
  }

  const uint64_t ticks = mach_absolute_time();
  return (ticks * (uint64_t)timebase.numer) / (uint64_t)timebase.denom;
}

bool ZLay_Cocoa_Init(const ZLay_CocoaInitInfo* info) {
  zlay_cocoa_state.native_window = info != NULL ? info->ns_window : NULL;
  zlay_cocoa_state.native_view = info != NULL ? info->ns_view : NULL;
  zlay_cocoa_state.width = info != NULL ? info->width : 0;
  zlay_cocoa_state.height = info != NULL ? info->height : 0;
  return true;
}

void ZLay_Cocoa_Shutdown(void) {
  zlay_cocoa_state = {};
}

void ZLay_Cocoa_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height) {
  zlay_cocoa_state.width = width;
  zlay_cocoa_state.height = height;
  if (ctx != NULL) {
    ZLay_Dimensions dimensions = {};
    dimensions.width = (float)width;
    dimensions.height = (float)height;
    ZLay_SetLayoutDimensions(ctx, dimensions);
  }
}

float ZLay_Cocoa_DpiScaleForNative(void* native_window, void* native_view, void* native_layer) {
  (void)native_layer;

#if ZLAY_COCOA_TARGET_IOS
  UIScreen* screen = nil;
  if (native_window != NULL) {
    screen = ((UIWindow*)native_window).screen;
  }
  if (screen == nil && native_view != NULL) {
    screen = ((UIView*)native_view).window.screen;
  }
  if (screen == nil) {
    screen = [UIScreen mainScreen];
  }
  return screen != nil ? (float)screen.scale : 1.0f;
#else
  NSWindow* window = (NSWindow*)native_window;
  NSView* view = (NSView*)native_view;
  if (window == nil && view != nil) {
    window = view.window;
  }
  if (window != nil) {
    return (float)window.backingScaleFactor;
  }
  if ([NSScreen mainScreen] != nil) {
    return (float)[NSScreen mainScreen].backingScaleFactor;
  }
  return 1.0f;
#endif
}

static ZLay_OSThemeMode ZLay_Cocoa_DetectTheme(void) {
#if ZLAY_COCOA_TARGET_IOS
  if (@available(iOS 13.0, *)) {
    UITraitCollection* traits = [UIScreen mainScreen].traitCollection;
    return traits.userInterfaceStyle == UIUserInterfaceStyleDark
      ? ZLAY_OS_THEME_DARK
      : ZLAY_OS_THEME_LIGHT;
  }
  return ZLAY_OS_THEME_LIGHT;
#else
  if (@available(macOS 10.14, *)) {
    NSAppearance* appearance = NSApp.effectiveAppearance;
    if (appearance == nil) {
      appearance = NSAppearance.currentAppearance;
    }
    NSAppearanceName best = [appearance bestMatchFromAppearancesWithNames:@[
      NSAppearanceNameAqua,
      NSAppearanceNameDarkAqua,
      NSAppearanceNameAccessibilityHighContrastAqua,
      NSAppearanceNameAccessibilityHighContrastDarkAqua
    ]];
    if ([best isEqualToString:NSAppearanceNameAccessibilityHighContrastDarkAqua] ||
        [best isEqualToString:NSAppearanceNameAccessibilityHighContrastAqua]) {
      return ZLAY_OS_THEME_HIGH_CONTRAST;
    }
    if ([best isEqualToString:NSAppearanceNameDarkAqua]) {
      return ZLAY_OS_THEME_DARK;
    }
  }
  return ZLAY_OS_THEME_LIGHT;
#endif
}

ZLay_OSStyleInfo ZLay_Cocoa_GetStyleInfo(void) {
  const ZLay_OSThemeMode mode = ZLay_Cocoa_DetectTheme();

  if (mode == ZLAY_OS_THEME_LIGHT) {
    ZLay_OSStyleInfo info = {};
    info.mode = ZLAY_OS_THEME_LIGHT;
    info.window_bg = {0xF7, 0xF7, 0xF8, 0xFF};
    info.panel_bg = {0xEC, 0xEC, 0xEF, 0xF4};
    info.text = {0x1D, 0x1D, 0x1F, 0xFF};
    info.accent = {0x00, 0x7A, 0xFF, 0xFF};
    info.corner_radius = 10.0f;
    info.control_height = 30.0f;
    info.font_family = ".AppleSystemUIFont";
    return info;
  }

  if (mode == ZLAY_OS_THEME_HIGH_CONTRAST) {
    ZLay_OSStyleInfo info = {};
    info.mode = ZLAY_OS_THEME_HIGH_CONTRAST;
    info.window_bg = {0x00, 0x00, 0x00, 0xFF};
    info.panel_bg = {0x00, 0x00, 0x00, 0xFF};
    info.text = {0xFF, 0xFF, 0xFF, 0xFF};
    info.accent = {0x00, 0x7A, 0xFF, 0xFF};
    info.corner_radius = 4.0f;
    info.control_height = 32.0f;
    info.font_family = ".AppleSystemUIFont";
    return info;
  }

  ZLay_OSStyleInfo info = {};
  info.mode = ZLAY_OS_THEME_DARK;
  info.window_bg = {0x1C, 0x1C, 0x1E, 0xFF};
  info.panel_bg = {0x2C, 0x2C, 0x2E, 0xF2};
  info.text = {0xF5, 0xF5, 0xF7, 0xFF};
  info.accent = {0x0A, 0x84, 0xFF, 0xFF};
  info.corner_radius = 10.0f;
  info.control_height = 30.0f;
  info.font_family = ".AppleSystemUIFont";
  return info;
}

const char* ZLay_OS_BackendName(void) {
  return ZLAY_COCOA_TARGET_IOS ? "UIKit" : "Cocoa";
}

ZLay_OSBackendType ZLay_OS_BackendType(void) {
  return ZLAY_COCOA_TARGET_IOS ? ZLAY_OS_BACKEND_IOS : ZLAY_OS_BACKEND_COCOA;
}

uint32_t ZLay_OS_Capabilities(void) {
  return ZLAY_OS_CAP_MONOTONIC_TIME |
         ZLAY_OS_CAP_DPI_SCALE |
         ZLAY_OS_CAP_NATIVE_THEME |
         ZLAY_OS_CAP_CLIPBOARD |
         ZLAY_OS_CAP_MOUSE_CURSOR |
         ZLAY_OS_CAP_TEXT_INPUT;
}

uint64_t ZLay_OS_NowNs(void) {
  return ZLay_Cocoa_NowNs();
}

float ZLay_OS_GetDpiScale(void* native_window) {
  return ZLay_Cocoa_DpiScaleForNative(
    native_window != NULL ? native_window : zlay_cocoa_state.native_window,
    zlay_cocoa_state.native_view,
    NULL);
}

ZLay_OSStyleInfo ZLay_OS_GetStyleInfo(void) {
  return ZLay_Cocoa_GetStyleInfo();
}

void ZLay_OS_NewFrame(ZLay_Context* ctx, const ZLay_OSFrameInfo* frame) {
  if (frame == NULL) {
    return;
  }
  ZLay_Cocoa_NewFrame(ctx, frame->width, frame->height);
}
