# Zlay Canvas Renderer Architecture

Zlay Canvas is a 2D drawing abstraction layer inside the Zlay renderer system.

Canvas is not a backend and not a driver.

Canvas belongs to the renderer abstraction because it provides high-level 2D drawing commands such as rectangles, paths, text, images, gradients, clipping, and transforms.

---

## 1. Core Decision

Canvas should be placed under:

```txt
zlay/renderer/canvas/
```

Not under:

```txt
zlay/backends/
zlay/drivers/
third_party/
```

Reason:

```txt
canvas = Zlay-owned rendering abstraction
backend = adapter from Zlay abstraction to external rendering implementation
driver  = low-level GPU/API wrapper
```

Canvas is part of Zlay's own rendering model.

---

## 2. High-Level Flow

```txt
Zlay UI / Layout
    ↓
Zlay Canvas
    ↓
ZLay_RenderCommandList
    ↓
Renderer Backend
    ↓
GPU Driver / Host Library
    ↓
OpenGL / Vulkan / raylib / sokol / ImGui / WebGPU / Metal / D3D
```

Example with OpenGL:

```txt
Zlay UI
    ↓
Canvas commands
    ↓
ZLay_RenderCommandList
    ↓
OpenGL Renderer Backend
    ↓
OpenGL GPU Driver
    ↓
OpenGL API
```

Example with raylib:

```txt
Zlay UI
    ↓
Canvas commands
    ↓
ZLay_RenderCommandList
    ↓
Raylib Renderer Backend
    ↓
raylib DrawRectangle / DrawText / DrawTexture
```

Example with ImGui:

```txt
Zlay UI
    ↓
Canvas commands
    ↓
ZLay_RenderCommandList
    ↓
ImGui Renderer Backend
    ↓
ImDrawList
```

---

## 3. Recommended Folder Structure

```txt
zlay/
├── renderer/
│   ├── canvas/
│   │   ├── zlay_canvas.h
│   │   ├── zlay_canvas.c
│   │   │
│   │   ├── zlay_canvas_command.h
│   │   ├── zlay_canvas_command.c
│   │   │
│   │   ├── zlay_canvas_paint.h
│   │   ├── zlay_canvas_paint.c
│   │   │
│   │   ├── zlay_canvas_path.h
│   │   ├── zlay_canvas_path.c
│   │   │
│   │   ├── zlay_canvas_text.h
│   │   ├── zlay_canvas_text.c
│   │   │
│   │   ├── zlay_canvas_image.h
│   │   ├── zlay_canvas_image.c
│   │   │
│   │   ├── zlay_canvas_gradient.h
│   │   ├── zlay_canvas_gradient.c
│   │   │
│   │   ├── zlay_canvas_transform.h
│   │   ├── zlay_canvas_transform.c
│   │   │
│   │   ├── zlay_canvas_clip.h
│   │   └── zlay_canvas_clip.c
│   │
│   ├── zlay_renderer.h
│   ├── zlay_renderer.c
│   ├── zlay_render_command.h
│   ├── zlay_draw_list.h
│   └── zlay_batch.h
│
├── backends/
│   ├── renderer/
│   │   ├── opengl/
│   │   ├── vulkan/
│   │   ├── raylib/
│   │   ├── sokol/
│   │   └── imgui/
│   │
│   └── layout/
│       └── clay/
│
└── drivers/
    └── gpu/
        ├── opengl/
        └── vulkan/
```

---

## 4. Layer Meaning

### 4.1 Canvas

Location:

```txt
zlay/renderer/canvas/
```

Canvas is responsible for high-level 2D drawing.

Canvas knows about:

```txt
- rectangles
- rounded rectangles
- lines
- paths
- text
- images
- gradients
- clipping
- transforms
- paint styles
- stroke styles
- fill styles
```

Canvas does not know about:

```txt
- glBindBuffer
- glDrawElements
- vkCmdDraw
- swapchain
- GPU command buffer
- window handles
- platform events
```

Canvas outputs Zlay render commands.

---

### 4.2 Renderer Core

Location:

```txt
zlay/renderer/
```

Renderer core owns:

```txt
- render command list
- draw list
- batching abstraction
- renderer state
- canvas integration
```

