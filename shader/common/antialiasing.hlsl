float zlay_saturate(float v) {
  return saturate(v);
}

float zlay_aa_coverage(float signed_distance) {
  float width = max(fwidth(signed_distance), 0.0001);
  return zlay_saturate(0.5 - signed_distance / width);
}

float zlay_rounded_rect_sdf(float2 p, float2 half_size, float radius) {
  float2 q = abs(p) - half_size + float2(radius, radius);
  return length(max(q, float2(0.0, 0.0))) + min(max(q.x, q.y), 0.0) - radius;
}

