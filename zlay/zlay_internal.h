#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zlay.h>

#ifndef ZLAY_ASSERT
  #include <assert.h>
  #define ZLAY_ASSERT(x) assert(x)
#endif

typedef struct ZLay_Node {
  ZLay_Id id;
  ZLay_NodeType type;
  ZLay_Style style;
  ZLay_Rect layout;

  // text leaf
  ZLay_String text;

  int32_t parent;
  int32_t first_child;
  int32_t next_sibling;
} ZLay_Node;

struct ZLay_Context {
  ZLay_Config config;
  ZLay_Dimensions viewport;

  ZLay_Arena arena;

  ZLay_MeasureTextFn measure_text;
  void* measure_text_user;

  ZLay_Node* nodes;
  uint32_t node_count;

  ZLay_RenderCommand* commands;
  uint32_t command_count;

  int32_t* open_stack;
  uint32_t open_stack_count;

  int32_t root_index;
};

uint64_t ZLay__HashBytes64(const void* data, size_t size);

void ZLay__ComputeLayout(ZLay_Context* ctx);
void ZLay__BuildRenderCommands(ZLay_Context* ctx);