Canvas is part of renderer core.

---

### 4.3 Renderer Backend

Location:

```txt
zlay/backends/renderer/
```

Renderer backend translates Zlay render commands into a concrete rendering implementation.

Examples:

```txt
zlay/backends/renderer/opengl/
zlay/backends/renderer/vulkan/
zlay/backends/renderer/raylib/
zlay/backends/renderer/sokol/
zlay/backends/renderer/imgui/
```

Renderer backend may receive canvas-generated commands and render them.

---

### 4.4 GPU Driver

Location:

```txt
zlay/drivers/gpu/
```

GPU driver is a low-level wrapper around a graphics API.

Examples:

```txt
zlay/drivers/gpu/opengl/
zlay/drivers/gpu/vulkan/
zlay/drivers/gpu/webgpu/
zlay/drivers/gpu/metal/
zlay/drivers/gpu/d3d12/
```

GPU driver does not know about canvas concepts.

---

## 5. Canvas API Scope

Canvas should provide an API similar to a lightweight 2D drawing surface.

Example:

```c
ZLay_Canvas* canvas = ZLay_GetCanvas(ctx);

ZLay_CanvasBegin(canvas);

ZLay_CanvasRect(canvas, x, y, w, h, paint);
ZLay_CanvasRoundedRect(canvas, x, y, w, h, radius, paint);
ZLay_CanvasLine(canvas, x1, y1, x2, y2, stroke);
ZLay_CanvasText(canvas, x, y, "Hello Zlay", text_style);
ZLay_CanvasImage(canvas, image, x, y, w, h);

ZLay_CanvasEnd(canvas);
```

Canvas should not directly call backend rendering APIs.

Bad:

```c
glDrawElements(...);
```

inside:

```txt
zlay/renderer/canvas/
```

Good:

```txt
Canvas command
    ↓
ZLay_RenderCommandList
    ↓
Renderer backend
```

---

## 6. Canvas Commands

Canvas commands should be converted into Zlay render commands.

Recommended command types:

```c
typedef enum ZLay_CanvasCommandType {
    ZLAY_CANVAS_COMMAND_NONE = 0,

    ZLAY_CANVAS_COMMAND_RECT,
    ZLAY_CANVAS_COMMAND_ROUNDED_RECT,
    ZLAY_CANVAS_COMMAND_LINE,
    ZLAY_CANVAS_COMMAND_PATH,
    ZLAY_CANVAS_COMMAND_TEXT,
    ZLAY_CANVAS_COMMAND_IMAGE,

    ZLAY_CANVAS_COMMAND_CLIP_BEGIN,
    ZLAY_CANVAS_COMMAND_CLIP_END,

    ZLAY_CANVAS_COMMAND_PUSH_TRANSFORM,
    ZLAY_CANVAS_COMMAND_POP_TRANSFORM,

    ZLAY_CANVAS_COMMAND_CUSTOM
} ZLay_CanvasCommandType;
```

Canvas commands are internal drawing instructions.

They may later be flattened into:

```txt
ZLay_RenderCommandList
```

---

## 7. Canvas Paint System

Canvas should use a paint model.

File:

```txt
zlay/renderer/canvas/zlay_canvas_paint.h
```

Suggested types:

```c
typedef enum ZLay_PaintType {
    ZLAY_PAINT_NONE = 0,
    ZLAY_PAINT_SOLID,
    ZLAY_PAINT_LINEAR_GRADIENT,
    ZLAY_PAINT_RADIAL_GRADIENT,
    ZLAY_PAINT_IMAGE
} ZLay_PaintType;

typedef struct ZLay_CanvasPaint {
    ZLay_PaintType type;

    float r;
    float g;
    float b;
    float a;

    void* image;
    void* gradient;
} ZLay_CanvasPaint;
```

Paint should be renderer-agnostic.

It should not store OpenGL texture IDs directly.

Use Zlay texture handles instead.

---

## 8. Canvas Path System

File:

```txt
zlay/renderer/canvas/zlay_canvas_path.h
```

Path should support:

```txt
- move_to
- line_to
- quadratic_to
- cubic_to
- close
```

Suggested API:

