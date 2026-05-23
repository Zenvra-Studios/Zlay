#include <stdio.h>
#include <string.h>

#include <driver/opengl/zlay_opengl.h>
#include <os/zlay_os.h>
#include <renderer/zlay_renderer.h>
#include <zlay.h>
#include <zlay_antialiasing.h>
#include <zlay_example.h>
#include <zlay_shell.h>
#include <zlay_wrapper/zlay_wrapper.h>

static ZLayW_TextProps text_props(const ZLay_Theme* theme, ZLay_LabelRole role, ZLay_ComponentSize size) {
  ZLayW_TextProps props = ZLayW_TextPropsDefault(theme);
  props.role = role;
  props.size = size;
  return props;
}

static ZLayW_ButtonProps button_props(const ZLay_Theme* theme, ZLay_ButtonVariant variant) {
  ZLayW_ButtonProps props = ZLayW_ButtonPropsDefault(theme);
  props.variant = variant;
  return props;
}

static ZLayW_BoxProps box_props(const ZLay_Theme* theme, ZLay_FlexDirection direction, ZLay_Size width, ZLay_Size height) {
  ZLayW_BoxProps props = ZLayW_BoxPropsDefault(theme);
  props.direction = direction;
  props.width = width;
  props.height = height;
  props.gap = theme->space_4;
  return props;
}

static ZLayW_BoxProps card_props(const ZLay_Theme* theme, ZLay_Size width, ZLay_Size height) {
  ZLayW_BoxProps props = box_props(theme, ZLAY_FLEX_COLUMN, width, height);
  props.padding = ZLay_Insets(theme->space_4);
  props.radius = theme->radius_md;
  props.background = theme->surface_raised;
  props.text_color = theme->text;
  props.use_radius = true;
  props.use_background = true;
  props.use_text_color = true;
  return props;
}

static void metric_card(ZLay_Context* ctx,
                        const char* id,
                        const char* label,
                        const char* value,
                        const char* footnote,
                        const ZLay_Theme* theme) {
  ZLayW_BoxProps card = card_props(theme, ZLay_Grow(1.0f), ZLay_Px(118.0f));
  ZLayW_TextProps caption = text_props(theme, ZLAY_LABEL_CAPTION, ZLAY_COMPONENT_SIZE_SM);
  ZLayW_TextProps title = text_props(theme, ZLAY_LABEL_TITLE, ZLAY_COMPONENT_SIZE_LG);
  ZLayW_TextProps note = text_props(theme, ZLAY_LABEL_SUBTITLE, ZLAY_COMPONENT_SIZE_SM);

  ZLAY(ctx, ZLay_IdFromCString(id), ZLayW_BoxDecl(ZLay_IdFromCString(id), &card)) {
    ZLayW_Text(ctx, ZLay_IdFromCString(label), label, &caption);
    ZLayW_Text(ctx, ZLay_IdFromCString(value), value, &title);
    ZLayW_Text(ctx, ZLay_IdFromCString(footnote), footnote, &note);
  }
}

