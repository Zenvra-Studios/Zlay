# ZLay OS Infrastructure PRD

## Overview

ZLay OS Infrastructure adalah platform abstraction layer untuk menangani seluruh komunikasi antara engine dan native operating system.

Layer ini menjadi pondasi untuk:

- Window management
- Event system
- Input handling
- Timer & frame pacing
- Native OS handle
- Clipboard
- File/path abstraction
- Threading
- DPI & monitor scaling
- Cursor handling

OS layer harus independen dari:

```txt
❌ layout
❌ styling
❌ widget system
❌ parser
❌ renderer logic
❌ Vulkan/OpenGL pipeline
```

---

# Architecture Position

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
ZLay OS
↓
Native OS
```

Detailed flow:

```txt
os/
  create window
  poll native events
  provide native handles

drivers/gpu/
  create OpenGL/Vulkan context
  create swapchain/surface

ui/
  process hover/focus/input state

layout/
  calculate positions/sizes

renderer/
  build render command list
```

---

# Goals

## Primary Goals

```txt
✅ Cross-platform abstraction
✅ Stable C ABI
✅ Low latency event handling
✅ Native handle interoperability
✅ High DPI support
✅ Multi-window support
✅ Renderer agnostic
✅ Static/shared library friendly
✅ vcpkg friendly
```

---

# Non Goals

```txt
❌ UI toolkit
❌ Rendering backend
❌ Widget framework
❌ Layout engine
❌ HTML/CSS runtime
❌ Browser environment
```

---

# Folder Structure

```txt
src/os/
├── zlay_os.h
├── zlay_platform.h
├── zlay_native.h
--- zlay_dpi.h
│
├── window/
│   ├── zlay_window.h
│   ├── zlay_window.c
│   ├── zlay_monitor.h
│   ├── zlay_monitor.c
│   ├── zlay_cursor.h
│   └── zlay_cursor.c
│
├── event/
│   ├── zlay_event.h
│   ├── zlay_event.c
│   ├── zlay_input.h
│   ├── zlay_input.c
│   ├── zlay_keycode.h
│   └── zlay_mouse.h
│
├── system/
│   ├── zlay_timer.h
│   ├── zlay_timer.c
│   ├── zlay_file.h
│   ├── zlay_file.c
│   ├── zlay_path.h
│   ├── zlay_path.c
│   ├── zlay_clipboard.h
│   ├── zlay_clipboard.c
│   ├── zlay_thread.h
│   ├── zlay_thread.c
│   ├── zlay_mutex.h
│   └── zlay_mutex.c
│
├── win32/
│   ├── zlay_win32_window.c
│   ├── zlay_win32_event.c
│   ├── zlay_win32_timer.c
│   ├── zlay_win32_file.c
│   └── zlay_win32_native.c
│
├── x11/
│   ├── zlay_x11_window.c
│   ├── zlay_x11_event.c
│   ├── zlay_x11_timer.c
│   ├── zlay_x11_clipboard.c
│   └── zlay_x11_native.c
│
├── wayland/
│   ├── zlay_wayland_window.c
│   ├── zlay_wayland_event.c
│   ├── zlay_wayland_timer.c
│   └── zlay_wayland_native.c
│
├── cocoa/
│   ├── zlay_cocoa_window.mm
│   ├── zlay_cocoa_event.mm
│   └── zlay_cocoa_native.mm
│
├── android/
│   ├── zlay_android_window.c
│   ├── zlay_android_event.c
│   └── zlay_android_native.c
│
└── emscripten/
    ├── zlay_emscripten_window.c
    ├── zlay_emscripten_event.c
    └── zlay_emscripten_native.c
```

---

# Minimal Initial Implementation

```txt
src/os/
├── zlay_platform.h
├── zlay_native.h
├── window/
│   └── zlay_window.h
├── event/
│   ├── zlay_event.h
│   └── zlay_keycode.h
├── system/
│   └── zlay_timer.h
└── x11/
    ├── zlay_x11_window.c
    ├── zlay_x11_event.c
    └── zlay_x11_native.c
