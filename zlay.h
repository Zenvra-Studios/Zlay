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

#define ZLAY_C_STANDARD_VERSION 0L
#define ZLAY_C_STANDARD_C99 0
#define ZLAY_C_STANDARD_C11 0

#if defined(__STDC_VERSION__)
#   undef ZLAY_C_STANDARD_VERSION
#   define ZLAY_C_STANDARD_VERSION __STDC_VERSION__
#   if __STDC_VERSION__ >= 199901L
#     undef ZLAY_C_STANDARD_C99
#     define ZLAY_C_STANDARD_C99 1
#   endif
#   if __STDC_VERSION__ >= 201112L
#     undef ZLAY_C_STANDARD_C11
#     define ZLAY_C_STANDARD_C11 1
#   endif
#endif

// -----------------------------
// Platform detection
// -----------------------------

#define ZLAY_PLATFORM_UNKNOWN 0
#define ZLAY_PLATFORM_WINDOWS 0
#define ZLAY_PLATFORM_WIN32 0
#define ZLAY_PLATFORM_WIN64 0
#define ZLAY_PLATFORM_APPLE 0
#define ZLAY_PLATFORM_MACOS 0
#define ZLAY_PLATFORM_MACCATALYST 0
#define ZLAY_PLATFORM_IOS 0
#define ZLAY_PLATFORM_TVOS 0
#define ZLAY_PLATFORM_WATCHOS 0
#define ZLAY_PLATFORM_VISIONOS 0
#define ZLAY_PLATFORM_ANDROID 0
#define ZLAY_PLATFORM_LINUX 0
#define ZLAY_PLATFORM_FREEBSD 0
#define ZLAY_PLATFORM_OPENBSD 0
#define ZLAY_PLATFORM_NETBSD 0
#define ZLAY_PLATFORM_DRAGONFLYBSD 0
#define ZLAY_PLATFORM_BSD 0
#define ZLAY_PLATFORM_SOLARIS 0
#define ZLAY_PLATFORM_HAIKU 0
#define ZLAY_PLATFORM_CYGWIN 0
#define ZLAY_PLATFORM_EMSCRIPTEN 0
#define ZLAY_PLATFORM_UNIX 0

#if defined(_WIN64)
#   undef ZLAY_PLATFORM_WINDOWS
#   define ZLAY_PLATFORM_WINDOWS 1
#   undef ZLAY_PLATFORM_WIN64
#   define ZLAY_PLATFORM_WIN64 1
#elif defined(_WIN32)
#   undef ZLAY_PLATFORM_WINDOWS
#   define ZLAY_PLATFORM_WINDOWS 1
#   undef ZLAY_PLATFORM_WIN32
#   define ZLAY_PLATFORM_WIN32 1
#elif defined(__CYGWIN__)
#   undef ZLAY_PLATFORM_CYGWIN
#   define ZLAY_PLATFORM_CYGWIN 1
#elif defined(__EMSCRIPTEN__)
#   undef ZLAY_PLATFORM_EMSCRIPTEN
#   define ZLAY_PLATFORM_EMSCRIPTEN 1
#elif defined(__APPLE__) && defined(__MACH__)
#   include <TargetConditionals.h>
#   undef ZLAY_PLATFORM_APPLE
#   define ZLAY_PLATFORM_APPLE 1
#   if defined(TARGET_OS_VISION) && TARGET_OS_VISION
#     undef ZLAY_PLATFORM_VISIONOS
#     define ZLAY_PLATFORM_VISIONOS 1
#   elif defined(TARGET_OS_TV) && TARGET_OS_TV
#     undef ZLAY_PLATFORM_TVOS
#     define ZLAY_PLATFORM_TVOS 1
#   elif defined(TARGET_OS_WATCH) && TARGET_OS_WATCH
#     undef ZLAY_PLATFORM_WATCHOS
#     define ZLAY_PLATFORM_WATCHOS 1
#   elif defined(TARGET_OS_MACCATALYST) && TARGET_OS_MACCATALYST
#     undef ZLAY_PLATFORM_MACCATALYST
#     define ZLAY_PLATFORM_MACCATALYST 1
#   elif defined(TARGET_OS_IOS) && TARGET_OS_IOS
#     undef ZLAY_PLATFORM_IOS
#     define ZLAY_PLATFORM_IOS 1
#   elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#     undef ZLAY_PLATFORM_IOS
#     define ZLAY_PLATFORM_IOS 1
#   else
#     undef ZLAY_PLATFORM_MACOS
#     define ZLAY_PLATFORM_MACOS 1
#   endif
#elif defined(__ANDROID__)
#   undef ZLAY_PLATFORM_ANDROID
#   define ZLAY_PLATFORM_ANDROID 1
#elif defined(__linux__)
#   undef ZLAY_PLATFORM_LINUX
#   define ZLAY_PLATFORM_LINUX 1
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#   undef ZLAY_PLATFORM_FREEBSD
#   define ZLAY_PLATFORM_FREEBSD 1
#   undef ZLAY_PLATFORM_BSD
#   define ZLAY_PLATFORM_BSD 1
#elif defined(__OpenBSD__)
#   undef ZLAY_PLATFORM_OPENBSD
#   define ZLAY_PLATFORM_OPENBSD 1
#   undef ZLAY_PLATFORM_BSD
#   define ZLAY_PLATFORM_BSD 1
#elif defined(__NetBSD__)
#   undef ZLAY_PLATFORM_NETBSD
#   define ZLAY_PLATFORM_NETBSD 1
#   undef ZLAY_PLATFORM_BSD
#   define ZLAY_PLATFORM_BSD 1
#elif defined(__DragonFly__)
#   undef ZLAY_PLATFORM_DRAGONFLYBSD
#   define ZLAY_PLATFORM_DRAGONFLYBSD 1
#   undef ZLAY_PLATFORM_BSD
#   define ZLAY_PLATFORM_BSD 1
#elif defined(__sun) && defined(__SVR4)
#   undef ZLAY_PLATFORM_SOLARIS
#   define ZLAY_PLATFORM_SOLARIS 1
#elif defined(__HAIKU__)
#   undef ZLAY_PLATFORM_HAIKU
#   define ZLAY_PLATFORM_HAIKU 1
#endif

