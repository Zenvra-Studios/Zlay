#include <backend/clay/z_clay_backend.h>

#include <stdint.h>

#define CLAY_IMPLEMENTATION

#include "../../lib/clay.h"

typedef struct ZLay_ClayIdMap {
  uint32_t clay_id;
  ZLay_Id zlay_id;
} ZLay_ClayIdMap;

struct ZLay_ClayBackend {
  ZLay_Config config;
  ZLay_Dimensions dimensions;
  ZLay_MeasureTextFn measure_text;
  void* measure_text_user;
  Clay_Context* clay;
  ZLay_RenderCommand* commands;
  ZLay_ClayIdMap* id_map;
  uint32_t id_map_count;
  uint32_t command_count;
};

static uint32_t ZLay__ClayHashId(ZLay_Id id) {
  uint64_t hash = id.hash;
  hash ^= hash >> 33u;
  hash *= 0xff51afd7ed558ccdull;
  hash ^= hash >> 33u;
  hash *= 0xc4ceb9fe1a85ec53ull;
  hash ^= hash >> 33u;
  return (uint32_t)(hash ? hash : 1u);
}

static Clay_ElementId ZLay__ClayElementId(ZLay_Id id) {
  Clay_ElementId clay_id;
  clay_id.id = ZLay__ClayHashId(id);
  clay_id.offset = 0u;
  clay_id.baseId = 0u;
  clay_id.stringId = (Clay_String){0};
  return clay_id;
}

static Clay_Color ZLay__ToClayColor(ZLay_Color color) {
  return (Clay_Color){(float)color.r, (float)color.g, (float)color.b, (float)color.a};
}

static ZLay_Color ZLay__FromClayColor(Clay_Color color) {
  return (ZLay_Color){
    (uint8_t)ZLay_ClampFloat(color.r, 0.0f, 255.0f),
    (uint8_t)ZLay_ClampFloat(color.g, 0.0f, 255.0f),
    (uint8_t)ZLay_ClampFloat(color.b, 0.0f, 255.0f),
    (uint8_t)ZLay_ClampFloat(color.a, 0.0f, 255.0f)
  };
}

static Clay_Dimensions ZLay__ToClayDimensions(ZLay_Dimensions dimensions) {
  return (Clay_Dimensions){dimensions.width, dimensions.height};
}

static ZLay_Rect ZLay__FromClayBounds(Clay_BoundingBox bounds) {
  return (ZLay_Rect){bounds.x, bounds.y, bounds.width, bounds.height};
}

static Clay_CornerRadius ZLay__ToClayRadius(const ZLay_Style* style) {
  ZLay_Rounded rounded = ZLay_RoundedResolve(style->rounded.radius > 0.0f ? style->rounded : ZLay_RoundedCircular(style->radius));
  return (Clay_CornerRadius){
    rounded.radius,
    rounded.radius,
    rounded.radius,
    rounded.radius
  };
}

static ZLay_Rounded ZLay__FromClayRadius(Clay_CornerRadius radius) {
  float resolved = radius.topLeft;
  resolved = ZLay_MaxFloat(resolved, radius.topRight);
  resolved = ZLay_MaxFloat(resolved, radius.bottomRight);
  resolved = ZLay_MaxFloat(resolved, radius.bottomLeft);
  return ZLay_RoundedCircular(resolved);
}

static uint16_t ZLay__ClayU16(float value) {
  if (value <= 0.0f) return 0u;
  if (value >= 65535.0f) return 65535u;
  return (uint16_t)(value + 0.5f);
}

static Clay_SizingAxis ZLay__ToClaySizing(ZLay_Size size) {
  switch (size.type) {
    case ZLAY_SIZE_PX:
      return (Clay_SizingAxis){.size = {.minMax = {size.value, size.value}}, .type = CLAY__SIZING_TYPE_FIXED};
    case ZLAY_SIZE_PERCENT:
      return (Clay_SizingAxis){.size = {.percent = size.value > 1.0f ? size.value / 100.0f : size.value}, .type = CLAY__SIZING_TYPE_PERCENT};
    case ZLAY_SIZE_GROW:
      return (Clay_SizingAxis){.size = {.minMax = {0.0f, 0.0f}}, .type = CLAY__SIZING_TYPE_GROW};
    case ZLAY_SIZE_AUTO:
    default:
      return (Clay_SizingAxis){.size = {.minMax = {0.0f, 0.0f}}, .type = CLAY__SIZING_TYPE_FIT};
  }
}