```

---

# Platform Layer

## zlay_platform.h

```c
typedef enum ZLay_Platform {
    ZLAY_PLATFORM_UNKNOWN = 0,
    ZLAY_PLATFORM_WIN32,
    ZLAY_PLATFORM_X11,
    ZLAY_PLATFORM_WAYLAND,
    ZLAY_PLATFORM_COCOA,
    ZLAY_PLATFORM_ANDROID,
    ZLAY_PLATFORM_EMSCRIPTEN
} ZLay_Platform;
```

---

# Native Handle Layer

## zlay_native.h

```c
typedef struct ZLay_NativeWindowHandle {
    ZLay_Platform platform;

    void* display;
    void* window;
    void* surface;
    void* connection;

    uintptr_t raw0;
    uintptr_t raw1;
    uintptr_t raw2;
} ZLay_NativeWindowHandle;
```

---

## Platform Mapping

### Win32

```txt
display = HINSTANCE
window  = HWND
```

### X11

```txt
display = Display*
window  = Window
```

### Wayland

```txt
display = wl_display*
surface = wl_surface*
```

### Cocoa

```txt
window  = NSWindow*
surface = CAMetalLayer*
```

### Android

```txt
window = ANativeWindow*
```

---

# Window Layer

## zlay_window.h

```c
typedef struct ZLay_Window ZLay_Window;
```

---

## Window Description

```c
typedef struct ZLay_WindowDesc {
    const char* title;

    uint32_t width;
    uint32_t height;

    int32_t x;
    int32_t y;

    bool resizable;
    bool decorated;
    bool visible;
    bool high_dpi;

    bool fullscreen;
    bool maximized;
    bool minimized;

    void* user_data;
} ZLay_WindowDesc;
```

---

## Window API

```c
ZLAY_API ZLay_Window* ZLay_WindowCreate(
    const ZLay_WindowDesc* desc
);

ZLAY_API void ZLay_WindowDestroy(
    ZLay_Window* window
);

ZLAY_API void ZLay_WindowShow(
    ZLay_Window* window
);

ZLAY_API void ZLay_WindowHide(
    ZLay_Window* window
);

ZLAY_API void ZLay_WindowSetTitle(
    ZLay_Window* window,
    const char* title
);

ZLAY_API bool ZLay_WindowShouldClose(
    ZLay_Window* window
);

ZLAY_API void ZLay_WindowRequestClose(
    ZLay_Window* window
);

ZLAY_API void ZLay_WindowGetSize(
    ZLay_Window* window,
    uint32_t* width,
    uint32_t* height
);

ZLAY_API void ZLay_WindowGetFramebufferSize(
    ZLay_Window* window,
    uint32_t* width,
    uint32_t* height
);

ZLAY_API float ZLay_WindowGetScale(
    ZLay_Window* window
);

ZLAY_API ZLay_NativeWindowHandle ZLay_WindowGetNativeHandle(
    ZLay_Window* window
);
```

---

# Event System

## Event Flow

```txt
Native OS Event
↓
ZLay_Event
↓
Input System
↓
UI System
```

---

## zlay_event.h

```c
typedef enum ZLay_EventType {
    ZLAY_EVENT_NONE = 0,

    ZLAY_EVENT_WINDOW_CLOSE,
    ZLAY_EVENT_WINDOW_RESIZE,
    ZLAY_EVENT_WINDOW_MOVE,
    ZLAY_EVENT_WINDOW_FOCUS,
    ZLAY_EVENT_WINDOW_BLUR,
    ZLAY_EVENT_WINDOW_DPI_CHANGED,

    ZLAY_EVENT_MOUSE_MOVE,
    ZLAY_EVENT_MOUSE_BUTTON_DOWN,
    ZLAY_EVENT_MOUSE_BUTTON_UP,
    ZLAY_EVENT_MOUSE_WHEEL,
    ZLAY_EVENT_MOUSE_ENTER,
    ZLAY_EVENT_MOUSE_LEAVE,

    ZLAY_EVENT_KEY_DOWN,
    ZLAY_EVENT_KEY_UP,
    ZLAY_EVENT_TEXT_INPUT,

    ZLAY_EVENT_TOUCH_DOWN,
    ZLAY_EVENT_TOUCH_MOVE,
    ZLAY_EVENT_TOUCH_UP,

    ZLAY_EVENT_DROP_FILE
} ZLay_EventType;
```

---

## Event Structure

```c
typedef struct ZLay_Event {
    ZLay_EventType type;
    double timestamp;

    union {
        struct {
            uint32_t width;
            uint32_t height;
        } resize;

        struct {
            int32_t x;
            int32_t y;
        } move;

        struct {
            float scale;
        } dpi;

        struct {
            float x;
            float y;
            float dx;
            float dy;
        } mouse_move;

        struct {
            int button;
            float x;
            float y;
        } mouse_button;

        struct {
            float x;
            float y;
        } wheel;

        struct {
            int key;
            int scancode;
            bool repeat;
            uint32_t modifiers;
        } key;

        struct {
            uint32_t codepoint;
        } text;

        struct {
            uint64_t id;
            float x;
            float y;
        } touch;

        struct {
            const char* path;
        } drop_file;
    };
} ZLay_Event;
```

---

# Event Queue API

```c
ZLAY_API void ZLay_PollEvents(void);

