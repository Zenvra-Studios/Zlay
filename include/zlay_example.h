#ifndef ZLAY_EXAMPLE_H
#define ZLAY_EXAMPLE_H

#include <stdlib.h>

#include <zlay.h>
#include <os/zlay_os.h>

typedef struct ZLay_ExampleMemory {
  void* memory;
  size_t size;
  ZLay_Arena arena;
} ZLay_ExampleMemory;

static inline ZLay_ExampleMemory ZLay_ExampleCreateMemory(size_t size) {
  ZLay_ExampleMemory result;
  result.memory = malloc(size);
  result.size = size;
  result.arena = ZLay_CreateArenaWithCapacityAndMemory(size, result.memory);
  return result;
}

static inline void ZLay_ExampleDestroyMemory(ZLay_ExampleMemory* memory) {
  if (!memory) return;
  free(memory->memory);
  memory->memory = 0;
  memory->size = 0;
  memory->arena = ZLay_CreateArenaWithCapacityAndMemory(0, 0);
}

static inline ZLay_Color ZLay_ExampleColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  ZLay_Color color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = a;
  return color;
}

static inline ZLay_Style ZLay_ExampleRootStyle(ZLay_OSStyleInfo style_info) {
  ZLay_Style style = ZLay_StyleDefault();
  style.flex_direction = ZLAY_FLEX_COLUMN;
  style.padding_left = 24.0f;
  style.padding_top = 24.0f;
  style.padding_right = 24.0f;
  style.padding_bottom = 24.0f;
  style.gap = 16.0f;
  style.background = style_info.window_bg;
  return style;
}

#endif
