#include <zlay_antialiasing.h>

#include <zlay_shader_sources.inc>

const char* ZLay_ShaderLanguageName(ZLay_ShaderLanguage language) {
  switch (language) {
    case ZLAY_SHADER_LANGUAGE_GLSL_330: return "GLSL 330";
    case ZLAY_SHADER_LANGUAGE_GLSL_450: return "GLSL 450";
    case ZLAY_SHADER_LANGUAGE_GLSL_ES_300: return "GLSL ES 300 (WebGL 2)";
    case ZLAY_SHADER_LANGUAGE_HLSL_50: return "HLSL 5.0";
    case ZLAY_SHADER_LANGUAGE_SLANG: return "Slang";
    case ZLAY_SHADER_LANGUAGE_WGSL: return "WGSL (WebGPU)";
    default: return "Unknown";
  }
}

const char* ZLay_ShaderStageName(ZLay_ShaderStage stage) {
  switch (stage) {
    case ZLAY_SHADER_STAGE_VERTEX: return "vertex";
    case ZLAY_SHADER_STAGE_FRAGMENT: return "fragment";
    default: return "unknown";
  }
}

const char* ZLay_ShaderProgramName(ZLay_ShaderProgram program) {
  switch (program) {
    case ZLAY_SHADER_PROGRAM_RECT: return "rect";
    case ZLAY_SHADER_PROGRAM_TEXT: return "text";
    default: return "unknown";
  }
}

const char* ZLay_Shader_AntialiasingSource(ZLay_ShaderLanguage language) {
  switch (language) {
    case ZLAY_SHADER_LANGUAGE_GLSL_330:
    case ZLAY_SHADER_LANGUAGE_GLSL_450:
    case ZLAY_SHADER_LANGUAGE_GLSL_ES_300:
      return ZLAY_SHADER_COMMON_ANTIALIASING_GLSL;
    case ZLAY_SHADER_LANGUAGE_HLSL_50:
      return ZLAY_SHADER_COMMON_ANTIALIASING_HLSL;
    case ZLAY_SHADER_LANGUAGE_SLANG:
      return ZLAY_SHADER_COMMON_ANTIALIASING_SLANG;
    case ZLAY_SHADER_LANGUAGE_WGSL:
      return ZLAY_SHADER_COMMON_ANTIALIASING_WGSL;
    default:
      return "";
  }
}

ZLay_ShaderSource ZLay_Shader_RectSource(ZLay_ShaderLanguage language, ZLay_ShaderStage stage) {
  ZLay_ShaderSource result;
  result.language = language;
  result.stage = stage;
  result.program = ZLAY_SHADER_PROGRAM_RECT;
  result.name = "zlay_rect";
  result.source = "";

  if (language == ZLAY_SHADER_LANGUAGE_GLSL_330) {
    result.source = stage == ZLAY_SHADER_STAGE_VERTEX ? ZLAY_SHADER_GLSL_RECT_VERT : ZLAY_SHADER_GLSL_RECT_FRAG;
  } else if (language == ZLAY_SHADER_LANGUAGE_GLSL_450) {
    result.source = stage == ZLAY_SHADER_STAGE_VERTEX ? ZLAY_SHADER_VULKAN_RECT_VERT : ZLAY_SHADER_VULKAN_RECT_FRAG;
  } else if (language == ZLAY_SHADER_LANGUAGE_GLSL_ES_300) {
    result.source = stage == ZLAY_SHADER_STAGE_VERTEX ? ZLAY_SHADER_WEBGL_RECT_VERT : ZLAY_SHADER_WEBGL_RECT_FRAG;
  } else if (language == ZLAY_SHADER_LANGUAGE_HLSL_50) {
    result.source = stage == ZLAY_SHADER_STAGE_VERTEX ? ZLAY_SHADER_HLSL_RECT_VERT : ZLAY_SHADER_HLSL_RECT_FRAG;
  } else if (language == ZLAY_SHADER_LANGUAGE_SLANG) {
    result.source = stage == ZLAY_SHADER_STAGE_VERTEX ? ZLAY_SHADER_SLANG_RECT_VERT : ZLAY_SHADER_SLANG_RECT_FRAG;
  } else if (language == ZLAY_SHADER_LANGUAGE_WGSL) {
    result.source = stage == ZLAY_SHADER_STAGE_VERTEX ? ZLAY_SHADER_WGSL_RECT_VERT : ZLAY_SHADER_WGSL_RECT_FRAG;
  }

  return result;
}
