#version 300 es

precision highp float;

in vec4 v_color;
in vec2 v_local;
in vec2 v_half_size;
in float v_radius;

out vec4 frag_color;

float zlay_saturate(float v) { return clamp(v, 0.0, 1.0); }

float zlay_aa_coverage(float signed_distance) {
  float width = max(fwidth(signed_distance), 0.0001);
  return zlay_saturate(0.5 - signed_distance / width);
}

float zlay_rounded_rect_sdf(vec2 p, vec2 half_size, float radius) {
  vec2 q = abs(p) - half_size + vec2(radius);
  return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0) - radius;
}

void main() {
  float r = min(v_radius, min(v_half_size.x, v_half_size.y));
  float dist = zlay_rounded_rect_sdf(v_local, v_half_size, r);
  float alpha = zlay_aa_coverage(dist);
  frag_color = vec4(v_color.rgb, v_color.a * alpha);
}