```c
ZLay_CanvasPath path;

ZLay_CanvasPathInit(&path);
ZLay_CanvasPathMoveTo(&path, x, y);
ZLay_CanvasPathLineTo(&path, x, y);
ZLay_CanvasPathCubicTo(&path, cx1, cy1, cx2, cy2, x, y);
ZLay_CanvasPathClose(&path);

ZLay_CanvasDrawPath(canvas, &path, paint);
```

Path triangulation should be separate from path definition.

Recommended future folder:

```txt
zlay/renderer/tessellator/
```

or:

```txt
zlay/renderer/canvas/tessellator/
```

---

## 9. Canvas Text System

File:

```txt
zlay/renderer/canvas/zlay_canvas_text.h
```

Canvas text should handle high-level text drawing.

Responsibilities:

```txt
- text command creation
- text position
- font handle
- size
- color
- alignment
```

Text rendering implementation may later use:

```txt
- stb_truetype for early stage
- FreeType
- HarfBuzz
- font atlas
- glyph cache
```

Text command should not directly depend on FreeType or HarfBuzz in the basic public canvas API.

Suggested type:

```c
typedef struct ZLay_TextStyle {
    void* font;
    float size;
    float r;
    float g;
    float b;
    float a;
} ZLay_TextStyle;
```

---

## 10. Canvas Image System

File:

```txt
zlay/renderer/canvas/zlay_canvas_image.h
```

Canvas image command should reference a Zlay image/texture handle.

Suggested type:

```c
typedef struct ZLay_ImageHandle {
    unsigned long long id;
} ZLay_ImageHandle;
```

Canvas should not store:

```txt
GLuint
VkImage
ID3D12Resource*
MTLTexture*
```

directly.

Those belong to `zlay/backends/renderer/*` or `zlay/drivers/gpu/*`.

---

## 11. Canvas Transform System

File:

```txt
zlay/renderer/canvas/zlay_canvas_transform.h
```

Canvas should support transform stack:

```txt
- translate
- scale
- rotate
- push transform
- pop transform
```

Suggested API:

```c
ZLay_CanvasPushTransform(canvas);
ZLay_CanvasTranslate(canvas, x, y);
ZLay_CanvasScale(canvas, sx, sy);
ZLay_CanvasRotate(canvas, radians);
ZLay_CanvasPopTransform(canvas);
```

Renderer backend should receive already-resolved transform data or command stack depending on implementation phase.

---

## 12. Canvas Clipping System

File:

```txt
zlay/renderer/canvas/zlay_canvas_clip.h
```

Canvas should support clipping:

```txt
- rectangular clip
- rounded clip later
- path clip later
```

Suggested API:

```c
ZLay_CanvasPushClipRect(canvas, x, y, w, h);
ZLay_CanvasPopClip(canvas);
```

This maps to renderer backends:

```txt
OpenGL  -> glScissor
Vulkan  -> vkCmdSetScissor
raylib  -> BeginScissorMode / EndScissorMode
ImGui   -> PushClipRect / PopClipRect
sokol   -> scissor state
```

---

## 13. Canvas and Render Command Relationship

Canvas can be implemented in two ways.

### Option A: Canvas directly emits render commands

```txt
Canvas API
    ↓
ZLay_RenderCommandList
    ↓
Renderer Backend
```

Pros:

```txt
- simple
- good for early stage
- less memory
- easier to debug
```

Cons:

```txt
- harder to support retained canvas operations
- less flexible for optimization
```

### Option B: Canvas stores canvas commands first

```txt
Canvas API
    ↓
ZLay_CanvasCommandList
    ↓
Canvas compiler
    ↓
ZLay_RenderCommandList
    ↓
Renderer Backend
```

Pros:

```txt
- more flexible
- easier to optimize
- easier to implement transform stack
- easier to implement path/tessellation
```

Cons:

```txt
- more code
- more memory
- needs compiler stage
```

Recommended for Zlay:

```txt
Phase 1: Option A
Phase 2: Option B
```

Start simple, then evolve.

---

## 14. Canvas and Backend Relationship

Canvas is not a backend.

Backends render canvas output.