ZLAY_API bool ZLay_WindowNextEvent(
    ZLay_Window* window,
    ZLay_Event* out_event
);

ZLAY_API void ZLay_WindowPushEvent(
    ZLay_Window* window,
    const ZLay_Event* event
);
```

---

# Input State Layer

## Purpose

```txt
event queue = raw events
input state = current frame snapshot
```

---

## zlay_input.h

```c
typedef struct ZLay_InputState {
    float mouse_x;
    float mouse_y;

    float mouse_dx;
    float mouse_dy;

    float wheel_x;
    float wheel_y;

    bool mouse_down[8];
    bool mouse_pressed[8];
    bool mouse_released[8];

    bool key_down[512];
    bool key_pressed[512];
    bool key_released[512];

    uint32_t modifiers;

    uint32_t text_input_count;
    uint32_t text_input[32];
} ZLay_InputState;
```

---

## Input API

```c
ZLAY_API void ZLay_InputBeginFrame(
    ZLay_InputState* input
);

ZLAY_API void ZLay_InputProcessEvent(
    ZLay_InputState* input,
    const ZLay_Event* event
);

ZLAY_API void ZLay_InputEndFrame(
    ZLay_InputState* input
);
```

---

# Keycode Abstraction

## zlay_keycode.h

```c
typedef enum ZLay_Key {
    ZLAY_KEY_UNKNOWN = 0,

    ZLAY_KEY_A,
    ZLAY_KEY_B,
    ZLAY_KEY_C,

    ZLAY_KEY_0,
    ZLAY_KEY_1,

    ZLAY_KEY_ESCAPE,
    ZLAY_KEY_ENTER,
    ZLAY_KEY_TAB,
    ZLAY_KEY_BACKSPACE,
    ZLAY_KEY_SPACE,

    ZLAY_KEY_LEFT,
    ZLAY_KEY_RIGHT,
    ZLAY_KEY_UP,
    ZLAY_KEY_DOWN,

    ZLAY_KEY_SHIFT,
    ZLAY_KEY_CONTROL,
    ZLAY_KEY_ALT,
    ZLAY_KEY_SUPER,

    ZLAY_KEY_F1,
    ZLAY_KEY_F2,
    ZLAY_KEY_F3
} ZLay_Key;
```

---

# Timer System

## Purpose

```txt
delta time
frame pacing
profiling
animation
double click
hold input
```

---

## zlay_timer.h

```c
typedef struct ZLay_FrameTimer {
    double last_time;
    double now;
    double delta;
    double elapsed;

    uint64_t frame_index;
} ZLay_FrameTimer;
```

---

## Timer API

```c
ZLAY_API double ZLay_TimeNow(void);

ZLAY_API void ZLay_SleepMilliseconds(
    uint32_t ms
);

ZLAY_API void ZLay_FrameTimerInit(
    ZLay_FrameTimer* timer
);

ZLAY_API void ZLay_FrameTimerTick(
    ZLay_FrameTimer* timer
);
```

---

# Monitor & DPI

## zlay_monitor.h

```c
typedef struct ZLay_MonitorInfo {
    int32_t x;
    int32_t y;

    uint32_t width;
    uint32_t height;

    float scale_x;
    float scale_y;

    uint32_t refresh_rate;

    const char* name;
} ZLay_MonitorInfo;
```

---

## Monitor API

```c
ZLAY_API uint32_t ZLay_GetMonitorCount(void);

ZLAY_API bool ZLay_GetMonitorInfo(
    uint32_t index,
    ZLay_MonitorInfo* out_info
);
```

---

# Clipboard

## zlay_clipboard.h

```c
ZLAY_API bool ZLay_ClipboardSetText(
    const char* text
);

ZLAY_API const char* ZLay_ClipboardGetText(void);
```

---

# File & Path

## zlay_file.h

```c
typedef struct ZLay_FileData {
    void* data;
    size_t size;
} ZLay_FileData;
```

---

## File API

```c
ZLAY_API bool ZLay_ReadFile(
    const char* path,
    ZLay_FileData* out_file
);

ZLAY_API bool ZLay_WriteFile(
    const char* path,
    const void* data,
    size_t size
);

