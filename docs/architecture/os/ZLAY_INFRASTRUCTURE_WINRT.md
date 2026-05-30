# ZLay Win32 + WinRT Infrastructure PRD

# Overview

ZLay Windows infrastructure menggunakan:

```txt
Win32
= core native backend

WinRT
= modern Windows enhancement layer
```

Win32 tetap menjadi pondasi utama untuk:

```txt
window
event loop
input
native handle
message processing
```

WinRT digunakan sebagai:

```txt
modern UI helper
composition
theme
DPI helper
notification
picker
Windows modern integration
```

---

# Core Philosophy

```txt
Win32 handles native infrastructure
WinRT enhances modern Windows experience
```

Bukan:

```txt
❌ mengganti Win32
❌ mengganti HWND
❌ mengganti event loop
```

Melainkan:

```txt
HWND
↓
WinRT enhancement
↓
Modern Windows experience
```

---

# Architecture Overview

```txt
Application
↓
ZLay UI
↓
ZLay Layout
↓
ZLay Renderer
↓
ZLay Driver
↓
ZLay Win32 Backend
↓
WinRT Modern Layer (optional)
↓
Windows OS
```

Detailed flow:

```txt
Win32
  window creation
  message loop
  input events
  native handle

WinRT
  composition
  mica/acrylic
  theme detection
  DPI helper
  modern integration
```

---

# Goals

## Primary Goals

```txt
✅ Native Win32 foundation
✅ Optional WinRT modern layer
✅ Stable C ABI
✅ Cross renderer compatibility
✅ Vulkan/OpenGL ready
✅ Modern Windows integration
✅ Low latency event handling
✅ DPI aware
✅ Mica/Acrylic support
✅ vcpkg friendly
```

---

# Non Goals

```txt
❌ UWP-only runtime
❌ XAML framework
❌ replacing Win32
❌ browser environment
❌ managed runtime dependency
```

---

# Folder Structure

```txt
src/os/
├── zlay_os.h
├── zlay_native.h
├── zlay_dpi.h
├── zlay_os_backend.h
│
├── win32/
│   ├── zlay_win32_backend.h
│   ├── zlay_win32_window.c
│   ├── zlay_win32_event.c
│   ├── zlay_win32_file.c
│   ├── zlay_win32_timer.c
│   ├── zlay_win32_native.c
│   ├── zlay_win32_clipboard.c
│   ├── zlay_win32_cursor.c
│   └── zlay_win32_monitor.c
│
├── winrt/
│   ├── zlay_winrt_config.h
│   ├── zlay_winrt_modern.h
│   ├── zlay_winrt_modern.cpp
│   │
│   ├── zlay_winrt_theme.h
│   ├── zlay_winrt_theme.cpp
│   │
│   ├── zlay_winrt_dpi.h
│   ├── zlay_winrt_dpi.cpp
│   │
│   ├── zlay_winrt_composition.h
│   ├── zlay_winrt_composition.cpp
│   │
│   ├── zlay_winrt_notification.h
│   ├── zlay_winrt_notification.cpp
│   │
│   ├── zlay_winrt_picker.h
│   ├── zlay_winrt_picker.cpp
│   │
│   └── internal/
│       ├── zlay_winrt_internal.hpp
│       └── zlay_winrt_internal.cpp
│
└── system/
    ├── zlay_timer.c
    ├── zlay_thread.c
    ├── zlay_path.c
    └── zlay_file.c
```

---

# Win32 Infrastructure

# Purpose

Win32 layer adalah backend utama Windows.

Bertugas untuk:

```txt
window creation
message loop
keyboard/mouse
native HWND
resize/focus
clipboard
monitor
cursor
timer
```

---

# Win32 Window Layer

## zlay_win32_window.c

Responsibilities:

```txt
CreateWindowEx
DestroyWindow
ShowWindow
SetWindowText
DPI awareness
fullscreen
window state
```

---

# Win32 Event Layer

## zlay_win32_event.c

Responsibilities:

```txt
WM_MOUSEMOVE
WM_LBUTTONDOWN
WM_KEYDOWN
WM_CHAR
WM_SIZE
WM_CLOSE
WM_DPICHANGED
```

