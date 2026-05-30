#include <cstdio>
#include <cstring>

#ifndef ZLAY_LOG_ACTIVE_LEVEL
#  define ZLAY_LOG_ACTIVE_LEVEL 1 // ZLAY_LOG_LEVEL_DEBUG before zlay_logger.h defines it.
#endif

#include <backends/renderer/opengl/zlay_opengl_renderer_backend.h>
#include <renderer/zlay_renderer.h>
#include <os/zlay_os.h>
#include <zlay.h>
#include <zlay_antialiasing.h>
#include <zlay_ui.h>
#include <zlay_example.h>
#include <zlay_logger.h>
#include <zlay_shell.h>

int main() {
  ZLay_ShellCapabilities shell = ZLay_ShellDetect(stdout);

  ZLay_ShellPrintRule(stdout, "ZLay basic C++ example", ZLAY_SHELL_TONE_ACCENT);
  ZLay_ShellPrintCapabilitySummary(stdout, &shell);
  ZLay_ShellPrintBadge(stdout, "debug demo", ZLAY_SHELL_TONE_DEBUG);
  fputs(" logger=zlay-example-cpp level=debug\n", stdout);

  ZLay_LogSetName("zlay-example-cpp");
  ZLay_LogSetLevel((ZLay_LogLevel)ZLAY_LOG_LEVEL_DEBUG);
  ZLay_LogEnableFlags(ZLAY_LOG_FLAG_FUNCTION);
  if (!shell.ansi) {
    ZLay_LogDisableFlags(ZLAY_LOG_FLAG_COLOR);
  }
  ZLAY_DEBUG("logger ready: runtime-level=%s color-depth=%s",
             ZLay_LogLevelName(ZLay_LogGetLevel()),
             ZLay_ShellColorDepthName(shell.color_depth));

  ZLay_Config cfg = ZLay_ConfigDefault();

  const size_t mem_size = ZLay_MinMemorySize(cfg);
  ZLay_ExampleMemory perm = ZLay_ExampleCreateMemory(mem_size);
  ZLay_Context* ctx = ZLay_Initialize(perm.arena, cfg);
  if (!ctx) {
    ZLAY_ERROR("failed to initialize context");
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
  ZLay_Theme theme = osStyle.mode == ZLAY_OS_THEME_DARK ? ZLay_ThemeDark() : ZLay_ThemeLight();
  theme.canvas = osStyle.window_bg;
  theme.surface = osStyle.panel_bg;
  theme.text = osStyle.text;
  theme.primary = osStyle.accent;
  theme.primary_hover = ZLay_ColorMix(osStyle.accent, theme.text, 0.12f);
  theme.primary_active = ZLay_ColorMix(osStyle.accent, theme.text_inverse, 0.18f);
  theme.radius_md = osStyle.corner_radius;
  theme.radius_lg = osStyle.corner_radius + 6.0f;
  theme.control_md = osStyle.control_height;

  ZLay_Style rootStyle = ZLay_ExampleRootStyle(osStyle);
  rootStyle.background = theme.canvas;
  rootStyle.text_color = theme.text;

  ZLAY(ctx, ZLAY_ID("Root"), ZLAY_BOX(.id = ZLAY_ID("Root"), .style = rootStyle)) {
    ZLay_NodeDeclarationBuilder header = ZLay_RowDecl(ZLAY_ID("Header"), theme.space_3, ZLay_Insets(0.0f));
    header.decl.style.height = ZLay_Px(48.0f);

    ZLAY(ctx, ZLAY_ID("Header"), header) {
      ZLay_LabelProps title = ZLay_LabelPropsDefault(&theme);
      title.role = ZLAY_LABEL_TITLE;
      title.size = ZLAY_COMPONENT_SIZE_LG;
      ZLay_Label(ctx, ZLAY_ID("AppTitle"), ZLAY_STRING_LITERAL("ZLay Components"), &title);

      ZLay_Style fill = ZLay_StyleDefault();
      fill.width = ZLay_Grow(1.0f);
      fill.height = ZLay_Px(1.0f);
      ZLAY(ctx, ZLAY_ID("HeaderFill"), ZLAY_BOX(.id = ZLAY_ID("HeaderFill"), .style = fill)) {}

      ZLay_ButtonProps ghost = ZLay_ButtonPropsDefault(&theme);
      ghost.variant = ZLAY_BUTTON_GHOST;
      ZLay_Button(ctx, ZLAY_ID("DocsButton"), ZLAY_STRING_LITERAL("Docs"), &ghost);

      ZLay_ButtonProps primary = ZLay_ButtonPropsDefault(&theme);
      primary.variant = ZLAY_BUTTON_PRIMARY;
      ZLay_Button(ctx, ZLAY_ID("NewButton"), ZLAY_STRING_LITERAL("New Project"), &primary);
    }

    ZLay_NodeDeclarationBuilder content = ZLay_RowDecl(ZLAY_ID("Content"), theme.space_4, ZLay_Insets(0.0f));
    content.decl.style.height = ZLay_Grow(1.0f);

    ZLAY(ctx, ZLAY_ID("Content"), content) {
      ZLay_NodeDeclarationBuilder sidebar = ZLay_PanelDecl(ZLAY_ID("Sidebar"), &theme);
      sidebar.decl.style.width = ZLay_Px(280.0f);
      sidebar.decl.style.height = ZLay_Grow(1.0f);
      sidebar.decl.style.gap = theme.space_3;

      ZLAY(ctx, ZLAY_ID("Sidebar"), sidebar) {
        ZLay_LabelProps caption = ZLay_LabelPropsDefault(&theme);
        caption.role = ZLAY_LABEL_CAPTION;
        ZLay_Label(ctx, ZLAY_ID("NavCaption"), ZLAY_STRING_LITERAL("NAVIGATION"), &caption);

        ZLay_ButtonProps selected = ZLay_ButtonPropsDefault(&theme);
        selected.variant = ZLAY_BUTTON_SECONDARY;
        selected.state = ZLAY_COMPONENT_STATE_SELECTED;
        ZLay_Button(ctx, ZLAY_ID("OverviewNav"), ZLAY_STRING_LITERAL("Overview"), &selected);

        ZLay_ButtonProps nav = ZLay_ButtonPropsDefault(&theme);
        nav.variant = ZLAY_BUTTON_GHOST;
        ZLay_Button(ctx, ZLAY_ID("ComponentsNav"), ZLAY_STRING_LITERAL("Components"), &nav);
        ZLay_Button(ctx, ZLAY_ID("ThemesNav"), ZLAY_STRING_LITERAL("Themes"), &nav);

        ZLay_Divider(ctx, ZLAY_ID("SidebarDivider"), ZLAY_AXIS_HORIZONTAL, &theme);

        ZLay_InputProps search = ZLay_InputPropsDefault(&theme);
        search.variant = ZLAY_INPUT_FILLED;
        ZLay_Input(ctx, ZLAY_ID("SearchInput"), ZLay_String{0}, ZLAY_STRING_LITERAL("Search components..."), &search);
      }

      ZLay_NodeDeclarationBuilder mainColumn = ZLay_ColumnDecl(ZLAY_ID("MainColumn"), theme.space_4, ZLay_Insets(0.0f));
      mainColumn.decl.style.width = ZLay_Grow(1.0f);
      mainColumn.decl.style.height = ZLay_Grow(1.0f);

      ZLAY(ctx, ZLAY_ID("MainColumn"), mainColumn) {
        ZLay_NodeDeclarationBuilder hero = ZLay_CardDecl(ZLAY_ID("HeroCard"), &theme);
        hero.decl.style.height = ZLay_Px(160.0f);
        hero.decl.style.gap = theme.space_3;

        ZLAY(ctx, ZLAY_ID("HeroCard"), hero) {
          ZLay_LabelProps badge = ZLay_LabelPropsDefault(&theme);
          badge.role = ZLAY_LABEL_BADGE;
          ZLay_Label(ctx, ZLAY_ID("HeroBadge"), ZLAY_STRING_LITERAL("Frontend Foundations"), &badge);

          ZLay_LabelProps heroTitle = ZLay_LabelPropsDefault(&theme);
          heroTitle.role = ZLAY_LABEL_TITLE;
          heroTitle.size = ZLAY_COMPONENT_SIZE_LG;
          ZLay_Label(ctx, ZLAY_ID("HeroTitle"), ZLAY_STRING_LITERAL("Layout, tokens, and reusable controls"), &heroTitle);

          ZLay_LabelProps muted = ZLay_LabelPropsDefault(&theme);
          muted.role = ZLAY_LABEL_SUBTITLE;
          ZLay_Label(ctx, ZLAY_ID("HeroSubtitle"), ZLAY_STRING_LITERAL("Rows, columns, panels, cards, buttons, labels, inputs, dividers, and spacers."), &muted);
        }

        ZLay_NodeDeclarationBuilder cards = ZLay_RowDecl(ZLAY_ID("CardsRow"), theme.space_4, ZLay_Insets(0.0f));
        cards.decl.style.height = ZLay_Px(132.0f);

        ZLAY(ctx, ZLAY_ID("CardsRow"), cards) {
          ZLay_NodeDeclarationBuilder cardA = ZLay_CardDecl(ZLAY_ID("MetricCardA"), &theme);
          cardA.decl.style.width = ZLay_Grow(1.0f);
          cardA.decl.style.height = ZLay_Grow(1.0f);

          ZLAY(ctx, ZLAY_ID("MetricCardA"), cardA) {
            ZLay_LabelProps label = ZLay_LabelPropsDefault(&theme);
            label.role = ZLAY_LABEL_CAPTION;
            ZLay_Label(ctx, ZLAY_ID("MetricALabel"), ZLAY_STRING_LITERAL("COMPONENTS"), &label);
            ZLay_LabelProps value = ZLay_LabelPropsDefault(&theme);
            value.role = ZLAY_LABEL_TITLE;
            ZLay_Label(ctx, ZLAY_ID("MetricAValue"), ZLAY_STRING_LITERAL("12 primitives"), &value);
          }

          ZLay_NodeDeclarationBuilder cardB = ZLay_CardDecl(ZLAY_ID("MetricCardB"), &theme);
          cardB.decl.style.width = ZLay_Grow(1.0f);
          cardB.decl.style.height = ZLay_Grow(1.0f);

          ZLAY(ctx, ZLAY_ID("MetricCardB"), cardB) {
            ZLay_LabelProps label = ZLay_LabelPropsDefault(&theme);
            label.role = ZLAY_LABEL_CAPTION;
            ZLay_Label(ctx, ZLAY_ID("MetricBLabel"), ZLAY_STRING_LITERAL("STYLE TOKENS"), &label);
            ZLay_LabelProps value = ZLay_LabelPropsDefault(&theme);
            value.role = ZLAY_LABEL_TITLE;
            ZLay_Label(ctx, ZLAY_ID("MetricBValue"), ZLAY_STRING_LITERAL("Theme ready"), &value);
          }
        }
      }
    }
  }

  ZLay_ExampleMemory frameMem = ZLay_ExampleCreateMemory(16 * 1024);
  ZLay_OpenGLRenderer glBackend{};
  ZLay_OpenGLRendererInitInfo glInfo{};
  glInfo.frame_arena = frameMem.arena;
  if (!ZLay_OpenGLRenderer_Init(&glBackend, &glInfo)) {
    ZLAY_ERROR("failed to initialize OpenGL renderer adapter");
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

  ZLAY_INFO("render: os=%s renderer=%s commands=%u vertices=%u indices=%u draw_calls=%u",
            ZLay_OS_BackendName(),
            ZLay_RendererBackendName(renderer.backend),
            renderer.last_stats.command_count,
            renderer.last_stats.vertex_count,
            renderer.last_stats.index_count,
            renderer.last_stats.draw_call_count);
  ZLAY_INFO("shader: rect %s/%s source-bytes=%zu",
            ZLay_ShaderLanguageName(glBackend.rect_fragment_shader.language),
            ZLay_ShaderStageName(glBackend.rect_fragment_shader.stage),
            glBackend.rect_fragment_shader.source ? std::strlen(glBackend.rect_fragment_shader.source) : 0u);
  for (uint32_t i = 0; i < cmds.count; ++i) {
    const ZLay_RenderCommand& c = cmds.commands[i];
    ZLAY_DEBUG("render-command #%u type=%u x=%.1f y=%.1f w=%.1f h=%.1f z=%d",
               i, (unsigned)c.type, c.bounds.x, c.bounds.y, c.bounds.width, c.bounds.height, (int)c.z_index);
  }

  ZLay_RendererShutdown(&renderer);
  ZLay_ExampleDestroyMemory(&frameMem);
  ZLay_ExampleDestroyMemory(&perm);
  return 0;
}