static Clay_ElementDeclaration ZLay__ToClayDeclaration(const ZLay_NodeDeclaration* declaration) {
  const ZLay_Style* style = &declaration->style;
  Clay_ElementDeclaration clay = {0};
  clay.layout.sizing.width = ZLay__ToClaySizing(style->width);
  clay.layout.sizing.height = ZLay__ToClaySizing(style->height);
  clay.layout.padding.left = ZLay__ClayU16(style->padding_left);
  clay.layout.padding.right = ZLay__ClayU16(style->padding_right);
  clay.layout.padding.top = ZLay__ClayU16(style->padding_top);
  clay.layout.padding.bottom = ZLay__ClayU16(style->padding_bottom);
  clay.layout.childGap = ZLay__ClayU16(style->gap);
  clay.layout.layoutDirection = style->flex_direction == ZLAY_FLEX_COLUMN ? CLAY_TOP_TO_BOTTOM : CLAY_LEFT_TO_RIGHT;
  clay.backgroundColor = ZLay__ToClayColor(style->background);
  clay.cornerRadius = ZLay__ToClayRadius(style);
  clay.border.color = ZLay__ToClayColor(style->border_color);
  clay.border.width.left = ZLay__ClayU16(style->border_width);
  clay.border.width.right = ZLay__ClayU16(style->border_width);
  clay.border.width.top = ZLay__ClayU16(style->border_width);
  clay.border.width.bottom = ZLay__ClayU16(style->border_width);
  clay.userData = (void*)(uintptr_t)declaration->id.hash;

  if (style->position == ZLAY_POSITION_ABSOLUTE || style->position == ZLAY_POSITION_FLOATING) {
    clay.floating.offset = (Clay_Vector2){style->left, style->top};
    clay.floating.attachTo = style->position == ZLAY_POSITION_FLOATING ? CLAY_ATTACH_TO_ROOT : CLAY_ATTACH_TO_PARENT;
  }

  return clay;
}

static Clay_Dimensions ZLay__ClayMeasureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* user) {
  ZLay_ClayBackend* backend = (ZLay_ClayBackend*)user;
  ZLay_String string;
  (void)config;

  string.chars = text.chars;
  string.length = text.length > 0 ? (uint32_t)text.length : 0u;
  string.is_static = false;

  if (backend && backend->measure_text) {
    return ZLay__ToClayDimensions(backend->measure_text(string, backend->measure_text_user));
  }

  return (Clay_Dimensions){8.0f * (float)string.length, 16.0f};
}

static void ZLay__ClayErrorHandler(Clay_ErrorData error) {
  (void)error;
}

static void ZLay__ClaySetCurrent(ZLay_ClayBackend* backend) {
  if (backend && backend->clay) {
    Clay_SetCurrentContext(backend->clay);
  }
}

static void ZLay__ClayRememberId(ZLay_ClayBackend* backend, uint32_t clay_id, ZLay_Id zlay_id) {
  uint32_t i;
  if (!backend || clay_id == 0u || zlay_id.hash == 0u) return;

  for (i = 0u; i < backend->id_map_count; ++i) {
    if (backend->id_map[i].clay_id == clay_id) {
      backend->id_map[i].zlay_id = zlay_id;
      return;
    }
  }

  if (backend->id_map_count < backend->config.max_nodes) {
    backend->id_map[backend->id_map_count++] = (ZLay_ClayIdMap){clay_id, zlay_id};
  }
}