#if ZLAY_PLATFORM_APPLE || ZLAY_PLATFORM_ANDROID || ZLAY_PLATFORM_LINUX || ZLAY_PLATFORM_BSD || ZLAY_PLATFORM_SOLARIS || ZLAY_PLATFORM_HAIKU || ZLAY_PLATFORM_CYGWIN
#   undef ZLAY_PLATFORM_UNIX
#   define ZLAY_PLATFORM_UNIX 1
#endif

#if !(ZLAY_PLATFORM_WINDOWS || ZLAY_PLATFORM_APPLE || ZLAY_PLATFORM_ANDROID || ZLAY_PLATFORM_LINUX || ZLAY_PLATFORM_BSD || ZLAY_PLATFORM_SOLARIS || ZLAY_PLATFORM_HAIKU || ZLAY_PLATFORM_CYGWIN || ZLAY_PLATFORM_EMSCRIPTEN)
#   undef ZLAY_PLATFORM_UNKNOWN
#   define ZLAY_PLATFORM_UNKNOWN 1
#endif

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
#   if defined(ZLAY_SHARED) && ZLAY_PLATFORM_WINDOWS
#     undef ZLAY_API
#     if defined(ZLAY_BUILDING)
#       define ZLAY_API __declspec(dllexport)
#     else
#       define ZLAY_API __declspec(dllimport)
#     endif
#  elif defined(ZLAY_SHARED) && (defined(__GNUC__) || defined(__clang__))
#     undef ZLAY_API
#     define ZLAY_API __attribute__((visibility("default")))
#  endif
#endif

#define ZLAY_STRING_LITERAL(str_lit) \
  ((ZLay_String){ (str_lit), (uint32_t)(sizeof(str_lit) - 1u), true })
  
#define ZLAY_ID(str_lit) ZLay_IdFromString(ZLAY_STRING_LITERAL(str_lit))

typedef uint32_t ZLay_ColorU32;