```txt
zlay/renderer/canvas/
    produces commands

zlay/backends/renderer/opengl/
    renders commands with OpenGL

zlay/backends/renderer/vulkan/
    renders commands with Vulkan

zlay/backends/renderer/raylib/
    renders commands with raylib

zlay/backends/renderer/sokol/
    renders commands with sokol

zlay/backends/renderer/imgui/
    renders commands with ImGui
```

Canvas should not include:

```c
#include <GL/gl.h>
#include <vulkan/vulkan.h>
#include <raylib.h>
#include <sokol_gfx.h>
#include <imgui.h>
```

Those includes belong to renderer backends or drivers.

---

## 15. Canvas and Driver Relationship

Drivers are below renderer backends.

Canvas should never call drivers directly.

Wrong:

```txt
Canvas
    ↓
GPU Driver
```

Correct:

```txt
Canvas
    ↓
Render Command List
    ↓
Renderer Backend
    ↓
GPU Driver
```

This keeps canvas portable.

---

## 16. Canvas and UI Widgets

UI widgets may use canvas internally.

Example:

```txt
ZLay_Button
    ↓
Canvas rounded rect
    ↓
Canvas text
    ↓
Render command list
```

Example:

```txt
ZLay_Slider
    ↓
Canvas track rect
    ↓
Canvas handle circle
    ↓
Render command list
```

Example:

```txt
ZLay_Timeline
    ↓
Canvas grid lines
    ↓
Canvas markers
    ↓
Canvas text labels
    ↓
Render command list
```

This makes canvas useful for:

```txt
- engine editor UI
- DAW timeline
- node graph
- IDE panels
- profiler
- viewport overlays
- shader UI
```

---

## 17. Canvas and DAW / Audio UI

Canvas is important for DAW-style interfaces.

DAW-related canvas usage:

```txt
- waveform drawing
- MIDI piano roll
- automation curves
- timeline grid
- playhead
- track lanes
- mixer meters
- plugin UI knobs
- EQ curves
- spectrum visualizer
```

Recommended future modules:

```txt
zlay/ui/timeline/
zlay/ui/waveform/
zlay/ui/node_graph/
zlay/ui/curve_editor/
```

These modules should use canvas.

---

## 18. Canvas and IDE UI

Canvas may also support IDE/editor UI.

IDE-related usage:

```txt
- text editor background
- minimap
- selection highlight
- diagnostic underline
- breakpoint gutter
- command palette background
- tab indicator
```

However, advanced text editing should not be fully implemented in canvas alone.

Canvas draws the visuals.

Text editor logic should live in a separate module.

---

## 19. Canvas and Engine Viewport

Canvas can be used for overlays on top of a 3D viewport.

Viewport overlay usage:

```txt
- transform gizmo 2D overlay
- selection rectangle
- bounding box outline
- viewport labels
- grid overlay
- FPS/profiler HUD
- camera name label
- debug draw
```

Flow:

```txt
3D Renderer
    ↓
Viewport Image / Framebuffer
    ↓
Canvas Overlay
    ↓
Final Renderer Backend
```

---

## 20. Initial Canvas API Proposal

File:

```txt
zlay/renderer/canvas/zlay_canvas.h
```

Example header:

```c
#ifndef ZLAY_CANVAS_H
#define ZLAY_CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct ZLay_Canvas ZLay_Canvas;
typedef struct ZLay_RenderCommandList ZLay_RenderCommandList;

typedef struct ZLay_CanvasColor {
    float r;
    float g;
    float b;
    float a;
} ZLay_CanvasColor;

typedef struct ZLay_CanvasRect {
    float x;
    float y;
    float w;
    float h;
} ZLay_CanvasRect;

typedef struct ZLay_CanvasPaint {
    ZLay_CanvasColor color;
} ZLay_CanvasPaint;

typedef struct ZLay_CanvasStroke {
    ZLay_CanvasColor color;
    float width;
} ZLay_CanvasStroke;

void ZLay_CanvasBegin(ZLay_Canvas* canvas);
void ZLay_CanvasEnd(ZLay_Canvas* canvas);

void ZLay_CanvasRect(
    ZLay_Canvas* canvas,
    float x,
    float y,
    float w,
    float h,
    ZLay_CanvasPaint paint
);

void ZLay_CanvasRoundedRect(
    ZLay_Canvas* canvas,
    float x,
    float y,
    float w,
    float h,
    float radius,
    ZLay_CanvasPaint paint
);

void ZLay_CanvasLine(
    ZLay_Canvas* canvas,
    float x1,
    float y1,
    float x2,
    float y2,
    ZLay_CanvasStroke stroke
);

void ZLay_CanvasText(
    ZLay_Canvas* canvas,
    float x,
    float y,
    const char* text,
    ZLay_CanvasPaint paint
);

void ZLay_CanvasPushClipRect(
    ZLay_Canvas* canvas,
    float x,
    float y,
    float w,
    float h
);

void ZLay_CanvasPopClip(ZLay_Canvas* canvas);

ZLay_RenderCommandList* ZLay_CanvasGetRenderCommands(ZLay_Canvas* canvas);

#ifdef __cplusplus
}
#endif

#endif
```