Convert:

```txt
Win32 Messages
↓
ZLay_Event
```

---

# Win32 Native Layer

## zlay_win32_native.c

Expose:

```txt
HWND
HINSTANCE
HMONITOR
```

Used by:

```txt
OpenGL context
Vulkan surface
WinRT enhancement
```

---

# Win32 Clipboard Layer

## zlay_win32_clipboard.c

Responsibilities:

```txt
copy
paste
clipboard text
```

---

# Win32 Cursor Layer

## zlay_win32_cursor.c

Responsibilities:

```txt
arrow
hand
resize
text cursor
hidden cursor
```

---

# Win32 Monitor Layer

## zlay_win32_monitor.c

Responsibilities:

```txt
monitor enumeration
refresh rate
DPI scale
monitor bounds
```

---

# WinRT Infrastructure

# Purpose

WinRT layer bukan pengganti Win32.

WinRT digunakan untuk:

```txt
modern Windows integration
modern UI enhancement
composition helper
theme helper
DPI helper
notification
modern picker
```

---

# WinRT Core Philosophy

```txt
Win32 owns the window
WinRT enhances the experience
```

---

# WinRT Config Layer

## zlay_winrt_config.h

```c
#ifndef ZLAY_ENABLE_WINRT
#define ZLAY_ENABLE_WINRT 0
#endif

#ifndef ZLAY_ENABLE_WINRT_MODERN_UI
#define ZLAY_ENABLE_WINRT_MODERN_UI 0
#endif
```

---

# WinRT Public API

## zlay_winrt_modern.h

```c
#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ZLay_WinRTModernFlags {
    ZLAY_WINRT_MODERN_NONE        = 0,
    ZLAY_WINRT_MODERN_DPI         = 1 << 0,
    ZLAY_WINRT_MODERN_THEME       = 1 << 1,
    ZLAY_WINRT_MODERN_COMPOSITION = 1 << 2,
    ZLAY_WINRT_MODERN_ALL         = 0xFFFFFFFF
} ZLay_WinRTModernFlags;

typedef struct ZLay_WinRTModernDesc {
    void* native_window;

    uint32_t flags;

    bool enable_mica;
    bool enable_acrylic;
    bool enable_dark_mode;
} ZLay_WinRTModernDesc;

bool ZLay_WinRTModernEnable(
    const ZLay_WinRTModernDesc* desc
);

void ZLay_WinRTModernDisable(void);

bool ZLay_WinRTIsAvailable(void);

#ifdef __cplusplus
}
#endif
```

---

# WinRT Theme Layer

## zlay_winrt_theme.cpp

Responsibilities:

```txt
dark mode
light mode
accent color
system theme
```

WinRT namespaces:

```cpp
Windows.UI.ViewManagement
```

---

# WinRT DPI Layer

## zlay_winrt_dpi.cpp

Responsibilities:

```txt
DPI scale
logical pixel
monitor scale
high DPI helper
```

---

# WinRT Composition Layer

## zlay_winrt_composition.cpp

Responsibilities:

```txt
mica
acrylic
blur
composition helper
window backdrop
```

Future:

```txt
transform
opacity
layer composition
animation helper
```

---

# WinRT Notification Layer

## zlay_winrt_notification.cpp

Responsibilities:

```txt
toast notification
system notification
```

---

# WinRT Picker Layer

## zlay_winrt_picker.cpp

Responsibilities:

```txt
modern file picker
folder picker
save dialog
```

---

# Internal WinRT Include Layer

## Purpose

Centralize all WinRT includes.

Avoid:

```txt
public API contamination
compile bloat
cross-platform breakage
```

---

# zlay_winrt_internal.hpp

```cpp
#pragma once

#include <winrt/base.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.UI.Notifications.h>
```

---

# Important Rule

## NEVER include WinRT in public C headers

Correct:

```txt
zlay/
  pure C API and backend interface headers only

zlay/backends/platform/winrt/
  internal WinRT includes
```

Wrong:

```txt
❌ include/winrt/...
❌ exposing WinRT ABI publicly
❌ forcing WinRT dependency globally
```