#define ZLAY_COLOR_U32_R_SHIFT 0u
#define ZLAY_COLOR_U32_G_SHIFT 8u
#define ZLAY_COLOR_U32_B_SHIFT 16u
#define ZLAY_COLOR_U32_A_SHIFT 24u
#define ZLAY_COLOR_U32_CHANNEL_MASK 0xFFu
#define ZLAY_COLOR_U32_RGB_MASK 0x00FFFFFFu
#define ZLAY_COLOR_U32_ALPHA_MASK 0xFF000000u
#define ZLAY_COLOR_U32_COMPONENT(value) ((uint32_t)(value) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_SET_R(r) (ZLAY_COLOR_U32_COMPONENT(r) << ZLAY_COLOR_U32_R_SHIFT)
#define ZLAY_COLOR_U32_SET_G(g) (ZLAY_COLOR_U32_COMPONENT(g) << ZLAY_COLOR_U32_G_SHIFT)
#define ZLAY_COLOR_U32_SET_B(b) (ZLAY_COLOR_U32_COMPONENT(b) << ZLAY_COLOR_U32_B_SHIFT)
#define ZLAY_COLOR_U32_SET_A(a) (ZLAY_COLOR_U32_COMPONENT(a) << ZLAY_COLOR_U32_A_SHIFT)
#define ZLAY_COLOR_U32_GET_R(color) (((uint32_t)(color) >> ZLAY_COLOR_U32_R_SHIFT) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_GET_G(color) (((uint32_t)(color) >> ZLAY_COLOR_U32_G_SHIFT) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_GET_B(color) (((uint32_t)(color) >> ZLAY_COLOR_U32_B_SHIFT) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_GET_A(color) (((uint32_t)(color) >> ZLAY_COLOR_U32_A_SHIFT) & ZLAY_COLOR_U32_CHANNEL_MASK)
#define ZLAY_COLOR_U32_SET_ALPHA(color, alpha) \
  ((((uint32_t)(color)) & ZLAY_COLOR_U32_RGB_MASK) | ZLAY_COLOR_U32_SET_A(alpha))
#define ZLAY_COLOR_U32_GET_ALPHA(color) ZLAY_COLOR_U32_GET_A(color)
#define ZLAY_COLOR_U32(r, g, b, a) \
  (ZLAY_COLOR_U32_SET_R(r) | ZLAY_COLOR_U32_SET_G(g) | ZLAY_COLOR_U32_SET_B(b) | ZLAY_COLOR_U32_SET_A(a))
#define ZLAY_COLOR_U32_RGB(r, g, b) ZLAY_COLOR_U32((r), (g), (b), 255u)
#define ZLAY_COLOR_U32_TRANSPARENT ZLAY_COLOR_U32(0u, 0u, 0u, 0u)
#define ZLAY_COLOR_U32_WHITE ZLAY_COLOR_U32(255u, 255u, 255u, 255u)
#define ZLAY_COLOR_U32_BLACK ZLAY_COLOR_U32(0u, 0u, 0u, 255u)

// -----------------------------
// String
// Math, geometry, and color primitives live in zlay_math.h.
// -----------------------------

typedef struct ZLay_String {
  const char* chars;
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
  uint8_t* memory;
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

typedef enum ZLay_PositionType {
  ZLAY_POSITION_RELATIVE = 0,
  ZLAY_POSITION_ABSOLUTE = 1,
  ZLAY_POSITION_FLOATING = 2
} ZLay_PositionType;

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

  ZLay_FlexDirection flex_direction;
  ZLay_PositionType position;
  float left;
  float top;
  float right;
  float bottom;
  int32_t z_index;

  ZLay_Color background;
  ZLay_Color text_color;
  float radius;
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
  ZLay_RenderCommand* commands;
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
  ZLay_Rect bounds;
  ZLay_Color color;
  float radius;
  int32_t z_index;
  ZLay_String text;
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
    void* pointer_value;
  } as;
} ZLay_StateValue;

typedef struct ZLay_StateSlot {
  ZLay_Id key;
  ZLay_StateValue value;
  uint64_t revision;
  bool active;
  bool dirty;
} ZLay_StateSlot;

typedef void (*ZLay_WidgetBuilderFn)(ZLay_BuildContext* build_context, void* user);
typedef void (*ZLay_AppLifecycleFn)(ZLay_App* app, ZLay_AppLifecycleState state, void* user);

typedef struct ZLay_Route {
  ZLay_String name;
  ZLay_WidgetBuilderFn build;
  void* user;
} ZLay_Route;

typedef struct ZLay_Navigator {
  ZLay_Route* stack;
  uint32_t capacity;
  uint32_t count;
} ZLay_Navigator;