int main(void) {
  ZLay_ShellCapabilities shell = ZLay_ShellDetect(stdout);
  ZLay_ShellStyle ok_style = ZLay_ShellStyleStrong(ZLAY_SHELL_TONE_SUCCESS);
  ZLay_ShellStyle warn_style = ZLay_ShellStyleStrong(ZLAY_SHELL_TONE_WARNING);
  ZLay_ShellStyle dim_style = ZLay_ShellStyleTone(ZLAY_SHELL_TONE_MUTED);

  ZLay_Config cfg = ZLay_ConfigDefault();
  ZLay_ExampleMemory perm = ZLay_ExampleCreateMemory(ZLay_MinMemorySize(cfg));
  ZLay_Context* ctx = ZLay_Initialize(perm.arena, cfg);
  if (ctx == NULL) {
    ZLay_ShellPrintfStyled(stderr, warn_style, "ZLay wrapper: failed to initialize context\n");
    ZLay_ExampleDestroyMemory(&perm);
    return 1;
  }

  ZLay_OSFrameInfo frame = {0};
  frame.width = 1080;
  frame.height = 680;
  frame.dpi_scale = ZLay_OS_GetDpiScale(NULL);
  frame.time_ns = ZLay_OS_NowNs();
  ZLay_OS_NewFrame(ctx, &frame);

  ZLay_OSStyleInfo os_style = ZLay_OS_GetStyleInfo();
  ZLay_Theme theme = os_style.mode == ZLAY_OS_THEME_DARK ? ZLay_ThemeDark() : ZLay_ThemeLight();
  theme.canvas = os_style.window_bg;
  theme.surface = os_style.panel_bg;
  theme.surface_raised = ZLay_ColorMix(os_style.panel_bg, os_style.text, 0.04f);
  theme.text = os_style.text;
  theme.text_muted = ZLay_ColorMix(os_style.text, os_style.window_bg, 0.36f);
  theme.primary = os_style.accent;
  theme.primary_hover = ZLay_ColorMix(os_style.accent, os_style.text, 0.10f);
  theme.primary_active = ZLay_ColorMix(os_style.accent, theme.text_inverse, 0.18f);

  ZLayW_ScaffoldProps scaffold = ZLayW_ScaffoldPropsDefault(&theme);
  scaffold.width = (float)frame.width;
  scaffold.height = (float)frame.height;
  scaffold.padding = theme.space_6;
  scaffold.gap = theme.space_5;

  ZLayW_AppBarProps app_bar = ZLayW_AppBarPropsDefault(&theme);
  ZLayW_TextProps brand = text_props(&theme, ZLAY_LABEL_TITLE, ZLAY_COMPONENT_SIZE_LG);
  ZLayW_TextProps caption = text_props(&theme, ZLAY_LABEL_CAPTION, ZLAY_COMPONENT_SIZE_SM);
  ZLayW_TextProps hero = text_props(&theme, ZLAY_LABEL_TITLE, ZLAY_COMPONENT_SIZE_LG);
  ZLayW_TextProps subtitle = text_props(&theme, ZLAY_LABEL_SUBTITLE, ZLAY_COMPONENT_SIZE_MD);
  ZLayW_ButtonProps primary = button_props(&theme, ZLAY_BUTTON_PRIMARY);
  ZLayW_ButtonProps ghost = button_props(&theme, ZLAY_BUTTON_GHOST);
  ZLayW_ButtonProps selected = button_props(&theme, ZLAY_BUTTON_SECONDARY);
  ZLayW_InputProps search = ZLayW_InputPropsDefault(&theme);
  ZLayW_BoxProps content = box_props(&theme, ZLAY_FLEX_ROW, ZLay_Grow(1.0f), ZLay_Grow(1.0f));
  ZLayW_BoxProps sidebar = card_props(&theme, ZLay_Px(248.0f), ZLay_Grow(1.0f));
  ZLayW_BoxProps main_area = box_props(&theme, ZLAY_FLEX_COLUMN, ZLay_Grow(1.0f), ZLay_Grow(1.0f));
  ZLayW_BoxProps metrics = box_props(&theme, ZLAY_FLEX_ROW, ZLay_Grow(1.0f), ZLay_Px(118.0f));
  ZLayW_BoxProps hero_card = card_props(&theme, ZLay_Grow(1.0f), ZLay_Grow(1.0f));
  ZLayW_BoxProps actions = box_props(&theme, ZLAY_FLEX_ROW, ZLay_Grow(1.0f), ZLay_Px(theme.control_lg));

  selected.state = ZLAY_COMPONENT_STATE_SELECTED;
  search.variant = ZLAY_INPUT_OUTLINED;

  ZLay_ShellPrintRule(stdout, "ZLay wrapper C example", ZLAY_SHELL_TONE_ACCENT);
  ZLay_ShellPrintCapabilitySummary(stdout, &shell);

  ZLay_BeginLayout(ctx);

  ZLAYW_SCAFFOLD(ctx, "WrapperApp", &scaffold) {
    ZLAYW_APP_BAR(ctx, "AppBar", &app_bar) {
      ZLAYW_TEXT(ctx, "Brand", "ZLay Studio", &brand);
      ZLAYW_FILL(ctx, "AppBarFill");
      ZLAYW_INPUT(ctx, "Search", "", "Search projects", &search);
      ZLAYW_BUTTON(ctx, "SyncButton", "Sync", &ghost);
      ZLAYW_BUTTON(ctx, "CreateButton", "Create", &primary);
    }

    ZLAYW_ROW(ctx, "Content", &content) {
      ZLAYW_COLUMN(ctx, "Sidebar", &sidebar) {
        ZLAYW_TEXT(ctx, "MenuCaption", "WORKSPACE", &caption);
        ZLAYW_BUTTON(ctx, "OverviewItem", "Overview", &selected);
        ZLAYW_BUTTON(ctx, "PagesItem", "Pages", &ghost);
        ZLAYW_BUTTON(ctx, "MessagesItem", "Messages", &ghost);
        ZLay_Divider(ctx, ZLAY_ID("SidebarDivider"), ZLAY_AXIS_HORIZONTAL, &theme);
        ZLAYW_TEXT(ctx, "StatusCaption", "STATUS", &caption);
        ZLAYW_TEXT(ctx, "StatusCopy", "Built with ZLay UI.", &subtitle);
      }

      ZLAYW_COLUMN(ctx, "MainArea", &main_area) {
        ZLAYW_ROW(ctx, "Metrics", &metrics) {
          metric_card(ctx, "MetricViews", "Views", "24.8K", "+18% this week", &theme);
          metric_card(ctx, "MetricSales", "Orders", "1,284", "142 waiting", &theme);
          metric_card(ctx, "MetricHealth", "Health", "99.9%", "Renderer stable", &theme);
        }

        ZLAYW_COLUMN(ctx, "HeroCard", &hero_card) {
          ZLAYW_TEXT(ctx, "HeroBadge", "WRAPPER PREVIEW", &caption);
          ZLAYW_TEXT(ctx, "HeroTitle", "Build Flutter-like trees with plain C", &hero);
          ZLAYW_TEXT(ctx, "HeroSubtitle", "Scaffold, AppBar, Row, Column, Card, Text, Button, and Input on native ZLay.", &subtitle);
          ZLAYW_FILL(ctx, "HeroFill");
          ZLAYW_ROW(ctx, "HeroActions", &actions) {
            ZLAYW_BUTTON(ctx, "PrimaryAction", "Open dashboard", &primary);
            ZLAYW_BUTTON(ctx, "SecondaryAction", "View docs", &ghost);
          }
        }
      }
    }
  }

  ZLay_ExampleMemory frame_mem = ZLay_ExampleCreateMemory(16u * 1024u);
  ZLay_OpenGLRenderer gl_backend = {0};
  ZLay_OpenGLRendererInitInfo gl_info = {0};
  gl_info.frame_arena = frame_mem.arena;
  if (!ZLay_OpenGLRenderer_Init(&gl_backend, &gl_info)) {
    ZLay_ShellPrintfStyled(stderr, warn_style, "ZLay wrapper: failed to initialize OpenGL renderer adapter\n");
    ZLay_ExampleDestroyMemory(&frame_mem);
    ZLay_ExampleDestroyMemory(&perm);
    return 1;
  }

  ZLay_Renderer renderer = ZLay_OpenGLRenderer_AsRenderer(&gl_backend);
  ZLay_RendererFrameInfo render_frame = {0};
  render_frame.width = frame.width;
  render_frame.height = frame.height;
  render_frame.dpi_scale = frame.dpi_scale;
  render_frame.clear_color = theme.canvas;

  ZLay_RenderCommandList cmds = ZLay_EndLayout(ctx);
  ZLay_RendererBeginFrame(&renderer, &render_frame);
  ZLay_RendererRenderCommands(&renderer, &cmds);
  ZLay_RendererEndFrame(&renderer);

  ZLay_ShellWriteStyled(stdout, ok_style, "wrapper");
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

  for (uint32_t i = 0; i < cmds.count; ++i) {
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
