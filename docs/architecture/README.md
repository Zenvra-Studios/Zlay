# Zlay Backend, Renderer, Driver, and Host Integration Architecture

Zlay is an experimental native C/C++ GUI/layout framework for engine editors, creative tools, renderer-driven applications, IDE tooling, and DAW-style interfaces.

This document defines how Zlay organizes:

- layout backends
- renderer backends
- GPU drivers
- platform backends
- host integrations
- third-party dependencies
- renderer abstraction
- backend implementation rules

The goal is to keep Zlay flexible without making the architecture ambiguous.

---

## 1. Core Design Rule

Zlay separates **backend** and **driver** clearly.

```txt
backend = adapter from Zlay abstraction to a specific implementation
driver  = low-level wrapper around GPU/API/system calls

In short:

Backend speaks Zlay.
Driver speaks OpenGL, Vulkan, Metal, D3D, WebGPU, or OS API.
2. High-Level Flow
Application
    ↓
Zlay Public API
    ↓
Zlay Core
    ↓
Layout Backend
    ↓
Zlay Render Command List
    ↓
Renderer Backend
    ↓
GPU Driver
    ↓
Graphics API / Host Library

Example with Clay + OpenGL:

Zlay UI
    ↓
Clay Layout Backend
    ↓
ZLay_RenderCommandList
    ↓
OpenGL Renderer Backend
    ↓
OpenGL GPU Driver
    ↓
OpenGL API

Example with raylib:

Zlay UI
    ↓
Clay Layout Backend
    ↓
ZLay_RenderCommandList
    ↓
Raylib Renderer Backend
    ↓
raylib drawing API

Example with ImGui:

Zlay UI
    ↓
ZLay_RenderCommandList
    ↓
ImGui Renderer Backend
    ↓
ImDrawList
    ↓
Dear ImGui render pipeline
3. Recommended Folder Structure
Zlay/
├── include/
│   └── zlay/
│       └── zlay.hpp
│
├── zlay/
│   ├── core/
│   ├── renderer/
│   │   ├── zlay_renderer.c
│   │   ├── zlay_renderer.h
│   │   ├── zlay_render_command.h
│   │   ├── zlay_draw_list.h
│   │   └── zlay_batch.h
│   │
│   ├── backends/
│   │   ├── zlay_backends.h
│   │   ├── zlay_layout_backend.h
│   │   ├── zlay_renderer_backend.h
│   │   ├── zlay_platform_backend.h
│   │   ├── zlay_host_backend.h
│   │   ├── zlay_render_driver_backend.h
│   │   │
│   │   ├── layout/
│   │   │   └── clay/
│   │   │       ├── zlay_clay_layout_backend.c
│   │   │       └── zlay_clay_layout_backend.h
│   │   │
│   │   ├── renderer/
│   │   │   ├── opengl/
│   │   │   │   ├── zlay_opengl_renderer_backend.c
│   │   │   │   └── zlay_opengl_renderer_backend.h
│   │   │   ├── vulkan/
│   │   │   │   ├── zlay_vulkan_renderer_backend.c
│   │   │   │   └── zlay_vulkan_renderer_backend.h
│   │   │   ├── imgui/
│   │   │   │   ├── zlay_imgui_renderer_backend.c
│   │   │   │   └── zlay_imgui_renderer_backend.h
│   │   │   ├── sokol/
│   │   │   │   ├── zlay_sokol_renderer_backend.c
│   │   │   │   └── zlay_sokol_renderer_backend.h
│   │   │   └── raylib/
│   │   │       ├── zlay_raylib_renderer_backend.c
│   │   │       └── zlay_raylib_renderer_backend.h
│   │   │
│   │   ├── platform/
│   │   │   ├── win32/
│   │   │   ├── x11/
│   │   │   ├── cocoa/
│   │   │   ├── wayland/
│   │   │   ├── glfw/
│   │   │   ├── raylib/
│   │   │   └── sokol/
│   │   │
│   │   └── host/
│   │       ├── glfw/
│   │       │   ├── zlay_glfw_host_backend.c
│   │       │   └── zlay_glfw_host_backend.h
│   │       ├── imgui/
│   │       │   ├── zlay_imgui_host_backend.c
│   │       │   └── zlay_imgui_host_backend.h
│   │       ├── sokol/
│   │       │   ├── zlay_sokol_host_backend.c
│   │       │   └── zlay_sokol_host_backend.h
│   │       └── raylib/
│   │           ├── zlay_raylib_host_backend.c
│   │           └── zlay_raylib_host_backend.h
│   │
│   ├── drivers/
│   │   └── gpu/
│   │       ├── zlay_gpu_driver.h
│   │       │
│   │       ├── opengl/
│   │       │   ├── zlay_gl_driver.c
│   │       │   ├── zlay_gl_driver.h
│   │       │   ├── zlay_gl_buffer.c
│   │       │   ├── zlay_gl_buffer.h
│   │       │   ├── zlay_gl_texture.c
│   │       │   ├── zlay_gl_texture.h
│   │       │   ├── zlay_gl_pipeline.c
│   │       │   └── zlay_gl_pipeline.h
│   │       │
│   │       ├── vulkan/
│   │       │   ├── zlay_vk_driver.c
│   │       │   ├── zlay_vk_driver.h
│   │       │   ├── zlay_vk_instance.c
│   │       │   ├── zlay_vk_device.c
│   │       │   ├── zlay_vk_swapchain.c
│   │       │   ├── zlay_vk_command.c
│   │       │   ├── zlay_vk_pipeline.c
│   │       │   └── zlay_vk_sync.c
│   │       │
│   │       ├── webgpu/
│   │       ├── metal/
│   │       └── d3d12/
│   │
│   ├── os/
│   ├── parser/
│   ├── lifecycle/
│   ├── ui/
│   └── components/
│
├── third_party/
│   ├── clay/
│   │   ├── clay.h
│   │   └── LICENSE.md
│   ├── stb/
│   └── xxhash/
│
├── shader/
├── docs/
├── example/
├── CMakeLists.txt
├── AI_POLICY.md
├── ATTRIBUTION.md
├── NOTICE.md
└── LICENSE
4. Layer Definitions
4.1 Core

Location:

zlay/core/

Core contains general-purpose foundation code.

Examples:

- memory
- arena
- cache
- logger
- hash
- context

Core must not depend on:

- OpenGL
- Vulkan
- GLFW
- ImGui
- raylib
- sokol
- Clay

Allowed direction:

backend → core
renderer → core
driver → core

Forbidden direction:

core → backend
core → driver
core → third_party rendering library
4.2 Layout Backend

Location:

zlay/backends/layout/

Layout backend is responsible for translating a layout system into Zlay layout/render data.

Example:

zlay/backends/layout/clay/

Clay is a layout backend, not a GPU driver.

Clay should be used internally only.

Public Zlay users should not call Clay APIs directly.

4.3 Renderer Core

Location:

zlay/renderer/

Renderer core defines Zlay-owned rendering abstractions.

Responsibilities:

- render command list
- draw list
- batching model
- clipping abstraction
- shape draw abstraction
- text draw abstraction
- image draw abstraction

Renderer core must not directly call:

glDrawElements
vkCmdDraw
DrawRectangle
sg_draw
ImDrawList::AddRect

Those calls belong in renderer backends or GPU drivers.

4.4 Renderer Backend

Location:

zlay/backends/renderer/

Renderer backend translates ZLay_RenderCommandList into a concrete rendering implementation.

Examples:

zlay/backends/renderer/opengl/
zlay/backends/renderer/vulkan/
zlay/backends/renderer/imgui/
zlay/backends/renderer/sokol/
zlay/backends/renderer/raylib/

Renderer backend may depend on:

- zlay/renderer
- zlay/core
- zlay/drivers/gpu/*
- external host library, if backend-specific

Renderer backend must not define Zlay public API.

4.5 GPU Driver

Location:

zlay/drivers/gpu/

GPU driver is a low-level wrapper around graphics APIs.

Examples:

zlay/drivers/gpu/opengl/
zlay/drivers/gpu/vulkan/
zlay/drivers/gpu/webgpu/
zlay/drivers/gpu/metal/
zlay/drivers/gpu/d3d12/

GPU driver responsibilities:

- create buffer
- upload buffer
- create texture
- compile shader
- create pipeline
- create swapchain
- create command buffer
- handle GPU sync
- issue low-level draw call

GPU driver must not know:

- ZLay_Button
- ZLay_Panel
- ZLay_Style
- Clay element types
- widget tree
- parser AST
4.6 Platform Backend

Location:

zlay/backends/platform/

Platform backend handles windowing, input, DPI, cursor, clipboard, and OS events.

Examples:

zlay/backends/platform/win32/
zlay/backends/platform/x11/
zlay/backends/platform/cocoa/
zlay/backends/platform/wayland/
zlay/backends/platform/glfw/
zlay/backends/platform/raylib/
zlay/backends/platform/sokol/

Platform backend responsibilities:

- window creation
- input polling
- keyboard events
- mouse events
- scroll events
- DPI scale
- cursor
- monitor
- clipboard
4.7 Host Backend

Location:

zlay/backends/host/

Host backend integrates Zlay into an existing application framework.

Examples:

zlay/backends/host/glfw/
zlay/backends/host/imgui/
zlay/backends/host/sokol/
zlay/backends/host/raylib/

Host backend is useful when an external library provides multiple systems at once.

Example:

raylib = window + input + drawing
sokol  = app + input + graphics
GLFW   = window + input + GL/Vulkan context
ImGui  = debug UI + draw list
5. Third-Party Dependencies

Third-party code must be placed in:

third_party/

Recommended structure:

third_party/
├── clay/
│   ├── clay.h
│   └── LICENSE.md
├── stb/
├── xxhash/
└── ...

Rules:

Do not mix third-party source directly into Zlay core.
Preserve original license notices.
Do not claim authorship of third-party code.
Modified third-party source must be clearly marked.
Public Zlay headers should not expose third-party types unless explicitly intended.
Third-party code must be documented in NOTICE.md or ATTRIBUTION.md.
6. Clay Integration

Clay may be used as an optional layout backend.

Location:

third_party/clay/
zlay/backends/layout/clay/

Flow:

Zlay Public API
    ↓
Zlay Layout Backend Interface
    ↓
Clay Layout Backend Adapter
    ↓
third_party/clay/clay.h

Rules:

- Do not expose Clay types in public Zlay API.
- Do not rename Clay as Zlay.
- Do not claim Clay authorship.
- Preserve Clay license.
- Convert Clay output into Zlay render commands.

Recommended files:

zlay/backends/layout/clay/
├── zlay_clay_layout_backend.c
└── zlay_clay_layout_backend.h

Clay backend should output:

ZLay_RenderCommandList

It must not return:

Clay_RenderCommandArray

from public Zlay APIs.

7. Renderer Backend Categories
7.1 OpenGL Renderer Backend

Location:

zlay/backends/renderer/opengl/

Depends on:

zlay/drivers/gpu/opengl/

Responsibilities:

- receive ZLay_RenderCommandList
- batch rectangles
- batch text
- batch images
- handle scissor/clipping
- call OpenGL GPU driver

Low-level OpenGL calls should be hidden inside:

zlay/drivers/gpu/opengl/
7.2 Vulkan Renderer Backend

Location:

zlay/backends/renderer/vulkan/

Depends on:

zlay/drivers/gpu/vulkan/

Responsibilities:

- receive ZLay_RenderCommandList
- create render batches
- bind pipeline
- bind descriptors
- record command buffers
- handle scissor/clipping
- call Vulkan GPU driver

Driver handles:

- instance
- device
- swapchain
- command pool
- command buffer
- sync
- pipeline
- buffer
- image
7.3 ImGui Renderer Backend

Location:

zlay/backends/renderer/imgui/

Purpose:

- debug preview
- prototype renderer
- editor tool embedding

Flow:

ZLay_RenderCommandList
    ↓
ImGui Renderer Backend
    ↓
ImDrawList

Rules:

- ImGui is optional.
- ImGui must not become required dependency.
- ImGui types must not leak into core public API.
- ImGui backend is mainly for debug/prototyping.
7.4 sokol Renderer Backend

Location:

zlay/backends/renderer/sokol/

Depends on:

sokol_gfx

Responsibilities:

- create sokol pipeline
- upload vertices
- upload indices
- manage font texture
- issue sg_draw

Optional platform backend:

zlay/backends/platform/sokol/

Optional host backend:

zlay/backends/host/sokol/
7.5 raylib Renderer Backend

Location:

zlay/backends/renderer/raylib/

Responsibilities:

- DrawRectangle
- DrawRectangleRounded
- DrawTextEx
- DrawTexture
- BeginScissorMode
- EndScissorMode

raylib backend is useful for:

- examples
- quick demo
- education
- prototype

raylib may also provide:

- platform backend
- host backend

Locations:

zlay/backends/platform/raylib/
zlay/backends/host/raylib/
8. GLFW Classification

GLFW is not a renderer backend.

GLFW should be treated as:

platform backend
host backend

Locations:

zlay/backends/platform/glfw/
zlay/backends/host/glfw/

GLFW responsibilities:

- create window
- create OpenGL context
- create Vulkan surface
- poll events
- mouse input
- keyboard input
- window size
- framebuffer size
- DPI scale

GLFW should not directly render Zlay UI.

Rendering should still be handled by:

zlay/backends/renderer/opengl/
zlay/backends/renderer/vulkan/
9. Dependency Direction Rules

Allowed:

backends/layout/clay       → third_party/clay
backends/renderer/opengl   → drivers/gpu/opengl
backends/renderer/vulkan   → drivers/gpu/vulkan
backends/renderer/raylib   → raylib
backends/renderer/sokol    → sokol_gfx
backends/platform/glfw     → GLFW

Forbidden:

core                        → backends/*
core                        → drivers/*
drivers/gpu/opengl          → backends/renderer/opengl
drivers/gpu/vulkan          → backends/renderer/vulkan
renderer core               → ImGui/raylib/sokol directly
public zlay.h               → clay.h
public zlay.h               → GLFW/glfw3.h
public zlay.h               → imgui.h
10. Public API Rule

Public Zlay API must stay clean.

Good:

#include <zlay.h>

ZLay_BeginFrame(ctx, width, height, delta_time);
ZLay_Panel(ctx, ...);
ZLay_Text(ctx, "Hello Zlay");
ZLay_EndFrame(ctx);

Bad:

#include "clay.h"
#include "GLFW/glfw3.h"
#include "imgui.h"

in core public Zlay API.

Backend-specific headers may expose external types only when necessary.

Example:

typedef struct ZLay_GlfwHostConfig {
    void* glfw_window;
    bool owns_window;
} ZLay_GlfwHostConfig;
11. Interface Specification
11.1 Layout Backend Interface
#ifndef ZLAY_LAYOUT_BACKEND_H
#define ZLAY_LAYOUT_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct ZLay_Context ZLay_Context;
typedef struct ZLay_RenderCommandList ZLay_RenderCommandList;
typedef struct ZLay_LayoutBackend ZLay_LayoutBackend;

typedef struct ZLay_LayoutBackendVTable {
    bool (*init)(ZLay_LayoutBackend* backend, ZLay_Context* ctx);
    void (*shutdown)(ZLay_LayoutBackend* backend, ZLay_Context* ctx);

    void (*begin_frame)(
        ZLay_LayoutBackend* backend,
        ZLay_Context* ctx,
        float width,
        float height,
        float delta_time
    );

    ZLay_RenderCommandList (*end_frame)(
        ZLay_LayoutBackend* backend,
        ZLay_Context* ctx
    );

    void (*set_pointer)(
        ZLay_LayoutBackend* backend,
        ZLay_Context* ctx,
        float x,
        float y,
        bool down
    );

    void (*set_scroll)(
        ZLay_LayoutBackend* backend,
        ZLay_Context* ctx,
        float x,
        float y
    );
} ZLay_LayoutBackendVTable;

struct ZLay_LayoutBackend {
    const char* name;
    void* userdata;
    const ZLay_LayoutBackendVTable* vtable;
};

#ifdef __cplusplus
}
#endif

#endif
11.2 Renderer Backend Interface
#ifndef ZLAY_RENDERER_BACKEND_H
#define ZLAY_RENDERER_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct ZLay_Context ZLay_Context;
typedef struct ZLay_RenderCommandList ZLay_RenderCommandList;
typedef struct ZLay_RendererBackend ZLay_RendererBackend;

typedef struct ZLay_RendererBackendVTable {
    bool (*init)(ZLay_RendererBackend* backend, ZLay_Context* ctx);
    void (*shutdown)(ZLay_RendererBackend* backend, ZLay_Context* ctx);

    void (*begin_frame)(
        ZLay_RendererBackend* backend,
        ZLay_Context* ctx,
        int width,
        int height
    );

    void (*render)(
        ZLay_RendererBackend* backend,
        ZLay_Context* ctx,
        const ZLay_RenderCommandList* commands
    );

    void (*end_frame)(
        ZLay_RendererBackend* backend,
        ZLay_Context* ctx
    );

    void (*resize)(
        ZLay_RendererBackend* backend,
        ZLay_Context* ctx,
        int width,
        int height
    );
} ZLay_RendererBackendVTable;

struct ZLay_RendererBackend {
    const char* name;
    void* userdata;
    const ZLay_RendererBackendVTable* vtable;
};

#ifdef __cplusplus
}
#endif

#endif
11.3 GPU Driver Interface
#ifndef ZLAY_GPU_DRIVER_H
#define ZLAY_GPU_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct ZLay_GpuDriver ZLay_GpuDriver;

typedef enum ZLay_GpuBackendKind {
    ZLAY_GPU_BACKEND_NONE = 0,
    ZLAY_GPU_BACKEND_OPENGL,
    ZLAY_GPU_BACKEND_VULKAN,
    ZLAY_GPU_BACKEND_WEBGPU,
    ZLAY_GPU_BACKEND_METAL,
    ZLAY_GPU_BACKEND_D3D11,
    ZLAY_GPU_BACKEND_D3D12
} ZLay_GpuBackendKind;

typedef struct ZLay_GpuBufferHandle {
    uint64_t id;
} ZLay_GpuBufferHandle;

typedef struct ZLay_GpuTextureHandle {
    uint64_t id;
} ZLay_GpuTextureHandle;

typedef struct ZLay_GpuPipelineHandle {
    uint64_t id;
} ZLay_GpuPipelineHandle;

typedef struct ZLay_GpuDriverVTable {
    bool (*init)(ZLay_GpuDriver* driver);
    void (*shutdown)(ZLay_GpuDriver* driver);

    ZLay_GpuBufferHandle (*create_buffer)(
        ZLay_GpuDriver* driver,
        const void* data,
        size_t size
    );

    void (*destroy_buffer)(
        ZLay_GpuDriver* driver,
        ZLay_GpuBufferHandle buffer
    );

    ZLay_GpuTextureHandle (*create_texture)(
        ZLay_GpuDriver* driver,
        int width,
        int height,
        const void* pixels
    );

    void (*destroy_texture)(
        ZLay_GpuDriver* driver,
        ZLay_GpuTextureHandle texture
    );

    void (*set_scissor)(
        ZLay_GpuDriver* driver,
        int x,
        int y,
        int width,
        int height
    );

    void (*draw)(
        ZLay_GpuDriver* driver,
        ZLay_GpuBufferHandle vertex_buffer,
        ZLay_GpuBufferHandle index_buffer,
        uint32_t index_count
    );
} ZLay_GpuDriverVTable;

struct ZLay_GpuDriver {
    const char* name;
    ZLay_GpuBackendKind kind;
    void* userdata;
    const ZLay_GpuDriverVTable* vtable;
};

#ifdef __cplusplus
}
#endif

#endif
11.4 Platform Backend Interface
#ifndef ZLAY_PLATFORM_BACKEND_H
#define ZLAY_PLATFORM_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct ZLay_Context ZLay_Context;
typedef struct ZLay_PlatformBackend ZLay_PlatformBackend;

typedef struct ZLay_PlatformEvent {
    int type;
    float mouse_x;
    float mouse_y;
    float scroll_x;
    float scroll_y;
    int key;
    int down;
} ZLay_PlatformEvent;

typedef struct ZLay_PlatformBackendVTable {
    bool (*init)(ZLay_PlatformBackend* backend, ZLay_Context* ctx);
    void (*shutdown)(ZLay_PlatformBackend* backend, ZLay_Context* ctx);

    bool (*poll_event)(
        ZLay_PlatformBackend* backend,
        ZLay_Context* ctx,
        ZLay_PlatformEvent* out_event
    );

    void (*get_window_size)(
        ZLay_PlatformBackend* backend,
        int* out_width,
        int* out_height
    );

    float (*get_dpi_scale)(
        ZLay_PlatformBackend* backend
    );
} ZLay_PlatformBackendVTable;

struct ZLay_PlatformBackend {
    const char* name;
    void* userdata;
    const ZLay_PlatformBackendVTable* vtable;
};

#ifdef __cplusplus
}
#endif

#endif
12. CMake Options

Recommended feature flags:

option(ZLAY_ENABLE_BACKEND_CLAY "Enable Clay layout backend" ON)

option(ZLAY_ENABLE_RENDERER_OPENGL "Enable OpenGL renderer backend" ON)
option(ZLAY_ENABLE_RENDERER_VULKAN "Enable Vulkan renderer backend" OFF)
option(ZLAY_ENABLE_RENDERER_IMGUI "Enable Dear ImGui renderer backend" OFF)
option(ZLAY_ENABLE_RENDERER_SOKOL "Enable sokol renderer backend" OFF)
option(ZLAY_ENABLE_RENDERER_RAYLIB "Enable raylib renderer backend" OFF)

option(ZLAY_ENABLE_PLATFORM_GLFW "Enable GLFW platform backend" OFF)
option(ZLAY_ENABLE_PLATFORM_RAYLIB "Enable raylib platform backend" OFF)
option(ZLAY_ENABLE_PLATFORM_SOKOL "Enable sokol platform backend" OFF)
option(ZLAY_ENABLE_PLATFORM_WIN32 "Enable Win32 platform backend" ON)
option(ZLAY_ENABLE_PLATFORM_X11 "Enable X11 platform backend" ON)
option(ZLAY_ENABLE_PLATFORM_COCOA "Enable Cocoa platform backend" ON)

option(ZLAY_ENABLE_DRIVER_OPENGL "Enable OpenGL GPU driver" ON)
option(ZLAY_ENABLE_DRIVER_VULKAN "Enable Vulkan GPU driver" OFF)
option(ZLAY_ENABLE_DRIVER_WEBGPU "Enable WebGPU GPU driver" OFF)
option(ZLAY_ENABLE_DRIVER_METAL "Enable Metal GPU driver" OFF)
option(ZLAY_ENABLE_DRIVER_D3D12 "Enable Direct3D 12 GPU driver" OFF)

Rules:

OpenGL renderer backend requires OpenGL GPU driver.
Vulkan renderer backend requires Vulkan GPU driver.
GLFW platform backend does not automatically mean OpenGL renderer.
raylib backend may provide platform + renderer.
sokol backend may provide platform + renderer.
ImGui renderer backend is optional debug/prototype bridge.
13. Dependency Matrix
Backend / Driver              Depends on
--------------------------------------------------------
layout/clay                   third_party/clay
renderer/opengl               drivers/gpu/opengl
renderer/vulkan               drivers/gpu/vulkan
renderer/imgui                Dear ImGui
renderer/sokol                sokol_gfx
renderer/raylib               raylib
platform/glfw                 GLFW
platform/sokol                sokol_app
platform/raylib               raylib
platform/win32                Win32 API
platform/x11                  Xlib / X11
platform/cocoa                Cocoa
drivers/gpu/opengl            OpenGL loader / system GL
drivers/gpu/vulkan            Vulkan SDK / Vulkan loader
drivers/gpu/webgpu            WebGPU implementation
drivers/gpu/metal             Metal
drivers/gpu/d3d12             Direct3D 12
14. Implementation Phases
Phase 1 — Structure Cleanup
1. Keep Clay layout backend under zlay/backends/layout/clay.
2. Keep GPU driver code under zlay/drivers/gpu.
3. Keep zlay/renderer as renderer core.
4. Rename lib to third_party.
5. Keep third-party licenses in each dependency folder.
Phase 2 — Interfaces

Add:

zlay/backends/zlay_layout_backend.h
zlay/backends/zlay_renderer_backend.h
zlay/backends/zlay_platform_backend.h
zlay/backends/zlay_host_backend.h
zlay/backends/zlay_render_driver_backend.h
zlay/drivers/gpu/zlay_gpu_driver.h
Phase 3 — Clay Layout Backend
1. Use Clay internally.
2. Preserve Clay license.
3. Do not expose Clay types.
4. Convert Clay commands into ZLay_RenderCommandList.
5. Keep public API under ZLay_*.
Phase 4 — OpenGL Renderer Path
1. Implement OpenGL GPU driver.
2. Implement OpenGL renderer backend.
3. Render rectangle command.
4. Render rounded rectangle command.
5. Render text command.
6. Render image command.
7. Add scissor/clipping.
Phase 5 — GLFW Platform Backend
1. Create GLFW window.
2. Poll keyboard/mouse events.
3. Provide framebuffer size.
4. Provide DPI scale.
5. Connect to OpenGL or Vulkan renderer backend.
Phase 6 — raylib Backend
1. Create raylib renderer backend.
2. Convert Zlay commands to raylib drawing calls.
3. Add raylib example.
4. Keep raylib optional.
Phase 7 — ImGui Backend
1. Convert Zlay commands to ImDrawList.
2. Use for debug/prototype only.
3. Keep ImGui optional.
Phase 8 — sokol Backend
1. Add sokol_app platform backend.
2. Add sokol_gfx renderer backend.
3. Add sokol example.
4. Keep sokol optional.
15. Naming Convention
Layout Backend
zlay_<name>_layout_backend.h
zlay_<name>_layout_backend.c

Example:

zlay_clay_layout_backend.c
Renderer Backend
zlay_<name>_renderer_backend.h
zlay_<name>_renderer_backend.c

Examples:

zlay_opengl_renderer_backend.c
zlay_vulkan_renderer_backend.c
zlay_imgui_renderer_backend.c
zlay_sokol_renderer_backend.c
zlay_raylib_renderer_backend.c
Platform Backend
zlay_<name>_platform_backend.h
zlay_<name>_platform_backend.c

Examples:

zlay_glfw_platform_backend.c
zlay_win32_platform_backend.c
zlay_x11_platform_backend.c
zlay_cocoa_platform_backend.c
Host Backend
zlay_<name>_host_backend.h
zlay_<name>_host_backend.c

Examples:

zlay_glfw_host_backend.c
zlay_raylib_host_backend.c
zlay_sokol_host_backend.c
zlay_imgui_host_backend.c
GPU Driver
zlay_<api>_driver.h
zlay_<api>_driver.c

Examples:

zlay_gl_driver.c
zlay_vk_driver.c
zlay_wgpu_driver.c
zlay_metal_driver.mm
zlay_d3d12_driver.cpp
16. Backend Rules for AI Agent

When implementing backends, follow these rules:

1. Do not expose third-party types in core public headers.
2. Keep Clay inside layout backend.
3. Keep GLFW inside platform/host backend.
4. Keep ImGui inside renderer/debug backend.
5. Keep raylib inside platform/renderer/host backend.
6. Keep sokol inside platform/renderer/host backend.
7. Keep GPU API calls inside drivers/gpu.
8. Renderer backend may call GPU driver.
9. GPU driver must not call renderer backend.
10. Core must not depend on backend or driver.
11. Public API must remain ZLay_*.
12. Third-party code must stay in third_party/.
13. Every third-party dependency must keep its license.
14. Modified third-party source must be marked.
15. Renderer core must stay independent from OpenGL/Vulkan/raylib/sokol/ImGui.
17. Example Configuration
OpenGL + GLFW + Clay
ZLay_Config config = {0};

config.layout_backend = ZLAY_LAYOUT_BACKEND_CLAY;
config.platform_backend = ZLAY_PLATFORM_BACKEND_GLFW;
config.renderer_backend = ZLAY_RENDERER_BACKEND_OPENGL;
config.gpu_driver = ZLAY_GPU_BACKEND_OPENGL;

Flow:

GLFW Platform Backend
    ↓
Clay Layout Backend
    ↓
ZLay_RenderCommandList
    ↓
OpenGL Renderer Backend
    ↓
OpenGL GPU Driver
raylib + Clay
ZLay_Config config = {0};

config.layout_backend = ZLAY_LAYOUT_BACKEND_CLAY;
config.platform_backend = ZLAY_PLATFORM_BACKEND_RAYLIB;
config.renderer_backend = ZLAY_RENDERER_BACKEND_RAYLIB;

Flow:

raylib Host
    ↓
Clay Layout Backend
    ↓
ZLay_RenderCommandList
    ↓
Raylib Renderer Backend
ImGui Debug Backend
ZLay_Config config = {0};

config.layout_backend = ZLAY_LAYOUT_BACKEND_CLAY;
config.renderer_backend = ZLAY_RENDERER_BACKEND_IMGUI;

Flow:

Zlay Render Commands
    ↓
ImGui Renderer Backend
    ↓
ImDrawList
sokol Backend
ZLay_Config config = {0};

config.layout_backend = ZLAY_LAYOUT_BACKEND_CLAY;
config.platform_backend = ZLAY_PLATFORM_BACKEND_SOKOL;
config.renderer_backend = ZLAY_RENDERER_BACKEND_SOKOL;

Flow:

sokol_app
    ↓
Sokol Platform Backend
    ↓
ZLay Events
    ↓
ZLay Render Commands
    ↓
sokol_gfx
18. What Must Not Happen

Do not do this:

zlay.h includes clay.h
zlay.h includes glfw3.h
zlay.h includes imgui.h
zlay.h includes raylib.h
zlay.h includes sokol_gfx.h

Do not do this:

typedef Clay_RenderCommandArray ZLay_RenderCommandList;
#define ZLay_BeginFrame Clay_BeginLayout
#define ZLay_EndFrame Clay_EndLayout

Do not put low-level GPU code inside renderer core:

zlay/renderer/zlay_renderer.c
    glBindBuffer(...)
    vkCmdDraw(...)

That belongs in:

zlay/drivers/gpu/opengl/
zlay/drivers/gpu/vulkan/
19. Architecture Summary
renderer core     → zlay/renderer/
renderer backend  → zlay/backends/renderer/
gpu driver        → zlay/drivers/gpu/
layout backend    → zlay/backends/layout/
platform backend  → zlay/backends/platform/
host integration  → zlay/backends/host/
third-party code  → third_party/

Final rule:

Backend is the translator.
Driver is the low-level handle.
Renderer core is Zlay-owned abstraction.
Third-party code is isolated.
Public API stays clean.
20. Recommended AI Agent Prompt

Use this prompt when asking an AI agent to refactor or implement the architecture:

Refactor Zlay backend and driver architecture.

Required architecture:

1. Move Clay layout backend into:
   zlay/backends/layout/clay/

2. Keep Zlay renderer abstraction in:
   zlay/renderer/

3. Add renderer backends under:
   zlay/backends/renderer/opengl/
   zlay/backends/renderer/vulkan/
   zlay/backends/renderer/imgui/
   zlay/backends/renderer/sokol/
   zlay/backends/renderer/raylib/

4. Keep low-level GPU drivers under:
   zlay/drivers/gpu/opengl/
   zlay/drivers/gpu/vulkan/
   zlay/drivers/gpu/webgpu/
   zlay/drivers/gpu/metal/
   zlay/drivers/gpu/d3d12/

5. Do not put GPU drivers inside backend.

6. Define backend as:
   adapter from Zlay abstraction to a specific implementation.

7. Define driver as:
   low-level wrapper around graphics API / GPU API.

8. Renderer backend may call GPU driver.
   GPU driver must not call renderer backend.

9. GLFW is platform/host backend, not renderer backend.
   Put it under:
   zlay/backends/platform/glfw/
   zlay/backends/host/glfw/

10. ImGui is renderer/debug backend.
    Put it under:
    zlay/backends/renderer/imgui/

11. raylib can be platform + renderer + host backend.
    Put related files under:
    zlay/backends/platform/raylib/
    zlay/backends/renderer/raylib/
    zlay/backends/host/raylib/

12. sokol can be platform + renderer + host backend.
    Put related files under:
    zlay/backends/platform/sokol/
    zlay/backends/renderer/sokol/
    zlay/backends/host/sokol/

13. Public Zlay API must not expose Clay, GLFW, ImGui, raylib, or sokol types.

14. Third-party libraries must stay in third_party/.

15. Add CMake options for each backend and driver.

16. Preserve all third-party licenses.

17. Keep Zlay public API clean under ZLay_*.

18. Renderer core must not call OpenGL/Vulkan/raylib/sokol/ImGui directly.
21. Final Position

Zlay should not be a thin wrapper around Clay, ImGui, raylib, sokol, GLFW, OpenGL, or Vulkan.

Zlay should be its own native GUI/layout framework.

Third-party libraries are allowed, but they must stay behind clean backend boundaries.

Zlay owns:
- public API
- style system
- renderer abstraction
- render command list
- backend interfaces
- editor/creative-tool direction

Third-party libraries provide:
- optional layout backend
- optional platform backend
- optional renderer backend
- optional host integration
- optional GPU implementation layer

This keeps Zlay flexible, auditable, and ready for future engine/editor/DAW integration.
