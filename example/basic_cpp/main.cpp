#include <cstdio>
#include <cstring>

#include <driver/opengl/zlay_opengl.h>
#include <os/zlay_os.h>
#include <renderer/zlay_renderer.h>
#include <zlay_antialiasing.h>
#include <zlay.h>
#include <zlay_example.h>

int main() {
  ZLay_Config cfg = ZLay_ConfigDefault();

  const size_t mem_size = ZLay_MinMemorySize(cfg);
  ZLay_ExampleMemory perm = ZLay_ExampleCreateMemory(mem_size);
  ZLay_Context* ctx = ZLay_Initialize(perm.arena, cfg);
  if (!ctx) {
    std::fprintf(stderr, "ZLay: failed to initialize context\n");
    ZLay_ExampleDestroyMemory(&perm);
    return 1;
  }

  ZLay_OSFrameInfo frame{};
  frame.width = 1280;
  frame.height = 720;
  frame.dpi_scale = ZLay_OS_GetDpiScale(nullptr);
  frame.time_ns = ZLay_OS_NowNs();
  ZLay_OS_NewFrame(ctx, &frame);

  ZLay_BeginLayout(ctx);

  ZLay_OSStyleInfo osStyle = ZLay_OS_GetStyleInfo();
  ZLay_Style rootStyle = ZLay_ExampleRootStyle(osStyle);

  ZLAY(ctx, ZLAY_ID("Root"), ZLAY_BOX(.id = ZLAY_ID("Root"), .style = rootStyle)) {
    ZLay_Style sidebar = ZLay_StyleDefault();
    sidebar.width = ZLay_Px(280);
    sidebar.height = ZLay_Grow(1);
    sidebar.background = osStyle.panel_bg;
    sidebar.radius = osStyle.corner_radius;
    sidebar.padding_left = sidebar.padding_top = sidebar.padding_right = sidebar.padding_bottom = 16;
    sidebar.flex_direction = ZLAY_FLEX_COLUMN;
    sidebar.gap = 12;

    ZLAY(ctx, ZLAY_ID("Sidebar"), ZLAY_BOX(.id = ZLAY_ID("Sidebar"), .style = sidebar)) {
      ZLay_Style textStyle = ZLay_StyleDefault();
      textStyle.background = ZLay_Color{0, 0, 0, 0};
      ZLAY_TEXT(ctx, ZLAY_ID("Title"), ZLAY_STRING_LITERAL("Dashboard"), &textStyle);
    }
  }

  ZLay_ExampleMemory frameMem = ZLay_ExampleCreateMemory(16 * 1024);
  ZLay_OpenGLRenderer glBackend{};
  ZLay_OpenGLRendererInitInfo glInfo{};
  glInfo.frame_arena = frameMem.arena;
  if (!ZLay_OpenGLRenderer_Init(&glBackend, &glInfo)) {
    std::fprintf(stderr, "ZLay: failed to initialize OpenGL renderer adapter\n");
    ZLay_ExampleDestroyMemory(&frameMem);
    ZLay_ExampleDestroyMemory(&perm);
    return 1;
  }

  ZLay_Renderer renderer = ZLay_OpenGLRenderer_AsRenderer(&glBackend);
  ZLay_RendererFrameInfo renderFrame{};
  renderFrame.width = frame.width;
  renderFrame.height = frame.height;
  renderFrame.dpi_scale = frame.dpi_scale;
  renderFrame.clear_color = osStyle.window_bg;

  ZLay_RenderCommandList cmds = ZLay_EndLayout(ctx);
  ZLay_RendererBeginFrame(&renderer, &renderFrame);
  ZLay_RendererRenderCommands(&renderer, &cmds);
  ZLay_RendererEndFrame(&renderer);

  std::printf("ZLay: os=%s renderer=%s commands=%u vertices=%u indices=%u draw_calls=%u\n",
              ZLay_OS_BackendName(),
              ZLay_RendererBackendName(renderer.backend),
              renderer.last_stats.command_count,
              renderer.last_stats.vertex_count,
              renderer.last_stats.index_count,
              renderer.last_stats.draw_call_count);
  std::printf("Shader: rect %s/%s source-bytes=%zu\n",
              ZLay_ShaderLanguageName(glBackend.rect_fragment_shader.language),
              ZLay_ShaderStageName(glBackend.rect_fragment_shader.stage),
              glBackend.rect_fragment_shader.source ? std::strlen(glBackend.rect_fragment_shader.source) : 0u);
  for (uint32_t i = 0; i < cmds.count; ++i) {
    const ZLay_RenderCommand& c = cmds.commands[i];
    std::printf("  #%u type=%u x=%.1f y=%.1f w=%.1f h=%.1f z=%d\n",
                i, (unsigned)c.type, c.bounds.x, c.bounds.y, c.bounds.width, c.bounds.height, (int)c.z_index);
  }

  ZLay_RendererShutdown(&renderer);
  ZLay_ExampleDestroyMemory(&frameMem);
  ZLay_ExampleDestroyMemory(&perm);
  return 0;
}
