#include "zlay_internal.h"

// Simple, stable 64-bit hash (FNV-1a).
// SIMD acceleration can be added later without changing the public API.
uint64_t ZLay__HashBytes64(const void* data, size_t size) {
  const uint8_t* p = (const uint8_t*)data;
  uint64_t h = 1469598103934665603ull;
  for (size_t i = 0; i < size; ++i) {
    h ^= (uint64_t)p[i];
    h *= 1099511628211ull;
  }
  return h;
}

ZLay_Id ZLay_IdFromString(ZLay_String s) {
  ZLay_Id id;
  id.hash = ZLay__HashBytes64(s.chars, (size_t)s.length);
  return id;
}

ZLay_Id ZLay_IdFromCString(const char* cstr) {
  if (!cstr) return (ZLay_Id){0};
  const char* p = cstr;
  while (*p) ++p;
  return ZLay_IdFromString((ZLay_String){cstr, (uint32_t)(p - cstr), false});
}