---

## 21. Initial Canvas Implementation Strategy

Phase 1 should be simple.

### Phase 1 Features

```txt
- rect
- rounded rect
- line
- text placeholder
- image placeholder
- clip rect
- render command output
```

### Phase 1 Backend Targets

```txt
- raylib backend first for quick visual test
- OpenGL backend second
- ImGui backend third for debug
```

Reason:

```txt
raylib = fastest to see visual result
OpenGL = real engine path
ImGui = debug/prototype bridge
```

---

## 22. Canvas Render Command Mapping

Canvas command to render command:

```txt
CanvasRect
    → ZLAY_RENDER_COMMAND_RECT

CanvasRoundedRect
    → ZLAY_RENDER_COMMAND_ROUNDED_RECT

CanvasText
    → ZLAY_RENDER_COMMAND_TEXT

CanvasImage
    → ZLAY_RENDER_COMMAND_IMAGE

CanvasPushClipRect
    → ZLAY_RENDER_COMMAND_SCISSOR_BEGIN

CanvasPopClip
    → ZLAY_RENDER_COMMAND_SCISSOR_END
```

---

## 23. Backend Mapping

### OpenGL

```txt
ZLAY_RENDER_COMMAND_RECT
    → batch quad
    → upload vertex buffer
    → draw indexed triangles

ZLAY_RENDER_COMMAND_TEXT
    → glyph atlas
    → textured quads

ZLAY_RENDER_COMMAND_SCISSOR_BEGIN
    → glEnable(GL_SCISSOR_TEST)
    → glScissor(...)
```

### Vulkan

```txt
ZLAY_RENDER_COMMAND_RECT
    → batch quad
    → vertex/index buffer
    → vkCmdDrawIndexed

ZLAY_RENDER_COMMAND_TEXT
    → descriptor font atlas
    → vkCmdDrawIndexed

ZLAY_RENDER_COMMAND_SCISSOR_BEGIN
    → vkCmdSetScissor
```

### raylib

```txt
ZLAY_RENDER_COMMAND_RECT
    → DrawRectangleRec

ZLAY_RENDER_COMMAND_ROUNDED_RECT
    → DrawRectangleRounded

ZLAY_RENDER_COMMAND_TEXT
    → DrawTextEx

ZLAY_RENDER_COMMAND_SCISSOR_BEGIN
    → BeginScissorMode
```

### ImGui

```txt
ZLAY_RENDER_COMMAND_RECT
    → ImDrawList::AddRectFilled

ZLAY_RENDER_COMMAND_TEXT
    → ImDrawList::AddText

ZLAY_RENDER_COMMAND_SCISSOR_BEGIN
    → ImDrawList::PushClipRect
```

### sokol

```txt
ZLAY_RENDER_COMMAND_RECT
    → sg_apply_pipeline
    → sg_apply_bindings
    → sg_draw
```

---

## 24. Naming Convention

Use this pattern:

```txt
zlay_canvas_*.h
zlay_canvas_*.c
```

Recommended names:

```txt
zlay_canvas.h
zlay_canvas.c

zlay_canvas_command.h
zlay_canvas_command.c

zlay_canvas_paint.h
zlay_canvas_paint.c

zlay_canvas_path.h
zlay_canvas_path.c

zlay_canvas_text.h
zlay_canvas_text.c

zlay_canvas_image.h
zlay_canvas_image.c

zlay_canvas_clip.h
zlay_canvas_clip.c

zlay_canvas_transform.h
zlay_canvas_transform.c
```

