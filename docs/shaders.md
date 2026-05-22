# ZLay Shaders

Shader layer menyimpan source shader bawaan untuk renderer backend. Source shader tidak ditaruh hardcode sebagai string panjang di `antialiasing.c`; semua shader hidup sebagai file source asli per bahasa, lalu CMake menjalankan `scripts/embed_shaders.py` untuk membuat include generated saat build.

```text
ZLay_RenderCommandList
  -> renderer/zlay_renderer.h
  -> driver/opengl atau driver/vulkan
  -> zlay_antialiasing.h
```

## Built-In Sources

`zlay/zlay_antialiasing.h` menyediakan C API shader bawaan dan diekspor CMake sebagai include public, sehingga example dapat memakai:

```c
#include <zlay_antialiasing.h>
```

Header ini menyediakan:

- anti-aliasing helper untuk GLSL dan WGSL,
- anti-aliasing helper untuk HLSL dan Slang,
- rounded-rectangle SDF helper,
- rect vertex/fragment shader untuk GLSL 330,
- rect vertex/fragment shader untuk GLSL 450,
- rect vertex/fragment shader untuk GLSL ES 300 / WebGL 2,
- rect vertex/fragment shader untuk HLSL 5.0,
- rect vertex/fragment shader untuk Slang,
- rect vertex/fragment shader untuk WGSL.

OpenGL adapter memakai GLSL 330 sebagai default. Vulkan adapter memakai GLSL 450 sebagai default. WebGPU direction sudah disiapkan melalui WGSL source. WebGL direction disiapkan melalui GLSL ES 300.

## Source Layout

```text
shader/
  common/
    antialiasing.glsl
    antialiasing.hlsl
    antialiasing.slang
    antialiasing.wgsl
  glsl/
    rect.vert.glsl
    rect.frag.glsl
  vulkan/
    rect.vert.glsl
    rect.frag.glsl
  webgl/
    rect.vert.glsl
    rect.frag.glsl
  wgsl/
    rect.vert.wgsl
    rect.frag.wgsl
  hlsl/
    rect.vert.hlsl
    rect.frag.hlsl
  slang/
    rect.vert.slang
    rect.frag.slang
```

Generated output:

```text
build/<preset>/generated/zlay_shader_sources.inc
```

## Why Header + C Source

Shader disimpan sebagai C API supaya:

- bisa dipakai tanpa filesystem runtime,
- mudah dipaketkan seperti library kecil,
- backend tetap bisa memilih shader language sesuai renderer,
- core `zlay.h` tetap tidak tahu detail GLSL/WGSL,
- source C-nya tetap ada di `zlay/zlay_antialiasing.c`, sementara aset shader tetap hidup di folder `shader/`.

Saat backend sudah punya real GPU context, shader source ini bisa diteruskan ke compiler/pipeline masing-masing.
