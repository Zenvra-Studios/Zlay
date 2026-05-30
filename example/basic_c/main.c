#include <stdio.h>
#include <string.h>

#include <backends/renderer/opengl/zlay_opengl_renderer_backend.h>
#include <os/zlay_os.h>
#include <renderer/zlay_renderer.h>
#include <zlay.h>
#include <zlay_antialiasing.h>
#include <zlay_ui.h>
#include <zlay_example.h>
#include <zlay_shell.h>

int main(void) {
  ZLay_ShellCapabilities shell = ZLay_ShellDetect(stdout);
  ZLay_ShellStyle ok_style = ZLay_ShellStyleStrong(ZLAY_SHELL_TONE_SUCCESS);
  ZLay_ShellStyle warn_style = ZLay_ShellStyleStrong(ZLAY_SHELL_TONE_WARNING);
  ZLay_ShellStyle dim_style = ZLay_ShellStyleTone(ZLAY_SHELL_TONE_MUTED);

  ZLay_Config cfg;
  size_t mem_size;
  ZLay_ExampleMemory perm;
  ZLay_Context* ctx;
  ZLay_OSFrameInfo frame;
  ZLay_OSStyleInfo os_style;
  ZLay_Theme theme;
  ZLay_Style root_style;
  ZLay_ExampleMemory frame_mem;
  ZLay_OpenGLRenderer gl_backend;
  ZLay_OpenGLRendererInitInfo gl_info;
  ZLay_Renderer renderer;
  ZLay_RendererFrameInfo render_frame;
  ZLay_RenderCommandList cmds;
  uint32_t i;

  ZLay_ShellPrintRule(stdout, "ZLay basic C example", ZLAY_SHELL_TONE_ACCENT);
  ZLay_ShellPrintCapabilitySummary(stdout, &shell);

  cfg = ZLay_ConfigDefault();
  mem_size = ZLay_MinMemorySize(cfg);
  perm = ZLay_ExampleCreateMemory(mem_size);
  ctx = ZLay_Initialize(perm.arena, cfg);
  if (ctx == NULL) {
    ZLay_ShellPrintfStyled(stderr, warn_style, "ZLay: failed to initialize context\n");
    ZLay_ExampleDestroyMemory(&perm);
    return 1;
  }

  frame = (ZLay_OSFrameInfo){0};
  frame.width = 960;
  frame.height = 540;
  frame.dpi_scale = ZLay_OS_GetDpiScale(NULL);
  frame.time_ns = ZLay_OS_NowNs();
  ZLay_OS_NewFrame(ctx, &frame);

  os_style = ZLay_OS_GetStyleInfo();
  theme = os_style.mode == ZLAY_OS_THEME_DARK ? ZLay_ThemeDark() : ZLay_ThemeLight();
  theme.canvas = os_style.window_bg;
  theme.surface = os_style.panel_bg;
  theme.text = os_style.text;
  theme.primary = os_style.accent;
  theme.primary_hover = ZLay_ColorMix(os_style.accent, theme.text, 0.12f);
  theme.primary_active = ZLay_ColorMix(os_style.accent, theme.text_inverse, 0.18f);
  theme.radius_md = os_style.corner_radius;
  theme.radius_lg = os_style.corner_radius + 6.0f;
  theme.control_md = os_style.control_height;

  root_style = ZLay_ExampleRootStyle(os_style);
  root_style.background = theme.canvas;
  root_style.text_color = theme.text;

  ZLay_BeginLayout(ctx);

  ZLAY(ctx, ZLAY_ID("RootC"), ZLAY_BOX(.id = ZLAY_ID("RootC"), .style = root_style)) {
    ZLay_NodeDeclarationBuilder header = ZLay_RowDecl(ZLAY_ID("HeaderC"), theme.space_3, ZLay_Insets(0.0f));
    header.decl.style.height = ZLay_Px(48.0f);

    ZLAY(ctx, ZLAY_ID("HeaderC"), header) {
      ZLay_LabelProps title = ZLay_LabelPropsDefault(&theme);
      ZLay_Style fill = ZLay_StyleDefault();
      ZLay_ButtonProps primary = ZLay_ButtonPropsDefault(&theme);

      title.role = ZLAY_LABEL_TITLE;
      title.size = ZLAY_COMPONENT_SIZE_LG;
      ZLay_Label(ctx, ZLAY_ID("TitleC"), ZLAY_STRING_LITERAL("ZLay C Components"), &title);

      fill.width = ZLay_Grow(1.0f);
      fill.height = ZLay_Px(1.0f);
      ZLAY(ctx, ZLAY_ID("HeaderFillC"), ZLAY_BOX(.id = ZLAY_ID("HeaderFillC"), .style = fill)) {
      }

      primary.variant = ZLAY_BUTTON_PRIMARY;
      ZLay_Button(ctx, ZLAY_ID("RunButtonC"), ZLAY_STRING_LITERAL("Run"), &primary);
    }

    {
      ZLay_NodeDeclarationBuilder content = ZLay_RowDecl(ZLAY_ID("ContentC"), theme.space_4, ZLay_Insets(0.0f));
      content.decl.style.height = ZLay_Grow(1.0f);

      ZLAY(ctx, ZLAY_ID("ContentC"), content) {
        ZLay_NodeDeclarationBuilder panel = ZLay_PanelDecl(ZLAY_ID("PanelC"), &theme);
        ZLay_NodeDeclarationBuilder card = ZLay_CardDecl(ZLAY_ID("CardC"), &theme);

        panel.decl.style.width = ZLay_Px(240.0f);
        panel.decl.style.height = ZLay_Grow(1.0f);
        panel.decl.style.gap = theme.space_3;

        ZLAY(ctx, ZLAY_ID("PanelC"), panel) {
          ZLay_LabelProps caption = ZLay_LabelPropsDefault(&theme);
          ZLay_ButtonProps selected = ZLay_ButtonPropsDefault(&theme);
          ZLay_ButtonProps ghost = ZLay_ButtonPropsDefault(&theme);

          caption.role = ZLAY_LABEL_CAPTION;
          ZLay_Label(ctx, ZLAY_ID("CaptionC"), ZLAY_STRING_LITERAL("C EXAMPLE"), &caption);

          selected.variant = ZLAY_BUTTON_SECONDARY;
          selected.state = ZLAY_COMPONENT_STATE_SELECTED;
          ZLay_Button(ctx, ZLAY_ID("OverviewC"), ZLAY_STRING_LITERAL("Overview"), &selected);

          ghost.variant = ZLAY_BUTTON_GHOST;
          ZLay_Button(ctx, ZLAY_ID("ComponentsC"), ZLAY_STRING_LITERAL("Components"), &ghost);
          ZLay_Button(ctx, ZLAY_ID("RendererC"), ZLAY_STRING_LITERAL("Renderer"), &ghost);
        }

        card.decl.style.width = ZLay_Grow(1.0f);
        card.decl.style.height = ZLay_Grow(1.0f);
        card.decl.style.gap = theme.space_3;

        ZLAY(ctx, ZLAY_ID("CardC"), card) {
          ZLay_LabelProps badge = ZLay_LabelPropsDefault(&theme);
          ZLay_LabelProps title = ZLay_LabelPropsDefault(&theme);
          ZLay_LabelProps subtitle = ZLay_LabelPropsDefault(&theme);
          ZLay_InputProps input = ZLay_InputPropsDefault(&theme);

          badge.role = ZLAY_LABEL_BADGE;
          ZLay_Label(ctx, ZLAY_ID("BadgeC"), ZLAY_STRING_LITERAL("Header-only shell colors"), &badge);

          title.role = ZLAY_LABEL_TITLE;
          title.size = ZLAY_COMPONENT_SIZE_LG;
          ZLay_Label(ctx, ZLAY_ID("HeroTitleC"), ZLAY_STRING_LITERAL("Portable terminal output"), &title);

          subtitle.role = ZLAY_LABEL_SUBTITLE;
          ZLay_Label(ctx, ZLAY_ID("SubtitleC"), ZLAY_STRING_LITERAL("The C example uses the same shell helper as C++."), &subtitle);

          input.variant = ZLAY_INPUT_FILLED;
          ZLay_Input(ctx, ZLAY_ID("InputC"), (ZLay_String){0}, ZLAY_STRING_LITERAL("Search..."), &input);
        }
      }
    }
  }

  frame_mem = ZLay_ExampleCreateMemory(16u * 1024u);
  gl_backend = (ZLay_OpenGLRenderer){0};
  gl_info = (ZLay_OpenGLRendererInitInfo){0};
  gl_info.frame_arena = frame_mem.arena;
  if (!ZLay_OpenGLRenderer_Init(&gl_backend, &gl_info)) {
    ZLay_ShellPrintfStyled(stderr, warn_style, "ZLay: failed to initialize OpenGL renderer adapter\n");
    ZLay_ExampleDestroyMemory(&frame_mem);
    ZLay_ExampleDestroyMemory(&perm);
    return 1;
  }

  renderer = ZLay_OpenGLRenderer_AsRenderer(&gl_backend);
  render_frame = (ZLay_RendererFrameInfo){0};
  render_frame.width = frame.width;
  render_frame.height = frame.height;
  render_frame.dpi_scale = frame.dpi_scale;
  render_frame.clear_color = os_style.window_bg;

  cmds = ZLay_EndLayout(ctx);
  ZLay_RendererBeginFrame(&renderer, &render_frame);
  ZLay_RendererRenderCommands(&renderer, &cmds);
  ZLay_RendererEndFrame(&renderer);

  ZLay_ShellWriteStyled(stdout, ok_style, "render");
  printf(": os=%s renderer=%s commands=%u vertices=%u indices=%u draw_calls=%u\n",
         ZLay_OS_BackendName(),
         ZLay_RendererBackendName(renderer.backend),
         renderer.last_stats.command_count,
         renderer.last_stats.vertex_count,
         renderer.last_stats.index_count,
         renderer.last_stats.draw_call_count);

  ZLay_ShellWriteStyled(stdout, ok_style, "shader");
  printf(": rect %s/%s source-bytes=%zu\n",
         ZLay_ShaderLanguageName(gl_backend.rect_fragment_shader.language),
         ZLay_ShaderStageName(gl_backend.rect_fragment_shader.stage),
         gl_backend.rect_fragment_shader.source != NULL ? strlen(gl_backend.rect_fragment_shader.source) : 0u);

  for (i = 0; i < cmds.count; ++i) {
    const ZLay_RenderCommand* command = &cmds.commands[i];
    ZLay_ShellPrintfStyled(stdout, dim_style, "  #%u", i);
    printf(" type=%u x=%.1f y=%.1f w=%.1f h=%.1f z=%d\n",
           (unsigned)command->type,
           command->bounds.x,
           command->bounds.y,
           command->bounds.width,
           command->bounds.height,
           (int)command->z_index);
  }

  ZLay_RendererShutdown(&renderer);
  ZLay_ExampleDestroyMemory(&frame_mem);
  ZLay_ExampleDestroyMemory(&perm);
  return 0;
}