Avoid:

```txt
zlay_canvas_backend.c
zlay_canvas_driver.c
```

Canvas is not backend or driver.

---

## 25. CMake Options

Recommended options:

```cmake
option(ZLAY_ENABLE_CANVAS "Enable Zlay Canvas rendering abstraction" ON)
option(ZLAY_ENABLE_CANVAS_PATH "Enable Canvas path drawing" ON)
option(ZLAY_ENABLE_CANVAS_TEXT "Enable Canvas text drawing" ON)
option(ZLAY_ENABLE_CANVAS_IMAGE "Enable Canvas image drawing" ON)
option(ZLAY_ENABLE_CANVAS_GRADIENT "Enable Canvas gradient drawing" OFF)
```

Canvas should be enabled by default because it is part of renderer abstraction.

---

## 26. Dependency Rules

Allowed:

```txt
zlay/renderer/canvas → zlay/renderer
zlay/renderer/canvas → zlay/core
zlay/ui              → zlay/renderer/canvas
zlay/backends        → zlay/renderer/canvas output
```

Forbidden:

```txt
zlay/renderer/canvas → OpenGL
zlay/renderer/canvas → Vulkan
zlay/renderer/canvas → raylib
zlay/renderer/canvas → sokol
zlay/renderer/canvas → ImGui
zlay/renderer/canvas → GLFW
```

Canvas must be renderer-agnostic.

---

## 27. AI Agent Implementation Prompt

Use this prompt for AI agents:

```txt
Implement a Zlay Canvas system.

Rules:

1. Place canvas under:
   zlay/renderer/canvas/

2. Do not place canvas under:
   zlay/backends/
   zlay/drivers/
   third_party/

3. Canvas is a Zlay-owned 2D drawing abstraction.

4. Canvas must not include:
   OpenGL headers
   Vulkan headers
   raylib headers
   sokol headers
   ImGui headers
   GLFW headers

5. Canvas should output ZLay_RenderCommandList or internal ZLay_CanvasCommandList.

6. Implement initial support for:
   - rect
   - rounded rect
   - line
   - text placeholder
   - image placeholder
   - clip rect

7. Use files:
   zlay_canvas.h
   zlay_canvas.c
   zlay_canvas_command.h
   zlay_canvas_command.c
   zlay_canvas_paint.h
   zlay_canvas_paint.c
   zlay_canvas_clip.h
   zlay_canvas_clip.c

8. Do not call GPU APIs from canvas.

9. Renderer backends are responsible for converting canvas/render commands to:
   - OpenGL
   - Vulkan
   - raylib
   - sokol
   - ImGui

10. Keep the public API under ZLay_* names.

11. Keep canvas renderer-agnostic.

12. Add CMake option:
    ZLAY_ENABLE_CANVAS
```

---

## 28. Roadmap

### Phase 1

```txt
- create zlay/renderer/canvas/
- add canvas begin/end
- add rect
- add rounded rect
- add line
- add clip rect
- emit ZLay_RenderCommandList
```

### Phase 2

```txt
- add text drawing
- add font handle abstraction
- add image handle abstraction
- add color/paint system
```

### Phase 3

```txt
- add path API
- add simple tessellation
- add stroke/fill separation
```

### Phase 4

```txt
- add gradient
- add transform stack
- add cached draw list
```

### Phase 5

```txt
- optimize batching
- add GPU-friendly command conversion
- add renderer backend conformance tests
```

---

## 29. Final Architecture Summary

```txt
Canvas location:
zlay/renderer/canvas/

Canvas role:
Zlay-owned 2D drawing abstraction

Canvas output:
ZLay_RenderCommandList

Renderer backend role:
Draw canvas/render commands using OpenGL, Vulkan, raylib, sokol, ImGui, etc.

Driver role:
Low-level GPU/API wrapper

Do not put canvas inside:
- backend
- driver
- third_party
```

Final rule:

```txt
Canvas draws in Zlay language.
Renderer backend translates it.
Driver executes low-level API commands.
```
