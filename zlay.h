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

// -----------------------------
// C standard detection
// -----------------------------

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

ZLAY_API uint32_t ZLay_Version(void);
ZLAY_API const char* ZLay_VersionString(void);
ZLAY_API const char* ZLay_LibraryName(void);
ZLAY_API const char* ZLay_VendorName(void);

ZLAY_API ZLay_Id ZLay_IdFromString(ZLay_String s);
ZLAY_API ZLay_Id ZLay_IdFromCString(const char* cstr);

ZLAY_API ZLay_Arena ZLay_CreateArenaWithCapacityAndMemory(size_t capacity, void* memory);
ZLAY_API void ZLay_ArenaReset(ZLay_Arena* arena);
ZLAY_API void* ZLay_ArenaAlloc(ZLay_Arena* arena, size_t size, size_t alignment);

ZLAY_API ZLay_Config ZLay_ConfigDefault(void);
ZLAY_API size_t ZLay_MinMemorySize(ZLay_Config config);

ZLAY_API ZLay_Style ZLay_StyleDefault(void);
ZLAY_API ZLay_Size ZLay_Px(float v);
ZLAY_API ZLay_Size ZLay_Percent(float v);
ZLAY_API ZLay_Size ZLay_Grow(float weight);

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ZLAY_ZLAY_H
