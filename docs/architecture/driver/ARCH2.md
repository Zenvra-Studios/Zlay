Gw akan susun ini sebagai arsitektur driver layer: mulai dari context, surface/swapchain, frame loop, command submit, sampai strategi low-latency. Fokusnya bukan OpenGL/Vulkan detail penuh dulu, tapi fondasi yang bisa lu implement bertahap.

Berikut infrastruktur driver layer yang cocok buat ZLay/Zenvra dari context sampai low-latency.

app
↓
os/window
↓
driver context
↓
surface/swapchain
↓
renderer command list
↓
gpu submit
↓
present

Core ZLay lu sudah output ZLay_RenderCommandList, jadi driver cukup menerima command list itu dan translate ke OpenGL/Vulkan.

Struktur Folder Driver
zlay/drivers/gpu/
├── zlay_driver.h
├── zlay_driver.c
├── zlay_backend.h
├── zlay_surface.h
├── zlay_surface.c
├── zlay_gpu.h
├── zlay_frame.h
├── zlay_present.h
│
├── opengl/
│   ├── zlay_gl_driver.c
│   ├── zlay_gl_context.c
│   ├── zlay_gl_pipeline.c
│   ├── zlay_gl_buffer.c
│   └── zlay_gl_texture.c
│
└── vulkan/
    ├── zlay_vk_driver.c
    ├── zlay_vk_instance.c
    ├── zlay_vk_device.c
    ├── zlay_vk_swapchain.c
    ├── zlay_vk_pipeline.c
    ├── zlay_vk_buffer.c
    ├── zlay_vk_command.c
    └── zlay_vk_sync.c
1. Backend Enum
typedef enum ZLay_Backend {
    ZLAY_BACKEND_NONE = 0,
    ZLAY_BACKEND_OPENGL,
    ZLAY_BACKEND_VULKAN
} ZLay_Backend;
2. Driver Desc

Ini data awal untuk bikin driver.

typedef struct ZLay_DriverDesc {
    ZLay_Backend backend;

    void* native_window;
    void* native_display;

    int width;
    int height;

    bool vsync;
    bool low_latency;
    uint32_t frames_in_flight;
} ZLay_DriverDesc;

Untuk Linux:

X11:
native_display = Display*
native_window  = Window

Wayland:
native_display = wl_display*
native_window  = wl_surface*

Win32:
native_window = HWND
3. Driver API Interface
typedef struct ZLay_Driver ZLay_Driver;

typedef struct ZLay_DriverAPI {
    bool (*init)(ZLay_Driver* driver, const ZLay_DriverDesc* desc);
    void (*shutdown)(ZLay_Driver* driver);

    void (*resize)(ZLay_Driver* driver, int width, int height);

    void (*begin_frame)(ZLay_Driver* driver);
    void (*submit)(ZLay_Driver* driver, ZLay_RenderCommandList commands);
    void (*end_frame)(ZLay_Driver* driver);
    void (*present)(ZLay_Driver* driver);
} ZLay_DriverAPI;

struct ZLay_Driver {
    ZLay_Backend backend;
    ZLay_DriverAPI api;
    void* impl;
};
4. Public Driver Function
bool ZLay_DriverCreate(
    ZLay_Driver* driver,
    const ZLay_DriverDesc* desc
);

void ZLay_DriverDestroy(
    ZLay_Driver* driver
);

Implementasi:

bool ZLay_DriverCreate(ZLay_Driver* driver, const ZLay_DriverDesc* desc)
{
    driver->backend = desc->backend;

    switch (desc->backend) {
        case ZLAY_BACKEND_OPENGL:
            return ZLay_GL_CreateDriver(driver, desc);

        case ZLAY_BACKEND_VULKAN:
            return ZLay_VK_CreateDriver(driver, desc);

        default:
            return false;
    }
}
5. Frame Flow

Runtime utama:

while (running) {
    zlay_os_poll_events();

    ZLay_BeginLayout(ctx);

    // UI declaration
    // ZLAY(...)
    // ZLAY_TEXT(...)

    ZLay_RenderCommandList commands = ZLay_EndLayout(ctx);

    driver.api.begin_frame(&driver);
    driver.api.submit(&driver, commands);
    driver.api.end_frame(&driver);
    driver.api.present(&driver);
}

Flow internal:

begin_frame
↓
acquire image / bind framebuffer
↓
clear / setup viewport
↓
submit draw commands
↓
flush command buffer
↓
present
6. Command Translation

ZLay punya command:

ZLAY_CMD_RECT
ZLAY_CMD_TEXT
ZLAY_CMD_CLIP_BEGIN
ZLAY_CMD_CLIP_END