ZLAY_API void ZLay_FreeFileData(
    ZLay_FileData* file
);
```

---

## Path API

```c
ZLAY_API const char* ZLay_GetExecutablePath(void);
ZLAY_API const char* ZLay_GetConfigPath(void);
ZLAY_API const char* ZLay_GetCachePath(void);
ZLAY_API const char* ZLay_GetTempPath(void);
```

---

# Threading

## zlay_thread.h

```c
typedef struct ZLay_Thread ZLay_Thread;

typedef int (*ZLay_ThreadFn)(void* user);
```

---

## Thread API

```c
ZLAY_API ZLay_Thread* ZLay_ThreadCreate(
    ZLay_ThreadFn fn,
    void* user
);

ZLAY_API void ZLay_ThreadJoin(
    ZLay_Thread* thread
);

ZLAY_API void ZLay_ThreadDestroy(
    ZLay_Thread* thread
);
```

---

# Mutex

## zlay_mutex.h

```c
typedef struct ZLay_Mutex ZLay_Mutex;
```

---

## Mutex API

```c
ZLAY_API ZLay_Mutex* ZLay_MutexCreate(void);

ZLAY_API void ZLay_MutexDestroy(
    ZLay_Mutex* mutex
);

ZLAY_API void ZLay_MutexLock(
    ZLay_Mutex* mutex
);

ZLAY_API void ZLay_MutexUnlock(
    ZLay_Mutex* mutex
);
```

---

# Cursor Layer

## zlay_cursor.h

```c
typedef enum ZLay_Cursor {
    ZLAY_CURSOR_ARROW,
    ZLAY_CURSOR_HAND,
    ZLAY_CURSOR_TEXT,
    ZLAY_CURSOR_RESIZE_HORIZONTAL,
    ZLAY_CURSOR_RESIZE_VERTICAL,
    ZLAY_CURSOR_CROSSHAIR,
    ZLAY_CURSOR_HIDDEN
} ZLay_Cursor;
```

---

## Cursor API

```c
ZLAY_API void ZLay_WindowSetCursor(
    ZLay_Window* window,
    ZLay_Cursor cursor
);
```

---

# Driver Integration

## OS → Driver Flow

```txt
Window Create
↓
Native Handle
↓
Driver Create
↓
Frame Loop
↓
Poll Events
↓
Process Input
↓
Build Layout
↓
Submit Render Commands
↓
Present
```

---

# Example Runtime Flow

```c
ZLay_Window* window = ZLay_WindowCreate(&(ZLay_WindowDesc){
    .title = "Zenvra",
    .width = 1280,
    .height = 720,
    .resizable = true,
    .high_dpi = true,
    .visible = true
});

ZLay_NativeWindowHandle native =
    ZLay_WindowGetNativeHandle(window);
```

---

## Driver Creation

```c
ZLay_Driver* driver = NULL;

ZLay_DriverCreate(&driver, &(ZLay_DriverDesc){
    .backend = ZLAY_BACKEND_VULKAN,
    .native = native,
    .width = 1280,
    .height = 720,
    .vsync = true,
    .low_latency = true,
    .frames_in_flight = 2
});
```

---

# Main Loop

```c
ZLay_InputState input;
ZLay_FrameTimer timer;

ZLay_FrameTimerInit(&timer);

