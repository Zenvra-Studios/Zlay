# ZLay Backends

ZLay mengikuti pemisahan ala backend ImGui:

- `zlay.h` adalah core layout/style/render-command murni C.
- `zlay/renderer/` adalah interface render backend-agnostic.
- `zlay/os/` adalah platform backend: window bridge, frame size, DPI, timer, style info.
- `zlay/os/win32/` adalah backend Windows. `zlay/os/winrt/` bukan backend kedua; folder itu hanya support modern Windows 10/11 di atas handle Win32 yang sudah ada.
- `zlay/driver/` adalah implementasi backend renderer: OpenGL/Vulkan translation dari render command ke data render.
- `include/` dipakai untuk helper tutorial/example, bukan implementasi core library.

ZLay tidak memasukkan CSS runtime, DOM scripting, JavaScript manipulation, WebKit, atau Chromium ke dalam core. Jika nanti WebGPU masuk, WebGPU diposisikan sebagai renderer backend, bukan sebagai alasan membawa model aplikasi web ke dalam library.

## Platform Backends

```text
zlay/os/
  zlay_os.h
  win32/zlay_win32_backend.h
  win32/zlay_win32_event.c
  win32/zlay_win32_file.c
  win32/zlay_win32_native.c
  win32/zlay_win32_timer.c
  win32/zlay_win32_window.c
  winrt/zlay_winrt_config.h
  winrt/zlay_winrt_composition.h
  winrt/zlay_winrt_composition.cpp
  winrt/zlay_winrt_dpi.h
  winrt/zlay_winrt_dpi.cpp
  winrt/zlay_winrt_modern.h
  winrt/zlay_winrt_modern.cpp
  winrt/zlay_winrt_notification.h
  winrt/zlay_winrt_notification.cpp
  winrt/zlay_winrt_picker.h
  winrt/zlay_winrt_picker.cpp
  winrt/zlay_winrt_theme.h
  winrt/zlay_winrt_theme.cpp
  winrt/internal/zlay_winrt_internal.hpp
  winrt/internal/zlay_winrt_internal.cpp
  cocoa/zlay_cocoa_backend.h
  cocoa/zlay_cocoa_platform.h
  cocoa/zlay_cocoa_window.mm
  cocoa/zlay_cocoa_event.mm
  cocoa/zlay_cocoa_timer.mm
  cocoa/zlay_cocoa_native.mm
  gtk/zlay_impl_gtk.h
  gtk/zlay_impl_gtk.c
  x11/zlay_x11_platform.h
  x11/zlay_x11_platform.c
```

## Renderer Backends

```text
zlay/renderer/
  zlay_renderer.h
  zlay_renderer.c
zlay/driver/
  opengl/zlay_opengl.h        # compatibility facade
  opengl/zlay_gl_driver.h
  opengl/zlay_gl_driver.c
  opengl/zlay_gl_context.c
  opengl/zlay_gl_pipeline.c
  opengl/zlay_gl_buffer.c
  opengl/zlay_gl_texture.c
  vulkan/zlay_vulkan.h        # compatibility facade
  vulkan/zlay_vk_driver.h
  vulkan/zlay_vk_driver.c
  vulkan/zlay_vk_instance.c
  vulkan/zlay_vk_device.c
  vulkan/zlay_vk_swapchain.c
  vulkan/zlay_vk_pipeline.c
  vulkan/zlay_vk_buffer.c
  vulkan/zlay_vk_command.c
  vulkan/zlay_vk_sync.c
```

Semua folder di bawah `zlay/` tetap diekspor sebagai include public lewat CMake, jadi consumer dapat memakai `#include <zlay.h>`, `#include <renderer/zlay_renderer.h>`, `#include <driver/opengl/zlay_opengl.h>`, dan `#include <os/zlay_os.h>` tanpa peduli lokasi fisik internalnya.

## CMake Selection

```bash
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DZLAY_OS_BACKEND=auto
cmake -S . -B build/debug-gtk -G Ninja -DCMAKE_BUILD_TYPE=Debug -DZLAY_OS_BACKEND=gtk
```

Available values:

```text
auto
x11
gtk
win32
cocoa
```

Future platform targets:

```text
wayland
posix unix
bsd family
solaris / illumos
android
ios
emscripten
```

## Style Info

Each platform backend exposes `ZLay_OS_GetStyleInfo()` so examples/tools can start with a native-ish visual baseline:

- GTK: Adwaita-like dark palette, soft radius, Adwaita/Cantarell font preference.
- Win32: Segoe UI baseline with conservative radius.
- WinRT support: optional Windows 10/11 helpers for system dark/light/high-contrast detection, dark title bar, Mica/Acrylic/blur backdrop, and tray icon manipulation on an existing Win32 window handle.
- Cocoa/UIKit: Apple system theme, DPI scale, clipboard, cursor/monitor helpers, and non-owning native handle support for `NSWindow`/`NSView`/`CAMetalLayer` or `UIWindow`/`UIView`/`CAMetalLayer`.
- X11: neutral dark editor palette for engine tools.

`zlay/os/winrt/zlay_winrt_modern.h` exposes the modern Windows helpers directly:

- `ZLay_WinRTModernEnable()`, `ZLay_WinRTModernDisable()`, and `ZLay_WinRTIsAvailable()` for the Win32 -> WinRT enhancement chain.
- `ZLay_WinRT_GetSystemThemeMode()` and `ZLay_WinRT_GetSystemStyleInfo()` for theme-aware layout/style defaults.
- `ZLay_WinRT_ApplyWindowSystemStyle()`, `ZLay_WinRT_SetWindowDarkMode()`, `ZLay_WinRT_SetWindowBackdrop()`, and `ZLay_WinRT_EnableWindowBlur()` for an app-owned `HWND`.
- `ZLay_WinRT_TrayAdd()`, `ZLay_WinRT_TrayUpdate()`, `ZLay_WinRT_TraySetTooltip()`, and `ZLay_WinRT_TrayRemove()` for tray manipulation.

## Future Renderer Targets

Renderer backend yang direncanakan secara arsitektural:

```text
OpenGL
Vulkan
WebGPU
Metal
Direct3D 11
Direct3D 12
software rasterizer
custom engine renderer
```

Semua renderer harus membaca data yang sama dari core: `ZLay_RenderCommandList`.
