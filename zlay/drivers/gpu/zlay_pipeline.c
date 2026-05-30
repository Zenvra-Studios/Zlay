#include <drivers/gpu/zlay_pipeline.h>

#include <string.h>

static ZLay_PipelineState ZLay__PipelineState(ZLay_PipelineKind kind, uint64_t generation, bool valid) {
  ZLay_PipelineState state;
  state.kind = kind;
  state.debug_name = ZLay_PipelineKindName(kind);
  state.generation = generation;
  state.valid = valid;
  return state;
}

void ZLay_PipelineCacheInit(ZLay_PipelineCache* cache, const ZLay_DriverCaps* caps) {
  const bool text_atlas = caps != NULL && (caps->features & ZLAY_DRIVER_FEATURE_TEXT_ATLAS) != 0u;

  if (!cache) return;

  memset(cache, 0, sizeof(*cache));
  cache->generation = 1u;
  cache->valid = true;
  cache->rect = ZLay__PipelineState(ZLAY_PIPELINE_RECT, cache->generation, true);
  cache->text = ZLay__PipelineState(ZLAY_PIPELINE_TEXT, cache->generation, text_atlas);
  cache->image = ZLay__PipelineState(ZLAY_PIPELINE_IMAGE, cache->generation, false);
}

void ZLay_PipelineCacheInvalidate(ZLay_PipelineCache* cache) {
  if (!cache) return;
  cache->generation++;
  cache->valid = false;
  cache->rect.valid = false;
  cache->text.valid = false;
  cache->image.valid = false;
}

bool ZLay_PipelineCacheEnsure(ZLay_PipelineCache* cache, ZLay_PipelineKind kind) {
  ZLay_PipelineState* state = NULL;

  if (!cache) return false;

  switch (kind) {
    case ZLAY_PIPELINE_RECT: state = &cache->rect; break;
    case ZLAY_PIPELINE_TEXT: state = &cache->text; break;
    case ZLAY_PIPELINE_IMAGE: state = &cache->image; break;
    case ZLAY_PIPELINE_NONE:
    default: return false;
  }

  cache->valid = true;
  state->kind = kind;
  state->debug_name = ZLay_PipelineKindName(kind);
  state->generation = cache->generation;
  state->valid = true;
  return true;
}

const ZLay_PipelineState* ZLay_PipelineCacheFind(const ZLay_PipelineCache* cache, ZLay_PipelineKind kind) {
  if (!cache) return NULL;

  switch (kind) {
    case ZLAY_PIPELINE_RECT: return &cache->rect;
    case ZLAY_PIPELINE_TEXT: return &cache->text;
    case ZLAY_PIPELINE_IMAGE: return &cache->image;
    case ZLAY_PIPELINE_NONE:
    default: return NULL;
  }
}

const char* ZLay_PipelineKindName(ZLay_PipelineKind kind) {
  switch (kind) {
    case ZLAY_PIPELINE_RECT: return "rect";
    case ZLAY_PIPELINE_TEXT: return "text";
    case ZLAY_PIPELINE_IMAGE: return "image";
    case ZLAY_PIPELINE_NONE:
    default: return "none";
  }
}