while (!ZLay_WindowShouldClose(window)) {
    ZLay_FrameTimerTick(&timer);

    ZLay_InputBeginFrame(&input);

    ZLay_PollEvents();

    ZLay_Event event;

    while (ZLay_WindowNextEvent(window, &event)) {
        ZLay_InputProcessEvent(&input, &event);

        if (event.type == ZLAY_EVENT_WINDOW_RESIZE) {
            ZLay_DriverResize(
                driver,
                event.resize.width,
                event.resize.height
            );
        }
    }

    ZLay_BeginLayout(ctx);

    build_ui(ctx, &input, timer.delta);

    ZLay_RenderCommandList cmds =
        ZLay_EndLayout(ctx);

    ZLay_DriverBeginFrame(driver);
    ZLay_DriverSubmit(driver, &cmds);
    ZLay_DriverEndFrame(driver);
    ZLay_DriverPresent(driver);

    ZLay_InputEndFrame(&input);
}
```

---

# Platform Backend Contract

Every OS backend must implement:

```txt
create window
destroy window
show/hide
set title
get size
get framebuffer size
get dpi scale
poll events
next event
native handle
clipboard
timer
cursor
```

---

# X11 Backend

```txt
x11/
├── zlay_x11_window.c
├── zlay_x11_event.c
├── zlay_x11_clipboard.c
├── zlay_x11_timer.c
└── zlay_x11_native.c
```

---

## Required Handles

```txt
Display*
Window
Atom WM_DELETE_WINDOW
```

---

## Event Mapping

```txt
MotionNotify     -> MOUSE_MOVE
ButtonPress      -> MOUSE_BUTTON_DOWN / WHEEL
ButtonRelease    -> MOUSE_BUTTON_UP
KeyPress         -> KEY_DOWN / TEXT_INPUT
KeyRelease       -> KEY_UP
ConfigureNotify  -> WINDOW_RESIZE
ClientMessage    -> WINDOW_CLOSE
FocusIn          -> WINDOW_FOCUS
FocusOut         -> WINDOW_BLUR
```

---

# Win32 Backend

```txt
win32/
├── zlay_win32_window.c
├── zlay_win32_event.c
├── zlay_win32_clipboard.c
├── zlay_win32_timer.c
└── zlay_win32_native.c
```

---

## Required Handles

```txt
HWND
HINSTANCE
```

---

## Message Mapping

```txt
WM_CLOSE        -> WINDOW_CLOSE
WM_SIZE         -> WINDOW_RESIZE
WM_MOUSEMOVE    -> MOUSE_MOVE
WM_LBUTTONDOWN  -> MOUSE_BUTTON_DOWN
WM_LBUTTONUP    -> MOUSE_BUTTON_UP
WM_MOUSEWHEEL   -> MOUSE_WHEEL
WM_KEYDOWN      -> KEY_DOWN
WM_KEYUP        -> KEY_UP
WM_CHAR         -> TEXT_INPUT
WM_DPICHANGED   -> WINDOW_DPI_CHANGED
```

---

# Wayland Backend

## Required Handles

```txt
wl_display*
wl_surface*
xdg_surface*
xdg_toplevel*
```

---

## Recommendation

```txt
Implement after X11 and Win32 are stable.
```

---

# Scalability Rules

## Stable Public API

```txt
zlay/os/zlay_os.h
zlay/os/window/zlay_window.h
zlay/os/event/zlay_event.h
```

---

## Private Backend Implementation

```txt
src/os/win32/
src/os/x11/
src/os/wayland/
```

User applications must never include:

```txt
zlay_x11_window.h
zlay_win32_window.h
```

---

# Build System

## CMake Options

```cmake
option(ZLAY_ENABLE_OS "Enable OS abstraction layer" ON)

option(ZLAY_OS_WIN32 "Enable Win32 backend" ON)
option(ZLAY_OS_X11 "Enable X11 backend" ON)
option(ZLAY_OS_WAYLAND "Enable Wayland backend" OFF)
option(ZLAY_OS_COCOA "Enable Cocoa backend" OFF)
option(ZLAY_OS_ANDROID "Enable Android backend" OFF)
option(ZLAY_OS_EMSCRIPTEN "Enable Emscripten backend" OFF)
```

---

## Platform Auto Detection

```cmake
if(WIN32)
    target_sources(zlay PRIVATE
        src/os/win32/zlay_win32_window.c
    )
elseif(UNIX AND NOT APPLE)
    target_sources(zlay PRIVATE
        src/os/x11/zlay_x11_window.c
    )
elseif(APPLE)
    target_sources(zlay PRIVATE
        src/os/cocoa/zlay_cocoa_window.mm
    )
endif()
```

---

# Recommended Implementation Order

## Phase 1

```txt
zlay_window.h
zlay_event.h
zlay_input.h
zlay_timer.h
X11 backend
```

---

## Phase 2

```txt
Win32 backend
clipboard
cursor
dpi
monitor
```

---

## Phase 3

```txt
file/path
thread/mutex
drag & drop
high DPI polish
```

---

## Phase 4

```txt
Wayland
Cocoa
Android
Emscripten
```

---

# Final Design Philosophy

```txt
os/       = platform abstraction
drivers/gpu/ = GPU abstraction
renderer/ = command batching
layout/   = size & position
style/    = visual properties
ui/       = interaction state
parser/   = optional DSL
```

---

# Final Product Targets

```txt
1. Standalone native UI library
2. Thirdparty dependency for Zenvra Engine
3. Static library
4. Shared library
5. vcpkg package
```

Berdasarkan struktur dan konsep yang sudah kamu tulis sebelumnya. :contentReference[oaicite:0]{index=0}
