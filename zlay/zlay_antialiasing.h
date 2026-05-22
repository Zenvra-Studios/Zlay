#ifndef ZLAY_SHADER_ANTIALIASING_H
#define ZLAY_SHADER_ANTIALIASING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_ShaderLanguage {
  ZLAY_SHADER_LANGUAGE_GLSL_330 = 0,
  ZLAY_SHADER_LANGUAGE_GLSL_450,
  ZLAY_SHADER_LANGUAGE_GLSL_ES_300,
  ZLAY_SHADER_LANGUAGE_HLSL_50,
  ZLAY_SHADER_LANGUAGE_SLANG,
  ZLAY_SHADER_LANGUAGE_WGSL
} ZLay_ShaderLanguage;

typedef enum ZLay_ShaderStage {
  ZLAY_SHADER_STAGE_VERTEX = 0,
  ZLAY_SHADER_STAGE_FRAGMENT
} ZLay_ShaderStage;

typedef enum ZLay_ShaderProgram {
  ZLAY_SHADER_PROGRAM_RECT = 0,
  ZLAY_SHADER_PROGRAM_TEXT
} ZLay_ShaderProgram;

typedef struct ZLay_ShaderSource {
  ZLay_ShaderLanguage language;
  ZLay_ShaderStage stage;
  ZLay_ShaderProgram program;
  const char* name;
  const char* source;
} ZLay_ShaderSource;

ZLAY_API const char* ZLay_ShaderLanguageName(ZLay_ShaderLanguage language);
ZLAY_API const char* ZLay_ShaderStageName(ZLay_ShaderStage stage);
ZLAY_API const char* ZLay_ShaderProgramName(ZLay_ShaderProgram program);
ZLAY_API const char* ZLay_Shader_AntialiasingSource(ZLay_ShaderLanguage language);
ZLAY_API ZLay_ShaderSource ZLay_Shader_RectSource(ZLay_ShaderLanguage language, ZLay_ShaderStage stage);

#ifdef __cplusplus
}
#endif

#endif
