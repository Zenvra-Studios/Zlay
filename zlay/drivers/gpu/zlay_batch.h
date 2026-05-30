#ifndef ZLAY_DRIVERS_GPU_ZLAY_BATCH_H
#define ZLAY_DRIVERS_GPU_ZLAY_BATCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>
#include <drivers/gpu/zlay_gpu.h>

typedef enum ZLay_BatchPrimitive {
  ZLAY_BATCH_PRIMITIVE_NONE = 0,
  ZLAY_BATCH_PRIMITIVE_RECT = 1,
  ZLAY_BATCH_PRIMITIVE_TEXT = 2,
  ZLAY_BATCH_PRIMITIVE_CLIP = 3,
  ZLAY_BATCH_PRIMITIVE_LINE = 4,
  ZLAY_BATCH_PRIMITIVE_IMAGE = 5
} ZLay_BatchPrimitive;

typedef struct ZLay_Vertex {
  float x;
  float y;
  float u;
  float v;
  ZLay_ColorU32 color;
  float radius;
  float z_index;
  float extra0;
  float extra1;
} ZLay_Vertex;

typedef struct ZLay_BatchStats {
  uint32_t draw_calls;
  uint32_t rect_count;
  uint32_t line_count;
  uint32_t text_count;
  uint32_t image_count;
  uint32_t clip_count;
  uint32_t vertex_count;
  uint32_t index_count;
  bool overflowed;
} ZLay_BatchStats;

typedef struct ZLay_Batch {
  ZLay_Vertex* vertices;
  uint32_t vertex_count;
  uint32_t vertex_capacity;
  uint32_t* indices;
  uint32_t index_count;
  uint32_t index_capacity;
  uint32_t draw_calls;
  uint32_t rect_count;
  uint32_t line_count;
  uint32_t text_count;
  uint32_t image_count;
  uint32_t clip_count;
  bool overflowed;
} ZLay_Batch;

ZLAY_API void ZLay_BatchInit(
  ZLay_Batch* batch,
  ZLay_Vertex* vertices,
  uint32_t vertex_capacity,
  uint32_t* indices,
  uint32_t index_capacity
);
ZLAY_API void ZLay_BatchReset(ZLay_Batch* batch);
ZLAY_API bool ZLay_BatchCanPush(const ZLay_Batch* batch, uint32_t vertex_count, uint32_t index_count);
ZLAY_API bool ZLay_BatchPushRect(
  ZLay_Batch* batch,
  ZLay_Rect bounds,
  ZLay_Color color,
  float radius,
  int32_t z_index
);
ZLAY_API bool ZLay_BatchPushLine(
  ZLay_Batch* batch,
  ZLay_Vec2 start,
  ZLay_Vec2 end,
  float width,
  ZLay_Color color,
  int32_t z_index
);
ZLAY_API bool ZLay_BatchPushText(
  ZLay_Batch* batch,
  ZLay_Rect bounds,
  ZLay_String text,
  ZLay_Color color,
  int32_t z_index
);
ZLAY_API bool ZLay_BatchPushImage(
  ZLay_Batch* batch,
  ZLay_Rect bounds,
  ZLay_ImageHandle image,
  ZLay_Color tint,
  int32_t z_index
);
ZLAY_API bool ZLay_BatchPushClip(ZLay_Batch* batch);
ZLAY_API bool ZLay_BatchBuildFromCommands(ZLay_Batch* batch, const ZLay_RenderCommandList* commands);
ZLAY_API ZLay_BatchStats ZLay_BatchGetStats(const ZLay_Batch* batch);
ZLAY_API ZLay_GpuStats ZLay_BatchGetGpuStats(const ZLay_Batch* batch);

#ifdef __cplusplus
}
#endif

#endif
