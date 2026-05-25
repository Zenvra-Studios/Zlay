/*
  ZLay - Layout & Styling Library (C)

  Copyright (c) 2026 Zenvra Studio

  License: GNU GPL (Vendor: Zenvra Studio)
  SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef ZLAY_ZLAY_H
#define ZLAY_ZLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zlay_math.h>

#if !defined(DISABLE_SIMD_HANDLE) && defined(__x86_64__) && (defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64))
#   include <emmintrin.h>
#elif defined(DISABLE_SIMD_HANDLE) && defined(__aarch64__)
#   include <arm_neon.h>
#endif

#if defined(__STDC_VERSION__)
#   undef ZLAY_C_STANDARD_VERSION
#   define ZLAY_C_STANDARD_VERSION __STDC_VERSION__
#   if __STDC_VERSION__ >= 199901L
#       undef ZLAY_C_STANDARD_C99
#       define ZLAY_C_STANDARD_C99 1
#   endif
#   if __STDC_VERSION__ >= 201112L
#       undef ZLAY_C_STANDARD_C11
#       define ZLAY_C_STANDARD_C11 1
#   endif
#endif

#if defined(WASM)
#   define ZLAY_WASM_EXPORT(name) __attribute__((export_name(name)))
#else
#   define ZLAY_WASM_EXPORT(null)
#endif

#if defined(WINNT_DLL_HANDLE_EXPORT)
#   define ZLAY_DLL_EXPORT __declspec(dllexport) __stdcall
#elif defined(UNIX_DYNMC_HANDLE_EXPORT)
#   define ZLAY_DYNMC_EXPORT __attribute__("") __stdcall
#endif

// -----------------------------
// Platform detection - single hex value with category bitmask
// -----------------------------

#define ZLAY_PLATFORM_ID_MASK 0x00FFu
#define ZLAY_PLATFORM_CATEGORY_MASK 0xFF00u

#define ZLAY_CAT_WINDOWS 0x0100u
#define ZLAY_CAT_APPLE 0x0200u
#define ZLAY_CAT_BSD 0x0400u
#define ZLAY_CAT_UNIX 0x0800u
#define ZLAY_CAT_WEB 0x1000u
#define ZLAY_CAT_EMSCRIPTEN ZLAY_CAT_WEB

#define ZLAY_PLATFORM_UNKNOWN 0x0000u
#define ZLAY_PLATFORM_WIN32 (0x01u | ZLAY_CAT_WINDOWS)
#define ZLAY_PLATFORM_WIN64 (0x02u | ZLAY_CAT_WINDOWS)
#define ZLAY_PLATFORM_MACOS (0x03u | ZLAY_CAT_APPLE | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_MACCATALYST (0x04u | ZLAY_CAT_APPLE | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_IOS (0x05u | ZLAY_CAT_APPLE | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_TVOS (0x06u | ZLAY_CAT_APPLE | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_WATCHOS (0x07u | ZLAY_CAT_APPLE | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_VISIONOS (0x08u | ZLAY_CAT_APPLE | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_ANDROID (0x09u | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_LINUX (0x0Au | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_FREEBSD (0x0Bu | ZLAY_CAT_BSD | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_OPENBSD (0x0Cu | ZLAY_CAT_BSD | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_NETBSD (0x0Du | ZLAY_CAT_BSD | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_DRAGONFLYBSD (0x0Eu | ZLAY_CAT_BSD | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_SOLARIS (0x0Fu | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_HAIKU (0x10u | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_CYGWIN (0x11u | ZLAY_CAT_UNIX)
#define ZLAY_PLATFORM_EMSCRIPTEN (0x12u | ZLAY_CAT_EMSCRIPTEN | ZLAY_CAT_UNIX)

#define ZLAY_PLATFORM_NONE ZLAY_PLATFORM_UNKNOWN
#define ZLAY_PLATFORM_WINDOWS ZLAY_CAT_WINDOWS
#define ZLAY_PLATFORM_APPLE ZLAY_CAT_APPLE
#define ZLAY_PLATFORM_BSD ZLAY_CAT_BSD
#define ZLAY_PLATFORM_UNIX ZLAY_CAT_UNIX

#if !defined(ZLAY_PLATFORM)
#   if defined(_WIN64)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_WIN64
#   elif defined(_WIN32)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_WIN32
#   elif defined(__CYGWIN__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_CYGWIN
#   elif defined(__EMSCRIPTEN__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_EMSCRIPTEN
#   elif defined(__APPLE__) && defined(__MACH__)
#       include <TargetConditionals.h>
#       if defined(TARGET_OS_VISION) && TARGET_OS_VISION
#           define ZLAY_PLATFORM ZLAY_PLATFORM_VISIONOS
#       elif defined(TARGET_OS_TV) && TARGET_OS_TV
#           define ZLAY_PLATFORM ZLAY_PLATFORM_TVOS
#       elif defined(TARGET_OS_WATCH) && TARGET_OS_WATCH
#           define ZLAY_PLATFORM ZLAY_PLATFORM_WATCHOS
#       elif defined(TARGET_OS_MACCATALYST) && TARGET_OS_MACCATALYST
#           define ZLAY_PLATFORM ZLAY_PLATFORM_MACCATALYST
#       elif (defined(TARGET_OS_IOS) && TARGET_OS_IOS) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
#           define ZLAY_PLATFORM ZLAY_PLATFORM_IOS
#       else
#           define ZLAY_PLATFORM ZLAY_PLATFORM_MACOS
#       endif
#   elif defined(__ANDROID__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_ANDROID
#   elif defined(__linux__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_LINUX
#   elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_FREEBSD
#   elif defined(__OpenBSD__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_OPENBSD
#   elif defined(__NetBSD__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_NETBSD
#   elif defined(__DragonFly__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_DRAGONFLYBSD
#   elif defined(__sun) && defined(__SVR4)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_SOLARIS
#   elif defined(__HAIKU__)
#       define ZLAY_PLATFORM ZLAY_PLATFORM_HAIKU
#   else
#       define ZLAY_PLATFORM ZLAY_PLATFORM_UNKNOWN
#   endif
#endif

#define ZLAY_IS_WINDOWS() ((ZLAY_PLATFORM) & ZLAY_CAT_WINDOWS)
#define ZLAY_IS_APPLE() ((ZLAY_PLATFORM) & ZLAY_CAT_APPLE)
#define ZLAY_IS_BSD() ((ZLAY_PLATFORM) & ZLAY_CAT_BSD)
#define ZLAY_IS_UNIX() ((ZLAY_PLATFORM) & ZLAY_CAT_UNIX)
#define ZLAY_IS_WEB() ((ZLAY_PLATFORM) & ZLAY_CAT_WEB)
#define ZLAY_PLATFORM_ID() ((ZLAY_PLATFORM) & ZLAY_PLATFORM_ID_MASK)
#define ZLAY_IS_PLATFORM(platform) (ZLAY_PLATFORM_ID() == ((platform) & ZLAY_PLATFORM_ID_MASK))
#define ZLAY_IS_EMSCRIPTEN() ZLAY_IS_PLATFORM(ZLAY_PLATFORM_EMSCRIPTEN)
#define ZLAY_HAS_PLATFORM(platform)                                            \
  ((((platform) & ZLAY_PLATFORM_ID_MASK) != 0u)                                \
     ? ZLAY_IS_PLATFORM(platform)                                              \
     : (((ZLAY_PLATFORM) & (platform)) != 0u))
#define ZLAY_CURRENT_PLATFORM ZLAY_PLATFORM

#if defined(__cplusplus)
#   define ZLAY_BOX(...) (ZLay_NodeDeclarationBuilder{ .decl = ZLay_NodeDeclaration{ .type = ZLAY_NODE_BOX, __VA_ARGS__ } })
#   define ZLAY_TEXT_NODE(...) (ZLay_NodeDeclarationBuilder{ .decl = ZLay_NodeDeclaration{ .type = ZLAY_NODE_TEXT, __VA_ARGS__ } })
#else
#   define ZLAY_BOX(...) ((ZLay_NodeDeclarationBuilder){ .decl = (ZLay_NodeDeclaration){ .type = ZLAY_NODE_BOX, __VA_ARGS__ } })
#   define ZLAY_TEXT_NODE(...) ((ZLay_NodeDeclarationBuilder){ .decl = (ZLay_NodeDeclaration){ .type = ZLAY_NODE_TEXT, __VA_ARGS__ } })
#endif

#define ZLAY(ctx, id, builder_expr) \
    for (ZLay_NodeDeclarationBuilder ZLAY__builder = (builder_expr), *ZLAY__builder_ptr = &ZLAY__builder; ZLAY__builder_ptr; ZLAY__builder_ptr = 0) \
        for (int ZLAY__once = (ZLay__OpenElement((ctx), (id), ZLAY__builder.decl.type), ZLay__ConfigureOpenElement((ctx), &ZLAY__builder.decl), 0); ZLAY__once == 0; ZLAY__once = (ZLay__CloseElement((ctx)), 1))

#define ZLAY_TEXT(ctx, id, text, style_ptr) \
    do { ZLay__OpenTextElement((ctx), (id), (text), (style_ptr)); } while (0)

// -----------------------------
// DLL export
// -----------------------------

#ifndef ZLAY_API
#   define ZLAY_API
#   if defined(ZLAY_SHARED) && ZLAY_IS_WINDOWS()
#       undef ZLAY_API
#       if defined(ZLAY_BUILDING)
#           define ZLAY_API __declspec(dllexport)
#       else
#           define ZLAY_API __declspec(dllimport)
#       endif
#   elif defined(ZLAY_SHARED) && (defined(__GNUC__) || defined(__clang__))
#       undef ZLAY_API
#       define ZLAY_API __attribute__((visibility("default")))
#   endif
#endif

#include <zlay/zlay_rounded.h>

#define ZLAY_STRING_LITERAL(str_lit)                                           \
  ((ZLay_String){(str_lit), (uint32_t)(sizeof(str_lit) - 1u), true})

#define ZLAY_ID(str_lit) ZLay_IdFromString(ZLAY_STRING_LITERAL(str_lit))
#define ZLAY_DEFAULT_FONT_FAMILY "Inter, Noto Sans, DejaVu Sans, sans"

typedef uint32_t ZLay_ColorU32;

#define ZLAY_COLOR_U32_R_SHIFT 0u
#define ZLAY_COLOR_U32_G_SHIFT 8u
#define ZLAY_COLOR_U32_B_SHIFT 16u
#define ZLAY_COLOR_U32_A_SHIFT 24u
#define ZLAY_COLOR_U32_CHANNEL_MASK 0xFFu
#define ZLAY_COLOR_U32_RGB_MASK 0x00FFFFFFu
#define ZLAY_COLOR_U32_ALPHA_MASK 0xFF000000u
#define ZLAY_COLOR_U32_COMPONENT(value)                                        \
  ((uint32_t)(value) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_SET_R(r)                                                \
  (ZLAY_COLOR_U32_COMPONENT(r) << ZLAY_COLOR_U32_R_SHIFT)
#define ZLAY_COLOR_U32_SET_G(g)                                                \
  (ZLAY_COLOR_U32_COMPONENT(g) << ZLAY_COLOR_U32_G_SHIFT)
#define ZLAY_COLOR_U32_SET_B(b)                                                \
  (ZLAY_COLOR_U32_COMPONENT(b) << ZLAY_COLOR_U32_B_SHIFT)
#define ZLAY_COLOR_U32_SET_A(a)                                                \
  (ZLAY_COLOR_U32_COMPONENT(a) << ZLAY_COLOR_U32_A_SHIFT)
#define ZLAY_COLOR_U32_GET_R(color)                                            \
  (((uint32_t)(color) >> ZLAY_COLOR_U32_R_SHIFT) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_GET_G(color)                                            \
  (((uint32_t)(color) >> ZLAY_COLOR_U32_G_SHIFT) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_GET_B(color)                                            \
  (((uint32_t)(color) >> ZLAY_COLOR_U32_B_SHIFT) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_GET_A(color)                                            \
  (((uint32_t)(color) >> ZLAY_COLOR_U32_A_SHIFT) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_SET_ALPHA(color, alpha)                                 \
  ((((uint32_t)(color)) & ZLAY_COLOR_U32_RGB_MASK) |                           \
   ZLAY_COLOR_U32_SET_A(alpha))
#define ZLAY_COLOR_U32_GET_ALPHA(color) ZLAY_COLOR_U32_GET_A(color)
#define ZLAY_COLOR_U32(r, g, b, a)                                             \
  (ZLAY_COLOR_U32_SET_R(r) | ZLAY_COLOR_U32_SET_G(g) |                         \
   ZLAY_COLOR_U32_SET_B(b) | ZLAY_COLOR_U32_SET_A(a))
#define ZLAY_COLOR_U32_RGB(r, g, b) ZLAY_COLOR_U32((r), (g), (b), 255u)
#define ZLAY_COLOR_U32_TRANSPARENT ZLAY_COLOR_U32(0u, 0u, 0u, 0u)
#define ZLAY_COLOR_U32_WHITE ZLAY_COLOR_U32(255u, 255u, 255u, 255u)
#define ZLAY_COLOR_U32_BLACK ZLAY_COLOR_U32(0u, 0u, 0u, 255u)

// -----------------------------
// String
// Math, geometry, and color primitives live in zlay_math.h.
// -----------------------------

typedef struct ZLay_String {
  const char *chars;
  uint32_t length;
  bool is_static;
} ZLay_String;

// -----------------------------
// ID
// -----------------------------

typedef struct ZLay_Id {
  uint64_t hash;
} ZLay_Id;

// -----------------------------
// Arena
// -----------------------------

typedef struct ZLay_Arena {
  uint8_t *memory;
  size_t capacity;
  size_t offset;
} ZLay_Arena;

// -----------------------------
// Config
// -----------------------------

typedef struct ZLay_Config {
  uint32_t max_nodes;
  uint32_t max_commands;
  uint32_t max_stack;
} ZLay_Config;

// -----------------------------
// Style
// -----------------------------

typedef enum ZLay_SizeType {
  ZLAY_SIZE_AUTO = 0,
  ZLAY_SIZE_PX = 1,
  ZLAY_SIZE_PERCENT = 2,
  ZLAY_SIZE_GROW = 3
} ZLay_SizeType;

typedef struct ZLay_Size {
  ZLay_SizeType type;
  float value;
} ZLay_Size;

typedef enum ZLay_FlexDirection {
  ZLAY_FLEX_ROW = 0,
  ZLAY_FLEX_COLUMN = 1
} ZLay_FlexDirection;

typedef enum ZLay_LayoutMode {
  ZLAY_LAYOUT_FLEX = 0,
  ZLAY_LAYOUT_GRID = 1
} ZLay_LayoutMode;

typedef enum ZLay_PositionType {
  ZLAY_POSITION_RELATIVE = 0,
  ZLAY_POSITION_ABSOLUTE = 1,
  ZLAY_POSITION_FLOATING = 2
} ZLay_PositionType;

typedef enum ZLay_PointerCursor {
  ZLAY_POINTER_CURSOR_DEFAULT = 0,
  ZLAY_POINTER_CURSOR_POINTER = 1,
  ZLAY_POINTER_CURSOR_TEXT = 2
} ZLay_PointerCursor;

typedef struct ZLay_Shadow {
  ZLay_Color color;
  float offset_x;
  float offset_y;
  float blur;
  float spread;
} ZLay_Shadow;

typedef enum ZLay_ShadowPreset {
  ZLAY_SHADOW_NONE = 0,
  ZLAY_SHADOW_SM = 1,
  ZLAY_SHADOW_MD = 2,
  ZLAY_SHADOW_LG = 3,
  ZLAY_SHADOW_XL = 4,
  ZLAY_SHADOW_2XL = 5,
  ZLAY_SHADOW_XXL = ZLAY_SHADOW_2XL
} ZLay_ShadowPreset;

typedef struct ZLay_Style {
  ZLay_Size width;
  ZLay_Size height;

  float padding_left;
  float padding_top;
  float padding_right;
  float padding_bottom;

  float margin_left;
  float margin_top;
  float margin_right;
  float margin_bottom;

  float gap;

  ZLay_LayoutMode layout_mode;
  ZLay_FlexDirection flex_direction;
  uint32_t grid_columns;
  uint32_t grid_rows;
  ZLay_Size grid_auto_row_height;
  float grid_column_gap;
  float grid_row_gap;
  uint32_t grid_column_span;
  uint32_t grid_row_span;
  ZLay_PositionType position;
  float left;
  float top;
  float right;
  float bottom;
  int32_t z_index;

  ZLay_Color background;
  ZLay_Color text_color;
  ZLay_Color border_color;
  float border_width;
  float radius;
  ZLay_Rounded rounded;
  ZLay_Shadow shadow;
  ZLay_String font_family;
  float font_size;
  ZLay_PointerCursor pointer_cursor;
  bool text_cursor_visible;
  uint32_t text_cursor_index;
  float text_cursor_width;
  ZLay_Color text_cursor_color;
  bool text_selection_visible;
  uint32_t text_selection_start;
  uint32_t text_selection_end;
  ZLay_Color text_selection_color;
} ZLay_Style;

typedef struct ZLay_EdgeInsets {
  float left;
  float top;
  float right;
  float bottom;
} ZLay_EdgeInsets;

typedef enum ZLay_ComponentSize {
  ZLAY_COMPONENT_SIZE_SM = 0,
  ZLAY_COMPONENT_SIZE_MD = 1,
  ZLAY_COMPONENT_SIZE_LG = 2
} ZLay_ComponentSize;

typedef enum ZLay_ComponentStateFlags {
  ZLAY_COMPONENT_STATE_NONE = 0,
  ZLAY_COMPONENT_STATE_HOVERED = 1 << 0,
  ZLAY_COMPONENT_STATE_ACTIVE = 1 << 1,
  ZLAY_COMPONENT_STATE_FOCUSED = 1 << 2,
  ZLAY_COMPONENT_STATE_DISABLED = 1 << 3,
  ZLAY_COMPONENT_STATE_INVALID = 1 << 4,
  ZLAY_COMPONENT_STATE_SELECTED = 1 << 5
} ZLay_ComponentStateFlags;

typedef struct ZLay_PointerState {
  float x;
  float y;
  bool inside;
  bool left_down;
  bool left_pressed;
  bool left_released;
  bool moved;
} ZLay_PointerState;

typedef enum ZLay_SurfaceVariant {
  ZLAY_SURFACE_CANVAS = 0,
  ZLAY_SURFACE_PANEL = 1,
  ZLAY_SURFACE_CARD = 2,
  ZLAY_SURFACE_MUTED = 3
} ZLay_SurfaceVariant;

typedef enum ZLay_Axis {
  ZLAY_AXIS_HORIZONTAL = 0,
  ZLAY_AXIS_VERTICAL = 1
} ZLay_Axis;

typedef enum ZLay_MainAxisAlignment {
  ZLAY_MAIN_AXIS_START = 0,
  ZLAY_MAIN_AXIS_CENTER = 1,
  ZLAY_MAIN_AXIS_END = 2,
  ZLAY_MAIN_AXIS_SPACE_BETWEEN = 3,
  ZLAY_MAIN_AXIS_SPACE_AROUND = 4,
  ZLAY_MAIN_AXIS_SPACE_EVENLY = 5
} ZLay_MainAxisAlignment;

typedef enum ZLay_CrossAxisAlignment {
  ZLAY_CROSS_AXIS_START = 0,
  ZLAY_CROSS_AXIS_CENTER = 1,
  ZLAY_CROSS_AXIS_END = 2,
  ZLAY_CROSS_AXIS_STRETCH = 3
} ZLay_CrossAxisAlignment;

typedef struct ZLay_Theme {
  ZLay_Color canvas;
  ZLay_Color surface;
  ZLay_Color surface_raised;
  ZLay_Color surface_muted;
  ZLay_Color border;
  ZLay_Color border_strong;
  ZLay_Color text;
  ZLay_Color text_muted;
  ZLay_Color text_inverse;
  ZLay_Color primary;
  ZLay_Color primary_hover;
  ZLay_Color primary_active;
  ZLay_Color primary_soft;
  ZLay_Color danger;
  ZLay_Color danger_hover;
  ZLay_Color danger_soft;
  ZLay_Color success;
  ZLay_Color warning;
  ZLay_Color focus_ring;
  float space_1;
  float space_2;
  float space_3;
  float space_4;
  float space_5;
  float space_6;
  float space_8;
  float radius_sm;
  float radius_md;
  float radius_lg;
  float radius_pill;
  float control_sm;
  float control_md;
  float control_lg;
} ZLay_Theme;

// -----------------------------
// Nodes / layout declaration
// -----------------------------

typedef enum ZLay_NodeType {
  ZLAY_NODE_BOX = 0,
  ZLAY_NODE_TEXT = 1
} ZLay_NodeType;

typedef struct ZLay_NodeDeclaration {
  ZLay_NodeType type;
  ZLay_Id id;
  ZLay_Style style;

  // For text node
  ZLay_String text;
} ZLay_NodeDeclaration;

typedef struct ZLay_RenderCommand ZLay_RenderCommand;

typedef struct ZLay_RenderCommandList {
  ZLay_RenderCommand *commands;
  uint32_t count;
} ZLay_RenderCommandList;

typedef struct ZLay_Context ZLay_Context;

// -----------------------------
// Render commands
// -----------------------------

typedef enum ZLay_RenderCommandType {
  ZLAY_CMD_RECT = 0,
  ZLAY_CMD_TEXT = 1,
  ZLAY_CMD_CLIP_BEGIN = 2,
  ZLAY_CMD_CLIP_END = 3
} ZLay_RenderCommandType;

struct ZLay_RenderCommand {
  ZLay_RenderCommandType type;
  ZLay_Id id;
  ZLay_Rect bounds;
  ZLay_Color color;
  float radius;
  ZLay_Rounded rounded;
  ZLay_Color border_color;
  float border_width;
  ZLay_Shadow shadow;
  int32_t z_index;
  ZLay_String text;
  ZLay_String font_family;
  float font_size;
  ZLay_PointerCursor pointer_cursor;
};

// Declarative macro API (Clay-like)
// Usage:
//   ZLay_BeginLayout(ctx);
//   ZLAY(ctx, ZLAY_ID("Root"), ZLAY_BOX(.style = ...)) { ... }
//   ZLay_RenderCommandList cmds = ZLay_EndLayout(ctx);

typedef struct ZLay_NodeDeclarationBuilder {
  ZLay_NodeDeclaration decl;
} ZLay_NodeDeclarationBuilder;

// Flutter-inspired app facade. It stays allocation-free: callers provide
// route and state-slot storage through ZLay_AppConfig.
typedef struct ZLay_App ZLay_App;
typedef struct ZLay_BuildContext ZLay_BuildContext;

typedef enum ZLay_AppLifecycleState {
  ZLAY_APP_DETACHED = 0,
  ZLAY_APP_RESUMED = 1,
  ZLAY_APP_INACTIVE = 2,
  ZLAY_APP_HIDDEN = 3,
  ZLAY_APP_PAUSED = 4,
  ZLAY_APP_SHUTDOWN = 5
} ZLay_AppLifecycleState;

typedef enum ZLay_StateValueType {
  ZLAY_STATE_NONE = 0,
  ZLAY_STATE_BOOL = 1,
  ZLAY_STATE_INT = 2,
  ZLAY_STATE_FLOAT = 3,
  ZLAY_STATE_STRING = 4,
  ZLAY_STATE_POINTER = 5
} ZLay_StateValueType;

typedef struct ZLay_StateValue {
  ZLay_StateValueType type;
  union {
    bool bool_value;
    int64_t int_value;
    double float_value;
    ZLay_String string_value;
    void *pointer_value;
  } as;
} ZLay_StateValue;

typedef struct ZLay_StateSlot {
  ZLay_Id key;
  ZLay_StateValue value;
  uint64_t revision;
  bool active;
  bool dirty;
} ZLay_StateSlot;

typedef void (*ZLay_WidgetBuilderFn)(ZLay_BuildContext *build_context,
                                     void *user);
typedef void (*ZLay_AppLifecycleFn)(ZLay_App *app, ZLay_AppLifecycleState state,
                                    void *user);

typedef struct ZLay_Route {
  ZLay_String name;
  ZLay_WidgetBuilderFn build;
  void *user;
} ZLay_Route;

typedef struct ZLay_Navigator {
  ZLay_Route *stack;
  uint32_t capacity;
  uint32_t count;
} ZLay_Navigator;

struct ZLay_BuildContext {
  ZLay_App *app;
  ZLay_Context *layout;
  const ZLay_Theme *theme;
  const ZLay_Route *route;
  ZLay_Dimensions viewport;
  double delta_ms;
  uint64_t frame_index;
  void *user;
};

typedef struct ZLay_AppConfig {
  ZLay_String title;
  ZLay_Theme theme;
  ZLay_Route *route_stack;
  uint32_t route_capacity;
  ZLay_StateSlot *state_slots;
  uint32_t state_capacity;
  ZLay_WidgetBuilderFn home;
  void *home_user;
  ZLay_AppLifecycleFn lifecycle;
  void *user;
} ZLay_AppConfig;

struct ZLay_App {
  ZLay_Context *layout;
  ZLay_AppConfig config;
  ZLay_Theme theme;
  ZLay_Navigator navigator;
  ZLay_Route home_route;
  ZLay_BuildContext build_context;
  ZLay_AppLifecycleState lifecycle_state;
  ZLay_Dimensions viewport;
  double delta_ms;
  uint64_t frame_index;
  uint64_t state_revision;
  bool needs_build;
};

typedef enum ZLay_AnimationStatus {
  ZLAY_ANIMATION_DISMISSED = 0,
  ZLAY_ANIMATION_FORWARD = 1,
  ZLAY_ANIMATION_REVERSE = 2,
  ZLAY_ANIMATION_COMPLETED = 3,
  ZLAY_ANIMATION_STOPPED = 4
} ZLay_AnimationStatus;

typedef struct ZLay_AnimationController {
  double duration_ms;
  double elapsed_ms;
  float value;
  ZLay_AnimationStatus status;
  bool repeat;
} ZLay_AnimationController;

#define ZLAY_ROUTE_LITERAL(name_lit, build_fn, user_ptr)                       \
  ((ZLay_Route){ZLAY_STRING_LITERAL(name_lit), (build_fn), (user_ptr)})

#define ZLAY_STATE_BOOL(value) ZLay_StateBool(value)

#define ZLAY_STATE_INT(value) ZLay_StateInt((int64_t)(value))

#define ZLAY_STATE_FLOAT(value) ZLay_StateFloat((double)(value))

#define ZLAY_STATE_STRING(value) ZLay_StateString(value)

#define ZLAY_STATE_POINTER(value) ZLay_StatePointer((void *)(value))

#define ZLay_ColorPackU32(r, g, b, a) ((ZLay_ColorU32)ZLAY_COLOR_U32((r), (g), (b), (a)))

#define ZLay_ColorToPackedU32(color) ZLay_ColorPackU32((color).r, (color).g, (color).b, (color).a)

#if defined(__cplusplus)
#   define ZLay_ColorFromPackedU32(color) (ZLay_Color{ (uint8_t)ZLAY_COLOR_U32_GET_R(color), (uint8_t)ZLAY_COLOR_U32_GET_G(color), (uint8_t)ZLAY_COLOR_U32_GET_B(color), (uint8_t)ZLAY_COLOR_U32_GET_A(color) })
#else
#   define ZLay_ColorFromPackedU32(color) ((ZLay_Color){ (uint8_t)ZLAY_COLOR_U32_GET_R(color), (uint8_t)ZLAY_COLOR_U32_GET_G(color), (uint8_t)ZLAY_COLOR_U32_GET_B(color), (uint8_t)ZLAY_COLOR_U32_GET_A(color) })
#endif

#define ZLay_ColorPackedU32WithAlpha(color, alpha) ((ZLay_ColorU32)ZLAY_COLOR_U32_SET_ALPHA((color), (alpha)))

#define ZLay_ColorPackedU32R(color) ((uint8_t)ZLAY_COLOR_U32_GET_R(color))
#define ZLay_ColorPackedU32G(color) ((uint8_t)ZLAY_COLOR_U32_GET_G(color))
#define ZLay_ColorPackedU32B(color) ((uint8_t)ZLAY_COLOR_U32_GET_B(color))
#define ZLay_ColorPackedU32A(color) ((uint8_t)ZLAY_COLOR_U32_GET_A(color))

ZLAY_API uint32_t ZLay_Version(void);
ZLAY_API const char *ZLay_VersionString(void);
ZLAY_API const char *ZLay_LibraryName(void);
ZLAY_API const char *ZLay_VendorName(void);

ZLAY_API ZLay_Id ZLay_IdFromString(ZLay_String s);
ZLAY_API ZLay_Id ZLay_IdFromCString(const char *cstr);
ZLAY_API bool ZLay_StringIsEmpty(ZLay_String string);
ZLAY_API ZLay_String ZLay_StringFromCString(const char *cstr);
ZLAY_API const char *ZLay_DefaultFontFamily(void);
ZLAY_API ZLay_String ZLay_DefaultFontFamilyString(void);

ZLAY_API ZLay_Arena ZLay_CreateArenaWithCapacityAndMemory(size_t capacity,
                                                          void *memory);
ZLAY_API void ZLay_ArenaReset(ZLay_Arena *arena);
ZLAY_API void *ZLay_ArenaAlloc(ZLay_Arena *arena, size_t size,
                               size_t alignment);

ZLAY_API ZLay_Config ZLay_ConfigDefault(void);
ZLAY_API size_t ZLay_MinMemorySize(ZLay_Config config);

ZLAY_API ZLay_Style ZLay_StyleDefault(void);
ZLAY_API ZLay_Size ZLay_Px(float v);
ZLAY_API ZLay_Size ZLay_Percent(float v);
ZLAY_API ZLay_Size ZLay_Grow(float weight);
ZLAY_API ZLay_EdgeInsets ZLay_EdgeInsetsAll(float value);
ZLAY_API ZLay_EdgeInsets ZLay_EdgeInsetsSymmetric(float horizontal,
                                                  float vertical);
ZLAY_API ZLay_EdgeInsets ZLay_EdgeInsetsOnly(float left, float top, float right,
                                             float bottom);
ZLAY_API ZLay_EdgeInsets ZLay_Insets(float all);
ZLAY_API ZLay_EdgeInsets ZLay_InsetsXY(float x, float y);
ZLAY_API ZLay_EdgeInsets ZLay_InsetsLTRB(float left, float top, float right,
                                         float bottom);
ZLAY_API ZLay_Style ZLay_StyleWithPadding(ZLay_Style style,
                                          ZLay_EdgeInsets padding);
ZLAY_API ZLay_Style ZLay_StyleWithMargin(ZLay_Style style,
                                         ZLay_EdgeInsets margin);
ZLAY_API ZLay_Style ZLay_StyleWithSize(ZLay_Style style, ZLay_Size width,
                                       ZLay_Size height);
ZLAY_API ZLay_Style ZLay_StyleWithBackground(ZLay_Style style,
                                             ZLay_Color color);
ZLAY_API ZLay_Style ZLay_StyleWithRadius(ZLay_Style style, float radius);
ZLAY_API ZLay_Style ZLay_StyleWithRounded(ZLay_Style style,
                                          ZLay_Rounded rounded);
ZLAY_API ZLay_Style ZLay_StyleWithRoundedPreset(ZLay_Style style,
                                                ZLay_RoundedPreset preset);
ZLAY_API ZLay_Shadow ZLay_ShadowNone(void);
ZLAY_API ZLay_Shadow ZLay_ShadowMake(ZLay_Color color, float offset_x,
                                     float offset_y, float blur, float spread);
ZLAY_API ZLay_Shadow ZLay_ShadowPresetValue(ZLay_ShadowPreset preset);
ZLAY_API ZLay_Shadow ZLay_ShadowPx(float offset_x, float offset_y, float blur,
                                   float spread, uint8_t alpha);
ZLAY_API ZLay_Shadow ZLay_ShadowFloating(void);
ZLAY_API ZLay_Style ZLay_StyleWithBorder(ZLay_Style style, float width,
                                         ZLay_Color color);
ZLAY_API ZLay_Style ZLay_StyleWithShadow(ZLay_Style style, ZLay_Shadow shadow);
ZLAY_API ZLay_Style ZLay_StyleWithShadowPreset(ZLay_Style style,
                                               ZLay_ShadowPreset preset);
ZLAY_API ZLay_Style ZLay_StyleWithGap(ZLay_Style style, float gap);
ZLAY_API ZLay_Style ZLay_StyleWithDirection(ZLay_Style style,
                                            ZLay_FlexDirection direction);
ZLAY_API ZLay_Style ZLay_StyleWithGrid(ZLay_Style style, uint32_t columns,
                                       ZLay_Size auto_row_height,
                                       float column_gap, float row_gap);
ZLAY_API ZLay_Style ZLay_StyleWithGridSpan(ZLay_Style style,
                                           uint32_t column_span,
                                           uint32_t row_span);
ZLAY_API ZLay_Style ZLay_StyleWithColors(ZLay_Style style,
                                         ZLay_Color background,
                                         ZLay_Color text);
ZLAY_API ZLay_Style ZLay_StyleWithFont(ZLay_Style style, ZLay_String family,
                                       float size);
ZLAY_API ZLay_Style ZLay_StyleWithPointerCursor(ZLay_Style style,
                                                ZLay_PointerCursor cursor);
ZLAY_API ZLay_Style ZLay_StyleWithTextCursor(ZLay_Style style, bool visible,
                                             uint32_t index, ZLay_Color color);
ZLAY_API ZLay_Style ZLay_StyleWithTextSelection(ZLay_Style style,
                                                uint32_t start, uint32_t end,
                                                ZLay_Color color);
ZLAY_API ZLay_Style ZLay_RowStyle(ZLay_Size width, ZLay_Size height, float gap);
ZLAY_API ZLay_Style ZLay_ColumnStyle(ZLay_Size width, ZLay_Size height,
                                     float gap);
ZLAY_API ZLay_Style ZLay_GridStyle(ZLay_Size width, ZLay_Size height,
                                   uint32_t columns, ZLay_Size auto_row_height,
                                   float gap);
ZLAY_API ZLay_Theme ZLay_ThemeLight(void);
ZLAY_API ZLay_Theme ZLay_ThemeDark(void);
ZLAY_API const ZLay_Theme *ZLay_ThemeDefault(void);
ZLAY_API float ZLay_ComponentControlHeight(ZLay_ComponentSize size,
                                           const ZLay_Theme *theme);
ZLAY_API ZLay_EdgeInsets ZLay_ComponentControlPadding(ZLay_ComponentSize size,
                                                      const ZLay_Theme *theme);
ZLAY_API ZLay_Style ZLay_StackStyle(ZLay_FlexDirection direction, float gap,
                                    ZLay_EdgeInsets padding);
ZLAY_API ZLay_Style ZLay_SurfaceStyle(ZLay_SurfaceVariant variant,
                                      const ZLay_Theme *theme);
ZLAY_API ZLay_Style ZLay_CardStyle(const ZLay_Theme *theme);
ZLAY_API ZLay_Style ZLay_SpacerStyle(float size, ZLay_Axis axis);
ZLAY_API ZLay_Style ZLay_DividerStyle(ZLay_Axis axis, const ZLay_Theme *theme);

ZLAY_API ZLay_Context *ZLay_Initialize(ZLay_Arena arena, ZLay_Config config);
ZLAY_API void ZLay_SetLayoutDimensions(ZLay_Context *ctx, ZLay_Dimensions dims);

typedef ZLay_Dimensions (*ZLay_MeasureTextFn)(ZLay_String text, void *user);
ZLAY_API void ZLay_SetMeasureTextFunction(ZLay_Context *ctx,
                                          ZLay_MeasureTextFn fn, void *user);
ZLAY_API bool ZLay_ContextFindBounds(const ZLay_Context *ctx, ZLay_Id id,
                                     ZLay_Rect *out_bounds);

ZLAY_API void ZLay_BeginLayout(ZLay_Context *ctx);
ZLAY_API ZLay_RenderCommandList ZLay_EndLayout(ZLay_Context *ctx);

// Low-level open/config/close (used by macros)
ZLAY_API void ZLay__OpenElement(ZLay_Context *ctx, ZLay_Id id,
                                ZLay_NodeType type);
ZLAY_API void ZLay__ConfigureOpenElement(ZLay_Context *ctx,
                                         const ZLay_NodeDeclaration *decl);
ZLAY_API void ZLay__CloseElement(ZLay_Context *ctx);
ZLAY_API void ZLay__OpenTextElement(ZLay_Context *ctx, ZLay_Id id,
                                    ZLay_String text, const ZLay_Style *style);

// Helpers for macro-friendly struct init
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_BoxDecl(ZLay_Id id, ZLay_Style style);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_TextDecl(ZLay_Id id, ZLay_String text,
                                                   ZLay_Style style);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_RowDecl(ZLay_Id id, float gap,
                                                  ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_ColumnDecl(ZLay_Id id, float gap,
                                                     ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_GridDecl(ZLay_Id id, uint32_t columns,
                                                   ZLay_Size auto_row_height,
                                                   float gap,
                                                   ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_PanelDecl(ZLay_Id id,
                                                    const ZLay_Theme *theme);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_CardDecl(ZLay_Id id,
                                                   const ZLay_Theme *theme);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_SpacerDecl(ZLay_Id id, float size,
                                                     ZLay_Axis axis);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_DividerDecl(ZLay_Id id,
                                                      ZLay_Axis axis,
                                                      const ZLay_Theme *theme);

ZLAY_API void ZLay_Spacer(ZLay_Context *ctx, ZLay_Id id, float size,
                          ZLay_Axis axis);
ZLAY_API void ZLay_Divider(ZLay_Context *ctx, ZLay_Id id, ZLay_Axis axis,
                           const ZLay_Theme *theme);

ZLAY_API bool ZLay_RectContains(ZLay_Rect rect, float x, float y);
ZLAY_API bool ZLay_RenderCommandListFindBounds(ZLay_RenderCommandList commands,
                                               ZLay_Id id,
                                               ZLay_Rect *out_bounds);
ZLAY_API ZLay_PointerCursor ZLay_RenderCommandListCursorAt(
    ZLay_RenderCommandList commands, float x, float y);
ZLAY_API uint32_t ZLay_ComponentStateFromPointer(
    const ZLay_PointerState *pointer, ZLay_Rect bounds, bool tracked);

ZLAY_API ZLay_AppConfig ZLay_AppConfigDefault(void);
ZLAY_API bool ZLay_AppInit(ZLay_App *app, ZLay_Context *layout,
                           ZLay_AppConfig config);
ZLAY_API bool ZLay_AppSetLifecycleState(ZLay_App *app,
                                        ZLay_AppLifecycleState state);
ZLAY_API void ZLay_AppSetTheme(ZLay_App *app, ZLay_Theme theme);
ZLAY_API const ZLay_Theme *ZLay_AppTheme(const ZLay_App *app);
ZLAY_API void ZLay_AppMarkNeedsBuild(ZLay_App *app);
ZLAY_API bool ZLay_AppNeedsBuild(const ZLay_App *app);
ZLAY_API bool ZLay_AppBeginFrame(ZLay_App *app, ZLay_Dimensions viewport,
                                 double delta_ms);
ZLAY_API bool ZLay_AppBuild(ZLay_App *app);
ZLAY_API ZLay_RenderCommandList ZLay_AppEndFrame(ZLay_App *app);
ZLAY_API ZLay_RenderCommandList ZLay_AppRunFrame(ZLay_App *app,
                                                 ZLay_Dimensions viewport,
                                                 double delta_ms);

ZLAY_API bool ZLay_NavigatorPush(ZLay_App *app, ZLay_Route route);
ZLAY_API bool ZLay_NavigatorReplace(ZLay_App *app, ZLay_Route route);
ZLAY_API bool ZLay_NavigatorPop(ZLay_App *app);
ZLAY_API bool ZLay_NavigatorCanPop(const ZLay_App *app);
ZLAY_API const ZLay_Route *ZLay_NavigatorCurrent(const ZLay_App *app);

ZLAY_API ZLay_StateValue ZLay_StateNone(void);
ZLAY_API ZLay_StateValue ZLay_StateBool(bool value);
ZLAY_API ZLay_StateValue ZLay_StateInt(int64_t value);
ZLAY_API ZLay_StateValue ZLay_StateFloat(double value);
ZLAY_API ZLay_StateValue ZLay_StateString(ZLay_String value);
ZLAY_API ZLay_StateValue ZLay_StatePointer(void *value);
ZLAY_API ZLay_StateSlot *ZLay_AppUseState(ZLay_App *app, ZLay_Id key,
                                          ZLay_StateValue initial_value);
ZLAY_API const ZLay_StateSlot *ZLay_AppFindState(const ZLay_App *app,
                                                 ZLay_Id key);
ZLAY_API bool ZLay_AppSetStateValue(ZLay_App *app, ZLay_Id key,
                                    ZLay_StateValue value);
ZLAY_API bool ZLay_AppClearState(ZLay_App *app, ZLay_Id key);

ZLAY_API void ZLay_AnimationControllerInit(ZLay_AnimationController *controller,
                                           double duration_ms);
ZLAY_API void ZLay_AnimationForward(ZLay_AnimationController *controller);
ZLAY_API void ZLay_AnimationReverse(ZLay_AnimationController *controller);
ZLAY_API void ZLay_AnimationStop(ZLay_AnimationController *controller);
ZLAY_API void ZLay_AnimationReset(ZLay_AnimationController *controller);
ZLAY_API bool ZLay_AnimationTick(ZLay_AnimationController *controller,
                                 double delta_ms);
ZLAY_API float ZLay_AnimationValue(const ZLay_AnimationController *controller);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ZLAY_ZLAY_H