struct ZLay_BuildContext {
  ZLay_App* app;
  ZLay_Context* layout;
  const ZLay_Theme* theme;
  const ZLay_Route* route;
  ZLay_Dimensions viewport;
  double delta_ms;
  uint64_t frame_index;
  void* user;
};

typedef struct ZLay_AppConfig {
  ZLay_String title;
  ZLay_Theme theme;
  ZLay_Route* route_stack;
  uint32_t route_capacity;
  ZLay_StateSlot* state_slots;
  uint32_t state_capacity;
  ZLay_WidgetBuilderFn home;
  void* home_user;
  ZLay_AppLifecycleFn lifecycle;
  void* user;
} ZLay_AppConfig;

struct ZLay_App {
  ZLay_Context* layout;
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

#define ZLAY_ROUTE_LITERAL(name_lit, build_fn, user_ptr) \
  ((ZLay_Route){ ZLAY_STRING_LITERAL(name_lit), (build_fn), (user_ptr) })

#define ZLAY_STATE_BOOL(value) \
  ZLay_StateBool(value)

#define ZLAY_STATE_INT(value) \
  ZLay_StateInt((int64_t)(value))

#define ZLAY_STATE_FLOAT(value) \
  ZLay_StateFloat((double)(value))

#define ZLAY_STATE_STRING(value) \
  ZLay_StateString(value)

#define ZLAY_STATE_POINTER(value) \
  ZLay_StatePointer((void*)(value))

static inline ZLay_ColorU32 ZLay_ColorPackU32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return (ZLay_ColorU32)ZLAY_COLOR_U32(r, g, b, a);
}

static inline ZLay_ColorU32 ZLay_ColorToPackedU32(ZLay_Color color) {
  return ZLay_ColorPackU32(color.r, color.g, color.b, color.a);
}

static inline ZLay_Color ZLay_ColorFromPackedU32(ZLay_ColorU32 color) {
  ZLay_Color result;
  result.r = (uint8_t)ZLAY_COLOR_U32_GET_R(color);
  result.g = (uint8_t)ZLAY_COLOR_U32_GET_G(color);
  result.b = (uint8_t)ZLAY_COLOR_U32_GET_B(color);
  result.a = (uint8_t)ZLAY_COLOR_U32_GET_A(color);
  return result;
}

static inline ZLay_ColorU32 ZLay_ColorPackedU32WithAlpha(ZLay_ColorU32 color, uint8_t alpha) {
  return (ZLay_ColorU32)ZLAY_COLOR_U32_SET_ALPHA(color, alpha);
}

static inline uint8_t ZLay_ColorPackedU32R(ZLay_ColorU32 color) {
  return (uint8_t)ZLAY_COLOR_U32_GET_R(color);
}

static inline uint8_t ZLay_ColorPackedU32G(ZLay_ColorU32 color) {
  return (uint8_t)ZLAY_COLOR_U32_GET_G(color);
}

static inline uint8_t ZLay_ColorPackedU32B(ZLay_ColorU32 color) {
  return (uint8_t)ZLAY_COLOR_U32_GET_B(color);
}

static inline uint8_t ZLay_ColorPackedU32A(ZLay_ColorU32 color) {
  return (uint8_t)ZLAY_COLOR_U32_GET_A(color);
}

ZLAY_API uint32_t ZLay_Version(void);
ZLAY_API const char* ZLay_VersionString(void);
ZLAY_API const char* ZLay_LibraryName(void);
ZLAY_API const char* ZLay_VendorName(void);

ZLAY_API ZLay_Id ZLay_IdFromString(ZLay_String s);
ZLAY_API ZLay_Id ZLay_IdFromCString(const char* cstr);
ZLAY_API bool ZLay_StringIsEmpty(ZLay_String string);
ZLAY_API ZLay_String ZLay_StringFromCString(const char* cstr);

ZLAY_API ZLay_Arena ZLay_CreateArenaWithCapacityAndMemory(size_t capacity, void* memory);
ZLAY_API void ZLay_ArenaReset(ZLay_Arena* arena);
ZLAY_API void* ZLay_ArenaAlloc(ZLay_Arena* arena, size_t size, size_t alignment);

ZLAY_API ZLay_Config ZLay_ConfigDefault(void);
ZLAY_API size_t ZLay_MinMemorySize(ZLay_Config config);

