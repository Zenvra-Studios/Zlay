# ZLay (Zen Layout + Styling Library)

Native C/C++ layout & styling library concept untuk UI tools/engine editor berbasis OpenGL/Vulkan, dengan arah backend masa depan seperti WebGPU, Metal, Direct3D, software rasterizer, dan custom engine renderer.

ZLay tidak membawa CSS runtime, DOM scripting, WebKit, atau Chromium. Core-nya menghasilkan layout dan render commands; backend yang menerjemahkan command itu ke platform/renderer yang dipilih.

- Dokumentasi: `docs/README.md`
- Visi: `docs/vision.md`

## Build (CMakePresets)

```bash
cmake --preset debug
cmake --build --preset debug -j
./build/debug/zlay_example_basic
```

## Python Helper

```bash
python3 scripts/zlay.py
python3 scripts/zlay.py --preset release
python3 scripts/zlay.py --backend gtk
python3 scripts/zlay.py clean --backend gtk
```

## Include

- C API core: `#include "zlay.h"`
- Example/tutorial helper: `#include "zlay_example.h"`
- Renderer backends: `#include "driver/opengl/zlay_opengl.h"` atau `#include "driver/vulkan/zlay_vulkan.h"`
- Platform backends: `#include "os/zlay_os.h"`

## Source Layout

- `zlay.h`: public single entry untuk core C API.
- `zlay/`: implementasi core, components, renderer, driver, shader API, dan OS backend.
- `shader/`: source shader asli per bahasa yang di-embed saat build.
- `include/`: helper untuk example/tutorial, bukan tempat implementasi core.