static ZLay_Id ZLay__ClayResolveId(ZLay_ClayBackend* backend, uint32_t clay_id) {
  uint32_t i;
  if (backend) {
    for (i = 0u; i < backend->id_map_count; ++i) {
      if (backend->id_map[i].clay_id == clay_id) {
        return backend->id_map[i].zlay_id;
      }
    }
  }
  return (ZLay_Id){clay_id};
}

static void ZLay__ClayPushCommand(ZLay_ClayBackend* backend, const ZLay_RenderCommand* command) {
  if (!backend || !command || backend->command_count >= backend->config.max_commands) return;
  backend->commands[backend->command_count++] = *command;
}

static void ZLay__ClayConvertCommand(ZLay_ClayBackend* backend, const Clay_RenderCommand* clay_command) {
  ZLay_RenderCommand command;
  if (!backend || !clay_command) return;

  command = (ZLay_RenderCommand){0};
  command.id = ZLay__ClayResolveId(backend, clay_command->id);
  if (clay_command->userData) {
    command.id.hash = (uint64_t)(uintptr_t)clay_command->userData;
  }
  command.bounds = ZLay__FromClayBounds(clay_command->boundingBox);
  command.z_index = clay_command->zIndex;
  command.pointer_cursor = ZLAY_POINTER_CURSOR_DEFAULT;
  command.rounded = ZLay_RoundedCircular(0.0f);

  switch (clay_command->commandType) {
    case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
      command.type = ZLAY_CMD_RECT;
      command.color = ZLay__FromClayColor(clay_command->renderData.rectangle.backgroundColor);
      command.rounded = ZLay__FromClayRadius(clay_command->renderData.rectangle.cornerRadius);
      command.radius = ZLay_RoundedEffectiveRadius(command.rounded, command.bounds.width, command.bounds.height);
      ZLay__ClayPushCommand(backend, &command);
      break;
    case CLAY_RENDER_COMMAND_TYPE_BORDER:
      command.type = ZLAY_CMD_RECT;
      command.color = (ZLay_Color){0};
      command.border_color = ZLay__FromClayColor(clay_command->renderData.border.color);
      command.border_width = (float)clay_command->renderData.border.width.left;
      command.rounded = ZLay__FromClayRadius(clay_command->renderData.border.cornerRadius);
      command.radius = ZLay_RoundedEffectiveRadius(command.rounded, command.bounds.width, command.bounds.height);
      ZLay__ClayPushCommand(backend, &command);
      break;
    case CLAY_RENDER_COMMAND_TYPE_TEXT:
      command.type = ZLAY_CMD_TEXT;
      command.color = ZLay__FromClayColor(clay_command->renderData.text.textColor);
      command.text.chars = clay_command->renderData.text.stringContents.chars;
      command.text.length = clay_command->renderData.text.stringContents.length > 0 ? (uint32_t)clay_command->renderData.text.stringContents.length : 0u;
      command.text.is_static = false;
      command.font_size = (float)clay_command->renderData.text.fontSize;
      ZLay__ClayPushCommand(backend, &command);
      break;
    case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
      command.type = ZLAY_CMD_CLIP_BEGIN;
      ZLay__ClayPushCommand(backend, &command);
      break;
    case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
      command.type = ZLAY_CMD_CLIP_END;
      ZLay__ClayPushCommand(backend, &command);
      break;
    default:
      break;
  }
}

size_t ZLay_ClayBackendMinMemorySize(ZLay_Config config) {
  Clay_Context* old_context = Clay_GetCurrentContext();
  uint32_t clay_bytes;
  if (config.max_nodes == 0u) config.max_nodes = ZLay_ConfigDefault().max_nodes;
  if (config.max_commands == 0u) config.max_commands = ZLay_ConfigDefault().max_commands;

  Clay_SetCurrentContext(NULL);
  Clay_SetMaxElementCount((int32_t)config.max_nodes);
  Clay_SetMaxMeasureTextCacheWordCount((int32_t)(config.max_nodes * 2u));
  clay_bytes = Clay_MinMemorySize();
  Clay_SetCurrentContext(old_context);

  return sizeof(ZLay_ClayBackend)
    + ((size_t)config.max_commands * sizeof(ZLay_RenderCommand))
    + ((size_t)config.max_nodes * sizeof(ZLay_ClayIdMap))
    + clay_bytes
    + 256u;
}

