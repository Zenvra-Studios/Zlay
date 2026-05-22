float zlay_saturate(float v) {
  return clamp(v, 0.0, 1.0);
}

float zlay_aa_coverage(float signed_distance) {
  float width = max(fwidth(signed_distance), 0.0001);
  return zlay_saturate(0.5 - signed_distance / width);
}

float zlay_rounded_rect_sdf(vec2 p, vec2 half_size, float radius) {
  vec2 q = abs(p) - half_size + vec2(radius);
  return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

