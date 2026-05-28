#ifndef Z_CLAY_BACKEND_H
#define Z_CLAY_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include <zlay.h>

typedef struct ZLay_ClayBackend ZLay_ClayBackend;

ZLAY_API size_t ZLay_ClayBackendMinMemorySize(ZLay_Config config);
ZLAY_API ZLay_ClayBackend* ZLay_ClayBackendCreate(ZLay_Arena arena, ZLay_Config config, ZLay_Dimensions dimensions);

ZLAY_API void ZLay_ClayBackendSetLayoutDimensions(ZLay_ClayBackend* backend, ZLay_Dimensions dimensions);
ZLAY_API void ZLay_ClayBackendSetMeasureTextFunction(ZLay_ClayBackend* backend, ZLay_MeasureTextFn fn, void* user);
ZLAY_API void ZLay_ClayBackendSetPointerState(ZLay_ClayBackend* backend, const ZLay_PointerState* pointer);

ZLAY_API void ZLay_ClayBackendBeginFrame(ZLay_ClayBackend* backend);
ZLAY_API void ZLay_ClayBackendOpenElement(ZLay_ClayBackend* backend, ZLay_Id id, ZLay_NodeType type);
ZLAY_API void ZLay_ClayBackendConfigureOpenElement(ZLay_ClayBackend* backend, const ZLay_NodeDeclaration* declaration);
ZLAY_API void ZLay_ClayBackendCloseElement(ZLay_ClayBackend* backend);
ZLAY_API void ZLay_ClayBackendText(ZLay_ClayBackend* backend, ZLay_Id id, ZLay_String text, const ZLay_Style* style);
ZLAY_API ZLay_RenderCommandList ZLay_ClayBackendEndFrame(ZLay_ClayBackend* backend, float delta_seconds);

ZLAY_API bool ZLay_ClayBackendFindBounds(ZLay_ClayBackend* backend, ZLay_Id id, ZLay_Rect* out_bounds);

#ifdef __cplusplus
}
#endif

#endif
