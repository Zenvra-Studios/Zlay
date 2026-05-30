Ini model infrastruktur drivers/gpu yang cocok buat ZLay/Zenvra: driver cuma nerima ZLay_RenderCommandList, lalu translate ke OpenGL/Vulkan. Core ZLay lu memang sudah output command list lewat ZLay_BeginLayout() → ZLay_EndLayout(), jadi driver jangan tahu soal Box, Text, parser, atau widget.

# ZLay Driver Infrastructure

## Goal

Driver layer bertugas menghubungkan:

```txt
ZLay_RenderCommandList
↓
Renderer backend
↓
GPU API
↓
Present ke window

Driver tidak mengurus:

❌ layout
❌ styling
❌ widget
❌ parser
❌ state UI

Driver hanya mengurus:

✅ GPU context
✅ swapchain/framebuffer
✅ shader
✅ buffer
✅ texture/font atlas
✅ batching
✅ frame pacing
✅ present
✅ low latency
Folder Structure
zlay/drivers/gpu/
├── zlay_driver.h
├── zlay_driver.c
├── zlay_backend.h
├── zlay_surface.h
├── zlay_surface.c
├── zlay_frame.h
├── zlay_frame.c
├── zlay_gpu.h
├── zlay_gpu.c
├── zlay_pipeline.h
├── zlay_pipeline.c
├── zlay_batch.h
├── zlay_batch.c
├── zlay_present.h
├── zlay_present.c
│
├── opengl/
│   ├── zlay_gl_driver.h
│   ├── zlay_gl_driver.c
│   ├── zlay_gl_context.c
│   ├── zlay_gl_pipeline.c
│   ├── zlay_gl_buffer.c
│   ├── zlay_gl_texture.c
│   └── zlay_gl_present.c
│
└── vulkan/
    ├── zlay_vk_driver.h
    ├── zlay_vk_driver.c
    ├── zlay_vk_instance.c
    ├── zlay_vk_device.c
    ├── zlay_vk_swapchain.c
    ├── zlay_vk_pipeline.c
    ├── zlay_vk_buffer.c
    ├── zlay_vk_texture.c
    ├── zlay_vk_command.c
    └── zlay_vk_present.c

Minimum dulu:

zlay/drivers/gpu/
├── zlay_driver.h
├── zlay_driver.c
├── zlay_backend.h
├── zlay_frame.h
├── zlay_batch.h
└── opengl/
    ├── zlay_gl_driver.h
    └── zlay_gl_driver.c
Driver Flow
OS create window
↓
Driver create GPU context / swapchain
↓
ZLay layout menghasilkan render command list
↓
Driver begin frame
↓
Driver convert command list jadi batch
↓
Driver upload vertex/index buffer
↓
Driver submit draw call
↓
Driver present

Runtime flow:

while (running) {
    zlay_os_poll_events();

    ZLay_BeginLayout(ctx);

    // user UI / parser UI
    // ZLAY(...)
    // ZLAY_TEXT(...)

    ZLay_RenderCommandList cmds = ZLay_EndLayout(ctx);

    ZLay_DriverBeginFrame(driver);
    ZLay_DriverSubmit(driver, &cmds);
    ZLay_DriverEndFrame(driver);
    ZLay_DriverPresent(driver);
}
Public Driver API
zlay_backend.h
typedef enum ZLay_Backend {
    ZLAY_BACKEND_NONE = 0,
    ZLAY_BACKEND_OPENGL = 1,
    ZLAY_BACKEND_VULKAN = 2,
    ZLAY_BACKEND_METAL = 3,
    ZLAY_BACKEND_D3D11 = 4,
    ZLAY_BACKEND_D3D12 = 5
} ZLay_Backend;
zlay_driver.h
typedef struct ZLay_Driver ZLay_Driver;

typedef struct ZLay_DriverDesc {
    ZLay_Backend backend;

    void* native_window;
    void* native_display;

    uint32_t width;
    uint32_t height;

    bool vsync;
    bool low_latency;
    uint32_t frames_in_flight;
} ZLay_DriverDesc;

typedef struct ZLay_DriverStats {
    float cpu_frame_ms;
    float gpu_frame_ms;
    float present_ms;

    uint32_t draw_calls;
    uint32_t vertex_count;
    uint32_t index_count;

    uint32_t frame_index;
} ZLay_DriverStats;

ZLAY_API bool ZLay_DriverCreate(
    ZLay_Driver** out_driver,
    const ZLay_DriverDesc* desc
);

ZLAY_API void ZLay_DriverDestroy(
    ZLay_Driver* driver
);

ZLAY_API void ZLay_DriverResize(
    ZLay_Driver* driver,
    uint32_t width,
    uint32_t height
);

ZLAY_API void ZLay_DriverBeginFrame(
    ZLay_Driver* driver
);

ZLAY_API void ZLay_DriverSubmit(
    ZLay_Driver* driver,
    const ZLay_RenderCommandList* commands
);

ZLAY_API void ZLay_DriverEndFrame(
    ZLay_Driver* driver
);

ZLAY_API void ZLay_DriverPresent(
    ZLay_Driver* driver
);

ZLAY_API ZLay_DriverStats ZLay_DriverGetStats(
    ZLay_Driver* driver
);
Internal Driver Object
typedef struct ZLay_DriverAPI {
    bool (*create)(ZLay_Driver* driver, const ZLay_DriverDesc* desc);
    void (*destroy)(ZLay_Driver* driver);

    void (*resize)(ZLay_Driver* driver, uint32_t width, uint32_t height);

    void (*begin_frame)(ZLay_Driver* driver);
    void (*submit)(ZLay_Driver* driver, const ZLay_RenderCommandList* commands);
    void (*end_frame)(ZLay_Driver* driver);
    void (*present)(ZLay_Driver* driver);
} ZLay_DriverAPI;

struct ZLay_Driver {
    ZLay_Backend backend;
    ZLay_DriverDesc desc;
    ZLay_DriverStats stats;

    void* backend_data;
    ZLay_DriverAPI api;
};
Render Command Translation

Core ZLay sudah punya command type seperti:

RECT
TEXT
CLIP_BEGIN
CLIP_END

dan render command berisi bounds, color, radius, z-index, text, dll.

Driver harus translate:

ZLAY_CMD_RECT
↓
quad vertices
↓
batch
↓
draw indexed triangles
ZLAY_CMD_TEXT
↓
font atlas glyphs
↓
quad per glyph
↓
batch
↓
draw indexed triangles
ZLAY_CMD_CLIP_BEGIN
↓
scissor rect
ZLAY_CMD_CLIP_END
↓
restore previous scissor
Batching System
zlay_batch.h
typedef struct ZLay_Vertex {
    float x;
    float y;

    float u;
    float v;

    uint32_t color;

    float radius;
    float extra0;
    float extra1;
    float extra2;
} ZLay_Vertex;

typedef struct ZLay_Batch {
    ZLay_Vertex* vertices;
    uint32_t vertex_count;
    uint32_t vertex_capacity;

    uint32_t* indices;
    uint32_t index_count;
    uint32_t index_capacity;

    uint32_t draw_calls;
} ZLay_Batch;

API:

bool ZLay_BatchInit(
    ZLay_Batch* batch,
    uint32_t max_vertices,
    uint32_t max_indices
);

void ZLay_BatchReset(
    ZLay_Batch* batch
);

void ZLay_BatchPushRect(
    ZLay_Batch* batch,
    ZLay_Rect rect,
    ZLay_Color color,
    float radius
);

void ZLay_BatchPushText(
    ZLay_Batch* batch,
    ZLay_Rect rect,
    ZLay_String text,
    ZLay_Color color
);
Frame Infrastructure
zlay_frame.h
typedef struct ZLay_FrameResources {
    uint32_t frame_index;

    void* vertex_buffer;
    void* index_buffer;
    void* uniform_buffer;

    void* command_buffer;
    void* fence;
    void* image_available;
    void* render_finished;
} ZLay_FrameResources;

Untuk low latency:

frames_in_flight = 1 atau 2
1 = latency paling rendah, tapi lebih gampang stutter kalau GPU lambat
2 = sweet spot
3 = lebih smooth tapi latency lebih tinggi

Saran:

.frames_in_flight = desc.low_latency ? 1 : 2;
Low Latency Strategy
1. Jangan Triple Buffer Default

Untuk UI/editor, default:

frames_in_flight = 2

Kalau mode ultra-low-latency:

frames_in_flight = 1
2. Jangan Block CPU Terlalu Awal

Buruk:

begin frame
↓
wait GPU
↓
build UI

Lebih baik:

poll input
↓
build UI
↓
wait only before reusing frame resource
↓
submit
↓
present
3. Input Harus Diproses Sedekat Mungkin ke Render

Flow low latency:

poll input
↓
update UI state
↓
layout
↓
build command list
↓
submit GPU
↓
present

Jangan:

poll input
↓
sleep
↓
layout
↓
render
4. Gunakan Dirty Flag

Jangan layout ulang semua kalau tidak berubah.

typedef enum ZLay_DirtyFlags {
    ZLAY_DIRTY_NONE   = 0,
    ZLAY_DIRTY_STYLE  = 1 << 0,
    ZLAY_DIRTY_LAYOUT = 1 << 1,
    ZLAY_DIRTY_TEXT   = 1 << 2,
    ZLAY_DIRTY_PAINT  = 1 << 3
} ZLay_DirtyFlags;

Rule:

text berubah     -> TEXT + LAYOUT + PAINT
warna berubah    -> PAINT
padding berubah  -> LAYOUT + PAINT
window resize    -> LAYOUT + PAINT
hover berubah    -> PAINT
5. Persistent Mapped Buffer

Untuk OpenGL/Vulkan, hindari alokasi buffer tiap frame.

Buruk:

malloc vertex tiap frame
glBufferData tiap command

Bagus:

ring buffer
persistent mapped buffer
upload sekali per frame
draw batched
6. Command Sorting

Sort berdasarkan:

z_index
texture
clip
pipeline

Tapi hati-hati: UI butuh order stabil.

Minimal awal:

stable order + z_index

Jangan aggressive sorting dulu karena clip/text bisa kacau.

7. Batched Draw Call

Target awal:

1 draw call untuk rect
1 draw call untuk text

Jangan satu rect satu draw call.

OpenGL Backend Flow
create context
↓
compile shader
↓
create VAO/VBO/IBO
↓
create font atlas texture
↓
begin frame
↓
clear / setup viewport
↓
convert command list to batch
↓
upload buffer
↓
draw
↓
swap buffers

OpenGL file minimal:

opengl/
├── zlay_gl_driver.h
├── zlay_gl_driver.c
├── zlay_gl_shader.c
├── zlay_gl_buffer.c
└── zlay_gl_texture.c
Vulkan Backend Flow
create instance
↓
create surface
↓
select physical device
↓
create logical device
↓
create swapchain
↓
create render pass / dynamic rendering
↓
create pipeline
↓
create frame resources
↓
begin frame
↓
acquire image
↓
record command buffer
↓
submit queue
↓
present queue

Vulkan file minimal:

vulkan/
├── zlay_vk_instance.c
├── zlay_vk_device.c
├── zlay_vk_swapchain.c
├── zlay_vk_pipeline.c
├── zlay_vk_buffer.c
├── zlay_vk_command.c
└── zlay_vk_present.c
Driver Does Not Own OS

Karena lu sudah punya os/, driver cukup terima native handle:

ZLay_DriverDesc desc = {
    .backend = ZLAY_BACKEND_VULKAN,
    .native_window = zlay_window_native_handle(window),
    .native_display = zlay_window_native_display(window),
    .width = 1280,
    .height = 720,
    .vsync = true,
    .low_latency = true,
    .frames_in_flight = 2
};

Jadi pembagiannya:

os/
  window, input, event, timer

drivers/gpu/
  GPU context, swapchain, present

renderer/
  command translation, batching

ui/layout/
  node tree, style, layout
Main Loop Low Latency
while (running) {
    ZLay_TimePoint frame_start = zlay_time_now();

    zlay_os_poll_events();

    ZLay_BeginLayout(ctx);

    build_ui(ctx);

    ZLay_RenderCommandList commands = ZLay_EndLayout(ctx);

    ZLay_DriverBeginFrame(driver);
    ZLay_DriverSubmit(driver, &commands);
    ZLay_DriverEndFrame(driver);
    ZLay_DriverPresent(driver);

    ZLay_DriverStats stats = ZLay_DriverGetStats(driver);
}

Untuk editor/game engine:

input harus sebelum layout
layout harus sebelum submit
present jangan nunggu terlalu lama
Optimization Priority
Phase 1
✅ OpenGL backend dulu
✅ single shader
✅ rect batching
✅ text batching
✅ scissor clip
✅ vsync on/off
Phase 2
✅ ring buffer
✅ persistent mapped buffer
✅ dirty flag
✅ frame stats
✅ font atlas cache
Phase 3
✅ Vulkan backend
✅ frames in flight
✅ timeline semaphore
✅ swapchain recreation
✅ GPU timestamp query
Phase 4
✅ partial repaint
✅ multi-thread command build
✅ async texture/font upload
✅ frame graph integration
Saran Final

Mulai dari ini dulu:

drivers/gpu/
├── zlay_driver.h
├── zlay_driver.c
├── zlay_backend.h
├── zlay_batch.h
├── zlay_batch.c
└── opengl/
    ├── zlay_gl_driver.h
    └── zlay_gl_driver.c

Jangan langsung Vulkan dulu. OpenGL lebih cepat buat validasi pipeline. Setelah command list + batching stabil, Vulkan tinggal jadi backend kedua.

Desain paling aman:

ZLay core produces commands
↓
driver batches commands
↓
backend uploads batch
↓
GPU draws
↓
present

Itu pondasi driver yang bersih, low-latency friendly, dan nanti gampang dioptimasi.
