# ZLay Renderer Layer

ZLay renderer layer berada di antara core render commands dan backend driver.

```text
zlay.h render commands
  -> renderer/zlay_renderer.h
  -> driver/opengl atau driver/vulkan
```

## Core Renderer Interface

`zlay/renderer/zlay_renderer.h` menyediakan interface renderer dan tetap bisa di-include sebagai:

```c
#include <renderer/zlay_renderer.h>
```

Header ini menyediakan:

- `ZLay_Renderer`: backend-agnostic renderer handle.
- `ZLay_RendererFrameInfo`: ukuran frame, DPI, clear color.
- `ZLay_RendererStats`: command count, draw call, vertex/index count.
- `ZLay_RendererBeginFrame()`, `ZLay_RendererRenderCommands()`, `ZLay_RendererEndFrame()`, `ZLay_RendererShutdown()`.

## Driver Adapters

OpenGL:

```c
ZLay_OpenGLRenderer gl_backend;
ZLay_OpenGLRenderer_Init(&gl_backend, &info);
ZLay_Renderer renderer = ZLay_OpenGLRenderer_AsRenderer(&gl_backend);
```

Vulkan:

```c
ZLay_VulkanRenderer vk_backend;
ZLay_VulkanRenderer_Init(&vk_backend, &info);
ZLay_Renderer renderer = ZLay_VulkanRenderer_AsRenderer(&vk_backend);
```

Saat ini OpenGL adapter membangun indexed geometry dari `ZLay_RenderCommandList`, sementara Vulkan adapter membangun batch metadata. Tahap berikutnya adalah menghubungkan adapter ini ke upload buffer, shader, pipeline, dan command submission nyata.

Shader source bawaan dijelaskan di `docs/shaders.md`.

## WebGPU Direction

WebGPU nantinya dapat masuk sebagai backend renderer:

```text
ZLay_RenderCommandList -> WebGPU vertex/index buffers -> WebGPU render pass
```

Core ZLay tetap tidak perlu mengetahui CSS, DOM, JavaScript, WebKit, atau browser engine. WebGPU diperlakukan seperti Vulkan/OpenGL: target grafis untuk menggambar hasil layout.