ZLay_ClayBackend* ZLay_ClayBackendCreate(ZLay_Arena arena, ZLay_Config config, ZLay_Dimensions dimensions) {
  ZLay_Arena cursor = arena;
  ZLay_ClayBackend* backend;
  void* clay_memory;
  uint32_t clay_bytes;
  Clay_Arena clay_arena;
  Clay_ErrorHandler error_handler;
  Clay_Context* old_context;

  if (config.max_nodes == 0u || config.max_commands == 0u || config.max_stack == 0u) {
    config = ZLay_ConfigDefault();
  }

  backend = (ZLay_ClayBackend*)ZLay_ArenaAlloc(&cursor, sizeof(ZLay_ClayBackend), 16u);
  if (!backend) return NULL;
  *backend = (ZLay_ClayBackend){0};
  backend->config = config;
  backend->dimensions = dimensions;
  backend->commands = (ZLay_RenderCommand*)ZLay_ArenaAlloc(&cursor, (size_t)config.max_commands * sizeof(ZLay_RenderCommand), 16u);
  backend->id_map = (ZLay_ClayIdMap*)ZLay_ArenaAlloc(&cursor, (size_t)config.max_nodes * sizeof(ZLay_ClayIdMap), 16u);

  old_context = Clay_GetCurrentContext();
  Clay_SetCurrentContext(NULL);
  Clay_SetMaxElementCount((int32_t)config.max_nodes);
  Clay_SetMaxMeasureTextCacheWordCount((int32_t)(config.max_nodes * 2u));
  clay_bytes = Clay_MinMemorySize();
  clay_memory = ZLay_ArenaAlloc(&cursor, clay_bytes, 64u);
  if (!backend->commands || !backend->id_map || !clay_memory) {
    Clay_SetCurrentContext(old_context);
    return NULL;
  }

  clay_arena = Clay_CreateArenaWithCapacityAndMemory(clay_bytes, clay_memory);
  error_handler.errorHandlerFunction = ZLay__ClayErrorHandler;
  error_handler.userData = backend;
  backend->clay = Clay_Initialize(clay_arena, ZLay__ToClayDimensions(dimensions), error_handler);
  if (!backend->clay) {
    Clay_SetCurrentContext(old_context);
    return NULL;
  }
  Clay_SetMeasureTextFunction(ZLay__ClayMeasureText, backend);
  Clay_SetCurrentContext(old_context);
  return backend;
}

void ZLay_ClayBackendSetLayoutDimensions(ZLay_ClayBackend* backend, ZLay_Dimensions dimensions) {
  if (!backend) return;
  backend->dimensions = dimensions;
  ZLay__ClaySetCurrent(backend);
  Clay_SetLayoutDimensions(ZLay__ToClayDimensions(dimensions));
}

void ZLay_ClayBackendSetMeasureTextFunction(ZLay_ClayBackend* backend, ZLay_MeasureTextFn fn, void* user) {
  if (!backend) return;
  backend->measure_text = fn;
  backend->measure_text_user = user;
  ZLay__ClaySetCurrent(backend);
  Clay_SetMeasureTextFunction(ZLay__ClayMeasureText, backend);
}

void ZLay_ClayBackendSetPointerState(ZLay_ClayBackend* backend, const ZLay_PointerState* pointer) {
  if (!backend || !pointer) return;
  ZLay__ClaySetCurrent(backend);
  Clay_SetPointerState((Clay_Vector2){pointer->x, pointer->y}, pointer->left_down);
}

void ZLay_ClayBackendBeginFrame(ZLay_ClayBackend* backend) {
  if (!backend) return;
  backend->command_count = 0u;
  backend->id_map_count = 0u;
  ZLay__ClaySetCurrent(backend);
  Clay_SetLayoutDimensions(ZLay__ToClayDimensions(backend->dimensions));
  Clay_BeginLayout();
}

