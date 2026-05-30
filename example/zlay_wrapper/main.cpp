#include <cstdio>
#include <cstring>
#include <chrono>
#include <string>
#include <thread>

#include <backends/renderer/opengl/zlay_opengl_renderer_backend.h>
#include <os/zlay_os.h>
#include <renderer/zlay_renderer.h>
#include "../platform/Window.h"
#include <zlay/zlay.hpp>
#include <zlay_antialiasing.h>
#include <zlay_example.h>
#include <zlay_shell.h>

static void Metric(zlay::Context& ctx,
                   const char* id,
                   const char* label,
                   const char* value,
                   const ZLay_Theme& theme) {
  zlay::Column(
      ctx,
      id,
      zlay::surfaceStyle(theme)
          .width(zlay::grow())
      .height(zlay::px(116))
      .gap(theme.space_2),
      [&] {
        zlay::Text(ctx, std::string(id) + "Label", label, zlay::TextStyle(&theme).role(ZLAY_LABEL_CAPTION));
        zlay::Text(ctx, std::string(id) + "Value", value, zlay::TextStyle(&theme).role(ZLAY_LABEL_TITLE).size(ZLAY_COMPONENT_SIZE_LG));
      });
}

int main() {
  ZLay_ShellCapabilities shell = ZLay_ShellDetect(stdout);
  ZLay_ShellStyle okStyle = ZLay_ShellStyleStrong(ZLAY_SHELL_TONE_SUCCESS);
  ZLay_ShellStyle warnStyle = ZLay_ShellStyleStrong(ZLAY_SHELL_TONE_WARNING);

  ZLay_ShellPrintRule(stdout, "ZLay wrapper C++ example", ZLAY_SHELL_TONE_ACCENT);
  ZLay_ShellPrintCapabilitySummary(stdout, &shell);

  ZLay_Config config = ZLay_ConfigDefault();
  ZLay_ExampleMemory perm = ZLay_ExampleCreateMemory(ZLay_MinMemorySize(config));
  ZLay_Context* raw = ZLay_Initialize(perm.arena, config);
  if (raw == nullptr) {
    ZLay_ShellPrintfStyled(stderr, warnStyle, "ZLay wrapper C++: failed to initialize context\n");
    ZLay_ExampleDestroyMemory(&perm);
    return 1;
  }

  ZLay_OSStyleInfo osStyle = ZLay_OS_GetStyleInfo();
  ZLay_Theme theme = osStyle.mode == ZLAY_OS_THEME_DARK ? ZLay_ThemeDark() : ZLay_ThemeLight();
  theme.canvas = osStyle.window_bg;
  theme.surface = osStyle.panel_bg;
  theme.surface_raised = ZLay_ColorMix(osStyle.panel_bg, osStyle.text, 0.04f);
  theme.text = osStyle.text;
  theme.text_muted = ZLay_ColorMix(osStyle.text, osStyle.window_bg, 0.35f);
  theme.primary = osStyle.accent;
  theme.primary_hover = ZLay_ColorMix(osStyle.accent, theme.text, 0.12f);
  theme.primary_active = ZLay_ColorMix(osStyle.accent, theme.text_inverse, 0.18f);

  zlay::platform::Window window;
  const bool windowReady = window.open({
      1280,
      720,
      "ZLay Wrapper C++",
      ZLay_DefaultFontFamily(),
  });
  if (windowReady) {
    ZLay_SetMeasureTextFunction(raw, zlay::platform::Window::measureTextCallback, &window);
  }

  zlay::App app(raw);
  ZLay_Rect docsBounds{};
  ZLay_Rect createBounds{};
  ZLay_Rect overviewBounds{};
  ZLay_Rect projectsBounds{};
  ZLay_Rect settingsBounds{};
  ZLay_Rect openBounds{};
  ZLay_Rect learnBounds{};
  bool docsTracked = false;
  bool createTracked = false;
  bool overviewTracked = false;
  bool projectsTracked = false;
  bool settingsTracked = false;
  bool openTracked = false;
  bool learnTracked = false;
  uint32_t docsState = ZLAY_COMPONENT_STATE_NONE;
  uint32_t createState = ZLAY_COMPONENT_STATE_NONE;
  uint32_t overviewState = ZLAY_COMPONENT_STATE_NONE;
  uint32_t projectsState = ZLAY_COMPONENT_STATE_NONE;
  uint32_t settingsState = ZLAY_COMPONENT_STATE_NONE;
  uint32_t openState = ZLAY_COMPONENT_STATE_NONE;
  uint32_t learnState = ZLAY_COMPONENT_STATE_NONE;

  auto buildFrame = [&](int width, int height) {
    ZLay_OSFrameInfo frame{};
    frame.width = width;
    frame.height = height;
    frame.dpi_scale = ZLay_OS_GetDpiScale(nullptr);
    frame.time_ns = ZLay_OS_NowNs();
    ZLay_OS_NewFrame(raw, &frame);

    app.setSize(static_cast<float>(width), static_cast<float>(height));

    return app.frame([&](zlay::Context& ctx) {
    zlay::Column(
        ctx,
        "Root",
        zlay::Style()
            .width(zlay::px(static_cast<float>(width)))
            .height(zlay::px(static_cast<float>(height)))
            .padding(theme.space_6)
            .gap(theme.space_5)
            .background(theme.canvas)
            .textColor(theme.text),
        [&] {
          zlay::Row(
              ctx,
              "AppBar",
              zlay::Style().width(zlay::grow()).height(zlay::px(52)).gap(theme.space_3),
              [&] {
                zlay::Text(ctx, "Brand", "ZLay C++", zlay::TextStyle(&theme).role(ZLAY_LABEL_TITLE).size(ZLAY_COMPONENT_SIZE_LG));
                zlay::Fill(ctx, "AppBarFill");
                zlay::Input(ctx, "Search", "", "Search", zlay::InputStyle(&theme).variant(ZLAY_INPUT_OUTLINED));
                zlay::Button(ctx, "Docs", "Docs", zlay::ButtonStyle(&theme).variant(ZLAY_BUTTON_GHOST).state(docsState));
                zlay::Button(ctx, "Create", "Create", zlay::ButtonStyle(&theme).variant(ZLAY_BUTTON_PRIMARY).state(createState));
              });

          zlay::Row(
              ctx,
              "Content",
              zlay::Style().width(zlay::grow()).height(zlay::grow()).gap(theme.space_4),
              [&] {
                zlay::Column(
                    ctx,
                    "Sidebar",
                    zlay::surfaceStyle(theme, ZLAY_SURFACE_PANEL)
                        .width(zlay::px(260))
                        .height(zlay::grow())
                        .gap(theme.space_3),
                    [&] {
                      zlay::Text(ctx, "MenuLabel", "WORKSPACE", zlay::TextStyle(&theme).role(ZLAY_LABEL_CAPTION));
                      zlay::Button(ctx,
                                   "Overview",
                                   "Overview",
                                   zlay::ButtonStyle(&theme)
                                       .variant(ZLAY_BUTTON_SECONDARY)
                                       .state(ZLAY_COMPONENT_STATE_SELECTED | overviewState));
                      zlay::Text(ctx,
                                 "Projects",
                                 "Projects",
                                 zlay::TextStyle(&theme)
                                     .role(ZLAY_LABEL_BODY)
                                     .color((projectsState & ZLAY_COMPONENT_STATE_HOVERED) ? theme.primary_hover : theme.primary));
                      zlay::Button(ctx, "Settings", "Settings", zlay::ButtonStyle(&theme).variant(ZLAY_BUTTON_GHOST).state(settingsState));
                      zlay::Divider(ctx, "SidebarDivider", theme);
                      zlay::Text(ctx, "Aside", "Tree UI, plain C++.", zlay::TextStyle(&theme).role(ZLAY_LABEL_SUBTITLE));
                    });

                zlay::Column(
                    ctx,
                    "Main",
                    zlay::Style().width(zlay::grow()).height(zlay::grow()).gap(theme.space_4),
                    [&] {
                      zlay::Row(
                          ctx,
                          "Metrics",
                          zlay::Style().width(zlay::grow()).height(zlay::px(116)).gap(theme.space_4),
                          [&] {
                            Metric(ctx, "ViewsCard", "VIEWS", "24.8K", theme);
                            Metric(ctx, "UsersCard", "USERS", "8,402", theme);
                            Metric(ctx, "HealthCard", "HEALTH", "99.9%", theme);
                          });

                      zlay::Column(
                          ctx,
                          "Hero",
                          zlay::surfaceStyle(theme)
                              .width(zlay::grow())
                              .height(zlay::grow())
                              .gap(theme.space_3),
                          [&] {
                            zlay::Text(ctx, "HeroBadge", "CPP WRAPPER", zlay::TextStyle(&theme).role(ZLAY_LABEL_BADGE));
                            zlay::Text(ctx,
                                       "HeroTitle",
                                       "Flutter-like widget trees on ZLay",
                                       zlay::TextStyle(&theme).role(ZLAY_LABEL_TITLE).size(ZLAY_COMPONENT_SIZE_LG));
                            zlay::Text(ctx,
                                       "HeroCopy",
                                       "Box, Row, Column, Card, Text, Button, and Input are small C++ wrappers over native ZLay.",
                                       zlay::TextStyle(&theme).role(ZLAY_LABEL_SUBTITLE));
                            zlay::Fill(ctx, "HeroFill");
                            zlay::Row(
                                ctx,
                                "HeroActions",
                                zlay::Style().width(zlay::grow()).height(zlay::px(theme.control_lg)).gap(theme.space_3),
                                [&] {
                                  zlay::Button(ctx, "Open", "Open app", zlay::ButtonStyle(&theme).variant(ZLAY_BUTTON_PRIMARY).state(openState));
                                  zlay::Button(ctx, "Learn", "Learn", zlay::ButtonStyle(&theme).variant(ZLAY_BUTTON_GHOST).state(learnState));
                                });
                          });
                    });
              });
        });
    });
  };

  auto updateInteractiveBounds = [&]() {
    zlay::Context ctx(raw);
    docsTracked = zlay::findBounds(ctx, "Docs", &docsBounds);
    createTracked = zlay::findBounds(ctx, "Create", &createBounds);
    overviewTracked = zlay::findBounds(ctx, "Overview", &overviewBounds);
    projectsTracked = zlay::findBounds(ctx, "Projects", &projectsBounds);
    settingsTracked = zlay::findBounds(ctx, "Settings", &settingsBounds);
    openTracked = zlay::findBounds(ctx, "Open", &openBounds);
    learnTracked = zlay::findBounds(ctx, "Learn", &learnBounds);
  };

  if (windowReady) {
    while (window.pollEvents()) {
      const zlay::platform::PointerState pointer = window.pointer();
      docsState = ZLay_ComponentStateFromPointer(&pointer, docsBounds, docsTracked);
      createState = ZLay_ComponentStateFromPointer(&pointer, createBounds, createTracked);
      overviewState = ZLay_ComponentStateFromPointer(&pointer, overviewBounds, overviewTracked);
      projectsState = ZLay_ComponentStateFromPointer(&pointer, projectsBounds, projectsTracked);
      settingsState = ZLay_ComponentStateFromPointer(&pointer, settingsBounds, settingsTracked);
      openState = ZLay_ComponentStateFromPointer(&pointer, openBounds, openTracked);
      learnState = ZLay_ComponentStateFromPointer(&pointer, learnBounds, learnTracked);
      ZLay_RenderCommandList windowCommands = buildFrame(window.width(), window.height());
      window.setPointerCursor(pointer.inside ? zlay::cursorAt(windowCommands, pointer.x, pointer.y) : ZLAY_POINTER_CURSOR_DEFAULT);
      window.render(windowCommands);
      updateInteractiveBounds();
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    ZLay_ExampleDestroyMemory(&perm);
    return 0;
  }

  ZLay_RenderCommandList commands = buildFrame(1280, 720);

  ZLay_ExampleMemory frameMem = ZLay_ExampleCreateMemory(16u * 1024u);
  ZLay_OpenGLRenderer glBackend{};
  ZLay_OpenGLRendererInitInfo glInfo{};
  glInfo.frame_arena = frameMem.arena;
  if (!ZLay_OpenGLRenderer_Init(&glBackend, &glInfo)) {
    ZLay_ShellPrintfStyled(stderr, warnStyle, "ZLay wrapper C++: failed to initialize OpenGL renderer adapter\n");
    ZLay_ExampleDestroyMemory(&frameMem);
    ZLay_ExampleDestroyMemory(&perm);
    return 1;
  }

  ZLay_Renderer renderer = ZLay_OpenGLRenderer_AsRenderer(&glBackend);
  ZLay_RendererFrameInfo renderFrame{};
  renderFrame.width = 1280;
  renderFrame.height = 720;
  renderFrame.dpi_scale = ZLay_OS_GetDpiScale(nullptr);
  renderFrame.clear_color = theme.canvas;

  ZLay_RendererBeginFrame(&renderer, &renderFrame);
  ZLay_RendererRenderCommands(&renderer, &commands);
  ZLay_RendererEndFrame(&renderer);

  ZLay_ShellWriteStyled(stdout, okStyle, "wrapper-cpp");
  std::printf(": os=%s renderer=%s commands=%u vertices=%u indices=%u draw_calls=%u\n",
              ZLay_OS_BackendName(),
              ZLay_RendererBackendName(renderer.backend),
              renderer.last_stats.command_count,
              renderer.last_stats.vertex_count,
              renderer.last_stats.index_count,
              renderer.last_stats.draw_call_count);

  ZLay_ShellWriteStyled(stdout, okStyle, "shader");
  std::printf(": rect %s/%s source-bytes=%zu\n",
              ZLay_ShaderLanguageName(glBackend.rect_fragment_shader.language),
              ZLay_ShaderStageName(glBackend.rect_fragment_shader.stage),
              glBackend.rect_fragment_shader.source != nullptr ? std::strlen(glBackend.rect_fragment_shader.source) : 0u);

  ZLay_RendererShutdown(&renderer);
  ZLay_ExampleDestroyMemory(&frameMem);
  ZLay_ExampleDestroyMemory(&perm);
  return 0;
}