ZLAY_API ZLay_Style ZLay_StyleDefault(void);
ZLAY_API ZLay_Size ZLay_Px(float v);
ZLAY_API ZLay_Size ZLay_Percent(float v);
ZLAY_API ZLay_Size ZLay_Grow(float weight);
ZLAY_API ZLay_EdgeInsets ZLay_EdgeInsetsAll(float value);
ZLAY_API ZLay_EdgeInsets ZLay_EdgeInsetsSymmetric(float horizontal, float vertical);
ZLAY_API ZLay_EdgeInsets ZLay_EdgeInsetsOnly(float left, float top, float right, float bottom);
ZLAY_API ZLay_EdgeInsets ZLay_Insets(float all);
ZLAY_API ZLay_EdgeInsets ZLay_InsetsXY(float x, float y);
ZLAY_API ZLay_EdgeInsets ZLay_InsetsLTRB(float left, float top, float right, float bottom);
ZLAY_API ZLay_Style ZLay_StyleWithPadding(ZLay_Style style, ZLay_EdgeInsets padding);
ZLAY_API ZLay_Style ZLay_StyleWithMargin(ZLay_Style style, ZLay_EdgeInsets margin);
ZLAY_API ZLay_Style ZLay_StyleWithSize(ZLay_Style style, ZLay_Size width, ZLay_Size height);
ZLAY_API ZLay_Style ZLay_StyleWithBackground(ZLay_Style style, ZLay_Color color);
ZLAY_API ZLay_Style ZLay_StyleWithRadius(ZLay_Style style, float radius);
ZLAY_API ZLay_Style ZLay_StyleWithGap(ZLay_Style style, float gap);
ZLAY_API ZLay_Style ZLay_StyleWithDirection(ZLay_Style style, ZLay_FlexDirection direction);
ZLAY_API ZLay_Style ZLay_StyleWithColors(ZLay_Style style, ZLay_Color background, ZLay_Color text);
ZLAY_API ZLay_Style ZLay_RowStyle(ZLay_Size width, ZLay_Size height, float gap);
ZLAY_API ZLay_Style ZLay_ColumnStyle(ZLay_Size width, ZLay_Size height, float gap);
ZLAY_API ZLay_Theme ZLay_ThemeLight(void);
ZLAY_API ZLay_Theme ZLay_ThemeDark(void);
ZLAY_API const ZLay_Theme* ZLay_ThemeDefault(void);
ZLAY_API float ZLay_ComponentControlHeight(ZLay_ComponentSize size, const ZLay_Theme* theme);
ZLAY_API ZLay_EdgeInsets ZLay_ComponentControlPadding(ZLay_ComponentSize size, const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_StackStyle(ZLay_FlexDirection direction, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_Style ZLay_SurfaceStyle(ZLay_SurfaceVariant variant, const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_CardStyle(const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_SpacerStyle(float size, ZLay_Axis axis);
ZLAY_API ZLay_Style ZLay_DividerStyle(ZLay_Axis axis, const ZLay_Theme* theme);

ZLAY_API ZLay_Context* ZLay_Initialize(ZLay_Arena arena, ZLay_Config config);
ZLAY_API void ZLay_SetLayoutDimensions(ZLay_Context* ctx, ZLay_Dimensions dims);

typedef ZLay_Dimensions (*ZLay_MeasureTextFn)(ZLay_String text, void* user);
ZLAY_API void ZLay_SetMeasureTextFunction(ZLay_Context* ctx, ZLay_MeasureTextFn fn, void* user);

ZLAY_API void ZLay_BeginLayout(ZLay_Context* ctx);
ZLAY_API ZLay_RenderCommandList ZLay_EndLayout(ZLay_Context* ctx);

// Low-level open/config/close (used by macros)
ZLAY_API void ZLay__OpenElement(ZLay_Context* ctx, ZLay_Id id, ZLay_NodeType type);
ZLAY_API void ZLay__ConfigureOpenElement(ZLay_Context* ctx, const ZLay_NodeDeclaration* decl);
ZLAY_API void ZLay__CloseElement(ZLay_Context* ctx);
ZLAY_API void ZLay__OpenTextElement(ZLay_Context* ctx, ZLay_Id id, ZLay_String text, const ZLay_Style* style);

// Helpers for macro-friendly struct init
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_BoxDecl(ZLay_Id id, ZLay_Style style);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_TextDecl(ZLay_Id id, ZLay_String text, ZLay_Style style);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_RowDecl(ZLay_Id id, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_ColumnDecl(ZLay_Id id, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_PanelDecl(ZLay_Id id, const ZLay_Theme* theme);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_CardDecl(ZLay_Id id, const ZLay_Theme* theme);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_SpacerDecl(ZLay_Id id, float size, ZLay_Axis axis);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_DividerDecl(ZLay_Id id, ZLay_Axis axis, const ZLay_Theme* theme);

ZLAY_API void ZLay_Spacer(ZLay_Context* ctx, ZLay_Id id, float size, ZLay_Axis axis);
ZLAY_API void ZLay_Divider(ZLay_Context* ctx, ZLay_Id id, ZLay_Axis axis, const ZLay_Theme* theme);

ZLAY_API ZLay_AppConfig ZLay_AppConfigDefault(void);
ZLAY_API bool ZLay_AppInit(ZLay_App* app, ZLay_Context* layout, ZLay_AppConfig config);
ZLAY_API bool ZLay_AppSetLifecycleState(ZLay_App* app, ZLay_AppLifecycleState state);
ZLAY_API void ZLay_AppSetTheme(ZLay_App* app, ZLay_Theme theme);
ZLAY_API const ZLay_Theme* ZLay_AppTheme(const ZLay_App* app);
ZLAY_API void ZLay_AppMarkNeedsBuild(ZLay_App* app);
ZLAY_API bool ZLay_AppNeedsBuild(const ZLay_App* app);
ZLAY_API bool ZLay_AppBeginFrame(ZLay_App* app, ZLay_Dimensions viewport, double delta_ms);
ZLAY_API bool ZLay_AppBuild(ZLay_App* app);
ZLAY_API ZLay_RenderCommandList ZLay_AppEndFrame(ZLay_App* app);
ZLAY_API ZLay_RenderCommandList ZLay_AppRunFrame(ZLay_App* app, ZLay_Dimensions viewport, double delta_ms);

ZLAY_API bool ZLay_NavigatorPush(ZLay_App* app, ZLay_Route route);
ZLAY_API bool ZLay_NavigatorReplace(ZLay_App* app, ZLay_Route route);
ZLAY_API bool ZLay_NavigatorPop(ZLay_App* app);
ZLAY_API bool ZLay_NavigatorCanPop(const ZLay_App* app);
ZLAY_API const ZLay_Route* ZLay_NavigatorCurrent(const ZLay_App* app);

ZLAY_API ZLay_StateValue ZLay_StateNone(void);
ZLAY_API ZLay_StateValue ZLay_StateBool(bool value);
ZLAY_API ZLay_StateValue ZLay_StateInt(int64_t value);
ZLAY_API ZLay_StateValue ZLay_StateFloat(double value);
ZLAY_API ZLay_StateValue ZLay_StateString(ZLay_String value);
ZLAY_API ZLay_StateValue ZLay_StatePointer(void* value);
ZLAY_API ZLay_StateSlot* ZLay_AppUseState(ZLay_App* app, ZLay_Id key, ZLay_StateValue initial_value);
ZLAY_API const ZLay_StateSlot* ZLay_AppFindState(const ZLay_App* app, ZLay_Id key);
ZLAY_API bool ZLay_AppSetStateValue(ZLay_App* app, ZLay_Id key, ZLay_StateValue value);
ZLAY_API bool ZLay_AppClearState(ZLay_App* app, ZLay_Id key);

ZLAY_API void ZLay_AnimationControllerInit(ZLay_AnimationController* controller, double duration_ms);
ZLAY_API void ZLay_AnimationForward(ZLay_AnimationController* controller);
ZLAY_API void ZLay_AnimationReverse(ZLay_AnimationController* controller);
ZLAY_API void ZLay_AnimationStop(ZLay_AnimationController* controller);
ZLAY_API void ZLay_AnimationReset(ZLay_AnimationController* controller);
ZLAY_API bool ZLay_AnimationTick(ZLay_AnimationController* controller, double delta_ms);
ZLAY_API float ZLay_AnimationValue(const ZLay_AnimationController* controller);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ZLAY_ZLAY_H