void ZLay_ClayBackendOpenElement(ZLay_ClayBackend* backend, ZLay_Id id, ZLay_NodeType type) {
  Clay_ElementId clay_id;
  (void)type;
  if (!backend) return;
  ZLay__ClaySetCurrent(backend);
  clay_id = ZLay__ClayElementId(id);
  ZLay__ClayRememberId(backend, clay_id.id, id);
  Clay__OpenElementWithId(clay_id);
}

void ZLay_ClayBackendConfigureOpenElement(ZLay_ClayBackend* backend, const ZLay_NodeDeclaration* declaration) {
  Clay_ElementDeclaration clay_declaration;
  if (!backend || !declaration) return;
  ZLay__ClaySetCurrent(backend);
  clay_declaration = ZLay__ToClayDeclaration(declaration);
  ZLay__ClayRememberId(backend, ZLay__ClayHashId(declaration->id), declaration->id);
  Clay__ConfigureOpenElement(clay_declaration);
}

void ZLay_ClayBackendCloseElement(ZLay_ClayBackend* backend) {
  if (!backend) return;
  ZLay__ClaySetCurrent(backend);
  Clay__CloseElement();
}

void ZLay_ClayBackendText(ZLay_ClayBackend* backend, ZLay_Id id, ZLay_String text, const ZLay_Style* style) {
  Clay_TextElementConfig config;
  Clay_ElementId clay_id;
  Clay_String clay_text;
  ZLay_Style default_style;

  if (!backend) return;
  default_style = ZLay_StyleDefault();
  if (!style) style = &default_style;

  ZLay__ClaySetCurrent(backend);
  clay_id = ZLay__ClayElementId(id);
  ZLay__ClayRememberId(backend, clay_id.id, id);
  Clay__OpenElementWithId(clay_id);
  Clay__ConfigureOpenElement((Clay_ElementDeclaration){
    .layout = {
      .sizing = {
        .width = ZLay__ToClaySizing(style->width),
        .height = ZLay__ToClaySizing(style->height)
      }
    },
    .userData = (void*)(uintptr_t)id.hash
  });
  config = (Clay_TextElementConfig){0};
  config.textColor = ZLay__ToClayColor(style->text_color);
  config.fontSize = ZLay__ClayU16(style->font_size > 0.0f ? style->font_size : 16.0f);
  config.lineHeight = config.fontSize;
  config.wrapMode = CLAY_TEXT_WRAP_WORDS;
  config.userData = (void*)(uintptr_t)id.hash;
  clay_text.isStaticallyAllocated = text.is_static;
  clay_text.length = (int32_t)text.length;
  clay_text.chars = text.chars;
  Clay__OpenTextElement(clay_text, config);
  Clay__CloseElement();
}

ZLay_RenderCommandList ZLay_ClayBackendEndFrame(ZLay_ClayBackend* backend, float delta_seconds) {
  Clay_RenderCommandArray clay_commands;
  int32_t i;
  if (!backend) return (ZLay_RenderCommandList){0};

  backend->command_count = 0u;
  ZLay__ClaySetCurrent(backend);
  clay_commands = Clay_EndLayout(delta_seconds);
  for (i = 0; i < clay_commands.length; ++i) {
    Clay_RenderCommand* clay_command = Clay_RenderCommandArray_Get(&clay_commands, i);
    ZLay__ClayConvertCommand(backend, clay_command);
  }

  return (ZLay_RenderCommandList){backend->commands, backend->command_count};
}

bool ZLay_ClayBackendFindBounds(ZLay_ClayBackend* backend, ZLay_Id id, ZLay_Rect* out_bounds) {
  Clay_ElementData data;
  if (!backend || !out_bounds) return false;
  ZLay__ClaySetCurrent(backend);
  data = Clay_GetElementData(ZLay__ClayElementId(id));
  if (!data.found) return false;
  *out_bounds = ZLay__FromClayBounds(data.boundingBox);
  return true;
}