Driver translate jadi GPU draw.

static void zlay_driver_submit_rect(
    ZLay_Driver* driver,
    const ZLay_RenderCommand* cmd
) {
    // rect bounds -> 2 triangles
    // color -> uniform/vertex color
    // radius -> shader uniform
}

Renderer backend jangan tahu Button, Box, TextInput.

Driver cuma tahu:

rect
text
image
clip
present
7. Low-Latency Design

Target low-latency:

input diterima
↓
layout dihitung
↓
command dibuat
↓
GPU submit
↓
present secepat mungkin

Yang perlu lu lakukan:

A. Batasi frames in flight

Untuk low latency:

desc.frames_in_flight = 1;

Untuk performa stabil:

desc.frames_in_flight = 2;

Jangan default 3 dulu, karena latency bisa lebih tinggi.

B. Immediate input sampling

Input jangan diproses telat.

poll event
↓
update input state
↓
build UI
↓
render

Jangan:

render dulu
↓
baru input
C. Dirty flag

Jangan layout ulang semua kalau tidak perlu.

typedef struct ZLay_DirtyState {
    bool layout_dirty;
    bool style_dirty;
    bool text_dirty;
    bool render_dirty;
} ZLay_DirtyState;

Flow:

input berubah → layout/render dirty
style berubah → style/layout/render dirty
text berubah → text/layout/render dirty
window resize → layout/render dirty
D. Persistent GPU buffer

Jangan alokasi buffer tiap frame.

Buruk:

malloc GPU buffer setiap frame
upload
free

Bagus:

create large buffer sekali
map/update per frame
ring buffer
typedef struct ZLay_GpuRingBuffer {
    void* mapped;
    size_t capacity;
    size_t offset;
} ZLay_GpuRingBuffer;
E. Command batching

Gabungkan draw command yang sama.

rect same pipeline
text same font atlas
image same texture

Jangan setiap rect langsung draw call kalau bisa dibatch.

Target awal:

1 batch rect
1 batch text
F. Avoid GPU sync stall

Hindari hal ini:

glFinish()
vkDeviceWaitIdle()
readback GPU data

Di frame loop normal, jangan panggil itu.

Pakai fence/semaphore.

8. OpenGL Backend Minimal

OpenGL lebih cepat buat prototyping.

init:
  create context
  load function pointer
  create shader
  create VAO/VBO
  create font texture

begin_frame:
  glViewport
  glClear

submit:
  convert commands to vertices
  upload vertex buffer
  glDrawArrays / glDrawElements

present:
  swap buffers

File:

opengl/
├── zlay_gl_context.c
├── zlay_gl_pipeline.c
├── zlay_gl_buffer.c
└── zlay_gl_driver.c
9. Vulkan Backend Minimal

Vulkan lebih cocok buat final engine.

init:
  create instance
  create surface
  pick physical device
  create logical device
  create swapchain
  create render pass
  create pipeline
  create command buffers
  create sync objects

begin_frame:
  acquire swapchain image
  begin command buffer
  begin render pass

submit:
  record draw commands

end_frame:
  end render pass
  end command buffer
  submit queue

present:
  queue present

File wajib:

vulkan/
├── zlay_vk_instance.c
├── zlay_vk_device.c
├── zlay_vk_swapchain.c
├── zlay_vk_pipeline.c
├── zlay_vk_command.c
├── zlay_vk_sync.c
└── zlay_vk_driver.c
10. Driver Data Internal
typedef struct ZLay_FrameState {
    uint32_t frame_index;
    double delta_time;
    double time;
} ZLay_FrameState;

typedef struct ZLay_Surface {
    int width;
    int height;
    void* native_window;
    void* native_display;
} ZLay_Surface;

typedef struct ZLay_GpuStats {
    uint32_t draw_calls;
    uint32_t rect_count;
    uint32_t text_count;
    uint32_t vertices;
} ZLay_GpuStats;
11. Final Flow Yang Lu Kejar
OS event
↓
ZLay input state
↓
ZLay layout
↓
ZLay render command list
↓
driver begin frame
↓
driver batch command
↓
GPU submit
↓
present
Saran Implementasi Bertahap

Urutan paling aman:

1. zlay_driver.h
2. backend enum
3. OpenGL backend dulu
4. render command translator
5. batching rect
6. font/text atlas
7. dirty flag
8. Vulkan backend
9. frame pacing + low latency mode

Jangan mulai dari Vulkan dulu kalau command list belum stabil. Stabilkan pipeline dengan OpenGL, baru Vulkan.
