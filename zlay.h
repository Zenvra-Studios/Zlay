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

#define ZLAY_VERSION_MAJOR 0
#define ZLAY_VERSION_MINOR 1
#define ZLAY_VERSION_PATCH 0
#define ZLAY_VERSION_STRING "0.1.0"
#define ZLAY_MAKE_VERSION(major, minor, patch) (((major) * 10000) + ((minor) * 100) + (patch))
#define ZLAY_VERSION ZLAY_MAKE_VERSION(ZLAY_VERSION_MAJOR, ZLAY_VERSION_MINOR, ZLAY_VERSION_PATCH)

// -----------------------------
// DLL export
// -----------------------------

#ifndef ZLAY_API
  #if defined(_WIN32) && defined(ZLAY_SHARED)
    #if defined(ZLAY_BUILDING)
      #define ZLAY_API __declspec(dllexport)
    #else
      #define ZLAY_API __declspec(dllimport)
    #endif
  #elif defined(ZLAY_SHARED) && defined(__GNUC__)
    #define ZLAY_API __attribute__((visibility("default")))
  #else
    #define ZLAY_API
  #endif
#endif

ZLAY_API uint32_t ZLay_Version(void);
ZLAY_API const char* ZLay_VersionString(void);
ZLAY_API const char* ZLay_LibraryName(void);
ZLAY_API const char* ZLay_VendorName(void);

// -----------------------------
// Common types
// -----------------------------

typedef struct ZLay_Vec2 {
  float x;
  float y;
} ZLay_Vec2;

typedef struct ZLay_Dimensions {
  float width;
  float height;
} ZLay_Dimensions;

typedef struct ZLay_Rect {
  float x;
  float y;
  float width;
  float height;
} ZLay_Rect;

typedef struct ZLay_Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} ZLay_Color;

typedef struct ZLay_String {
  const char* chars;
  uint32_t length;
  bool is_static;
} ZLay_String;

#define ZLAY_STRING_LITERAL(str_lit) \
  ((ZLay_String){ (str_lit), (uint32_t)(sizeof(str_lit) - 1u), true })

// -----------------------------
// ID
// -----------------------------

typedef struct ZLay_Id {
  uint64_t hash;
} ZLay_Id;

ZLAY_API ZLay_Id ZLay_IdFromString(ZLay_String s);
ZLAY_API ZLay_Id ZLay_IdFromCString(const char* cstr);

#define ZLAY_ID(str_lit) ZLay_IdFromString(ZLAY_STRING_LITERAL(str_lit))

// -----------------------------
// Arena
// -----------------------------

typedef struct ZLay_Arena {
  uint8_t* memory;
  size_t capacity;
  size_t offset;
} ZLay_Arena;

ZLAY_API ZLay_Arena ZLay_CreateArenaWithCapacityAndMemory(size_t capacity, void* memory);
ZLAY_API void ZLay_ArenaReset(ZLay_Arena* arena);
ZLAY_API void* ZLay_ArenaAlloc(ZLay_Arena* arena, size_t size, size_t alignment);

// -----------------------------
// Config
// -----------------------------

typedef struct ZLay_Config {
  uint32_t max_nodes;
  uint32_t max_commands;
  uint32_t max_stack;
} ZLay_Config;

ZLAY_API ZLay_Config ZLay_ConfigDefault(void);
ZLAY_API size_t ZLay_MinMemorySize(ZLay_Config config);

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
  float radius;
} ZLay_Style;

ZLAY_API ZLay_Style ZLay_StyleDefault(void);
ZLAY_API ZLay_Size ZLay_Px(float v);
ZLAY_API ZLay_Size ZLay_Percent(float v);
ZLAY_API ZLay_Size ZLay_Grow(float weight);

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

// Declarative macro API (Clay-like)
// Usage:
//   ZLay_BeginLayout(ctx);
//   ZLAY(ctx, ZLAY_ID("Root"), ZLAY_BOX(.style = ...)) { ... }
//   ZLay_RenderCommandList cmds = ZLay_EndLayout(ctx);

typedef struct ZLay_NodeDeclarationBuilder {
  ZLay_NodeDeclaration decl;
} ZLay_NodeDeclarationBuilder;

// Helpers for macro-friendly struct init
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_BoxDecl(ZLay_Id id, ZLay_Style style);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_TextDecl(ZLay_Id id, ZLay_String text, ZLay_Style style);

#if defined(__cplusplus)
  #define ZLAY_BOX(...) (ZLay_NodeDeclarationBuilder{ .decl = ZLay_NodeDeclaration{ .type = ZLAY_NODE_BOX, __VA_ARGS__ } })
  #define ZLAY_TEXT_NODE(...) (ZLay_NodeDeclarationBuilder{ .decl = ZLay_NodeDeclaration{ .type = ZLAY_NODE_TEXT, __VA_ARGS__ } })
#else
  #define ZLAY_BOX(...) ((ZLay_NodeDeclarationBuilder){ .decl = (ZLay_NodeDeclaration){ .type = ZLAY_NODE_BOX, __VA_ARGS__ } })
  #define ZLAY_TEXT_NODE(...) ((ZLay_NodeDeclarationBuilder){ .decl = (ZLay_NodeDeclaration){ .type = ZLAY_NODE_TEXT, __VA_ARGS__ } })
#endif

#define ZLAY(ctx, id, builder_expr) \
  for (ZLay_NodeDeclarationBuilder ZLAY__builder = (builder_expr), *ZLAY__builder_ptr = &ZLAY__builder; ZLAY__builder_ptr; ZLAY__builder_ptr = 0) \
    for (int ZLAY__once = (ZLay__OpenElement((ctx), (id), ZLAY__builder.decl.type), ZLay__ConfigureOpenElement((ctx), &ZLAY__builder.decl), 0); ZLAY__once == 0; ZLAY__once = (ZLay__CloseElement((ctx)), 1))

#define ZLAY_TEXT(ctx, id, text, style_ptr) \
  do { ZLay__OpenTextElement((ctx), (id), (text), (style_ptr)); } while (0)

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ZLAY_ZLAY_H
