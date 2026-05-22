#pragma once

#include <stdint.h>

// Minimal SIMD helpers (SSE2 / NEON) used for geometry building.

#if !defined(ZLAY_SIMD_NONE)
  #if defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(_M_X64)
    #define ZLAY_SIMD_SSE2 1
    #include <emmintrin.h>
  #elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    #define ZLAY_SIMD_NEON 1
    #include <arm_neon.h>
  #endif
#endif

static inline void ZLay__BuildQuadXY(float x, float y, float w, float h, float out_x[4], float out_y[4]) {
#if defined(ZLAY_SIMD_SSE2)
  const __m128 vx = _mm_setr_ps(x, x + w, x + w, x);
  const __m128 vy = _mm_setr_ps(y, y, y + h, y + h);
  _mm_storeu_ps(out_x, vx);
  _mm_storeu_ps(out_y, vy);
#elif defined(ZLAY_SIMD_NEON)
  const float ax[4] = {0.0f, w, w, 0.0f};
  const float ay[4] = {0.0f, 0.0f, h, h};
  const float32x4_t bx = vdupq_n_f32(x);
  const float32x4_t by = vdupq_n_f32(y);
  const float32x4_t vx = vaddq_f32(bx, vld1q_f32(ax));
  const float32x4_t vy = vaddq_f32(by, vld1q_f32(ay));
  vst1q_f32(out_x, vx);
  vst1q_f32(out_y, vy);
#else
  out_x[0] = x;
  out_y[0] = y;

  out_x[1] = x + w;
  out_y[1] = y;

  out_x[2] = x + w;
  out_y[2] = y + h;

  out_x[3] = x;
  out_y[3] = y + h;
#endif
}

