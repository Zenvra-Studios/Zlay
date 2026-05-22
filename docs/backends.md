# ZLay Backends

ZLay mengikuti pemisahan ala backend ImGui:

- `zlay.h` adalah core layout/style/render-command murni C.
- `zlay/renderer/` adalah interface render backend-agnostic.
- `zlay/os/` adalah platform backend: window bridge, frame size, DPI, timer, style info.
- `zlay/driver/` adalah implementasi backend renderer: OpenGL/Vulkan translation dari render command ke data render.
- `include/` dipakai untuk helper tutorial/example, bukan implementasi core library.

ZLay tidak memasukkan CSS runtime, DOM scripting, JavaScript manipulation, WebKit, atau Chromium ke dalam core. Jika nanti WebGPU masuk, WebGPU diposisikan sebagai renderer backend, bukan sebagai alasan membawa model aplikasi web ke dalam library.

## Platform Backends

```text
zlay/os/
  zlay_os.h
  win32/zlay_impl_win32.h
  win32/zlay_impl_win32.c
  winrt/zlay_impl_winrt.h
  winrt/zlay_impl_winrt.c
  cocoa/zlay_impl_cocoa.h
  cocoa/zlay_impl_cocoa.c
  gtk/zlay_impl_gtk.h
  gtk/zlay_impl_gtk.c
  libx11/zlay_impl_x11.h
  libx11/zlay_impl_x11.c
```

## Renderer Backends

```text
zlay/renderer/
  zlay_renderer.h
  zlay_renderer.c
zlay/driver/
  opengl/zlay_opengl.h
  opengl/zlay_opengl.c
  vulkan/zlay_vulkan.h
  vulkan/zlay_vulkan.c
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
winrt
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
- WinRT: Fluent/Mica-like dark palette, Segoe UI Variable preference.
- Win32: Segoe UI baseline with conservative radius.
- Cocoa: SF Pro baseline with macOS-like accent/radius.
- X11: neutral dark editor palette for engine tools.

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