---

# Runtime Flow

```txt
Win32 create HWND
↓
ZLay obtains HWND
↓
WinRT enhancement attaches to HWND
↓
Renderer uses HWND for Vulkan/OpenGL
↓
Modern UI features become available
```

---

# Example Runtime Initialization

## Window Creation

```c
ZLay_Window* window =
    ZLay_WindowCreate(&(ZLay_WindowDesc){
        .title = "Zenvra",
        .width = 1280,
        .height = 720,
        .resizable = true,
        .high_dpi = true
    });
```

---

## Native Handle

```c
ZLay_NativeWindowHandle native =
    ZLay_WindowGetNativeHandle(window);
```

---

## Enable Modern UI

```c
#if ZLAY_ENABLE_WINRT_MODERN_UI

ZLay_WinRTModernEnable(&(ZLay_WinRTModernDesc){
    .native_window = native.window,

    .flags =
        ZLAY_WINRT_MODERN_DPI |
        ZLAY_WINRT_MODERN_THEME |
        ZLAY_WINRT_MODERN_COMPOSITION,

    .enable_mica = true,
    .enable_acrylic = false,
    .enable_dark_mode = true
});

#endif
```

---

# Driver Integration

```txt
HWND
↓
Win32 backend
↓
OpenGL/WGL
or
Vulkan surface
↓
Renderer
```

WinRT layer tidak mengontrol:

```txt
❌ swapchain
❌ GPU pipeline
❌ renderer loop
❌ Vulkan device
❌ OpenGL context
```

WinRT hanya enhancement layer.

---

# Build System

# CMake Options

```cmake
option(ZLAY_OS_WIN32 "Enable Win32 backend" ON)

option(ZLAY_OS_WINRT "Enable WinRT enhancement layer" OFF)

option(ZLAY_WINRT_MODERN_UI
    "Enable WinRT modern UI features"
    OFF
)
```

---

# Win32 Build

```cmake
if(WIN32 AND ZLAY_OS_WIN32)

target_sources(zlay PRIVATE
    src/os/win32/zlay_win32_window.c
    src/os/win32/zlay_win32_event.c
    src/os/win32/zlay_win32_file.c
    src/os/win32/zlay_win32_timer.c
    src/os/win32/zlay_win32_native.c
)

endif()
```

---

# WinRT Build

```cmake
if(WIN32 AND ZLAY_OS_WINRT)

target_compile_definitions(zlay PRIVATE
    ZLAY_ENABLE_WINRT=1
)

target_sources(zlay PRIVATE
    src/os/winrt/zlay_winrt_modern.cpp
    src/os/winrt/zlay_winrt_theme.cpp
    src/os/winrt/zlay_winrt_dpi.cpp
    src/os/winrt/zlay_winrt_composition.cpp
)

target_compile_features(zlay PRIVATE cxx_std_17)

endif()
```

---

# Scalability Design

## Stable Public ABI

```txt
Pure C public API
```

---

## Internal WinRT Implementation

```txt
C++17 internal implementation
```

---

# Cross Platform Safety

Non-Windows platforms:

```txt
WinRT layer disabled automatically
```

Only:

```txt
Win32 backend compiled
```

---

# Recommended Development Order

## Phase 1

```txt
Win32 window
Win32 event loop
native HWND
OpenGL/Vulkan integration
```

---

## Phase 2

```txt
WinRT theme helper
dark mode
DPI helper
```

---

## Phase 3

```txt
Mica/Acrylic
composition helper
notification
picker
```

---

## Phase 4

```txt
advanced composition
animation layer
partial redraw optimization
```

---

# Final Architecture

```txt
os/
  platform abstraction

win32/
  native Windows backend

winrt/
  modern Windows enhancement

drivers/gpu/
  OpenGL/Vulkan abstraction

renderer/
  batching & rendering

layout/
  UI layout system

ui/
  interaction state

parser/
  optional DSL
```

---

# Final Design Philosophy

```txt
Win32 = infrastructure foundation
WinRT = modern enhancement layer

ZLay remains:
- native
- renderer agnostic
- low latency
- cross-platform
- pure C public ABI
```
