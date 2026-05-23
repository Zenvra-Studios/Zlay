#version 330 core

in vec4 v_color;
in vec2 v_local;
in vec2 v_half_size;
in float v_radius;
in float v_rounded_exponent;
in vec4 v_border_color;
in float v_border_width;

out vec4 frag_color;

float zlay_saturate(float v) { return clamp(v, 0.0, 1.0); }

float zlay_aa_coverage(float signed_distance) {
  float width = max(fwidth(signed_distance), 0.0001);
  return zlay_saturate(0.5 - signed_distance / width);
}

float zlay_rounded_rect_sdf(vec2 p, vec2 half_size, float radius, float exponent) {
  vec2 q = abs(p) - half_size + vec2(radius);
  vec2 outside = max(q, 0.0);
  float e = max(exponent, 2.0);
  float outside_len = e <= 2.0001
    ? length(outside)
    : pow(pow(outside.x, e) + pow(outside.y, e), 1.0 / e);
  return outside_len + min(max(q.x, q.y), 0.0) - radius;
}

float zlay_shadow_alpha(float signed_distance, float blur) {
  float b = max(blur, 0.0001);
  return exp(-(max(signed_distance, 0.0) * max(signed_distance, 0.0)) / (2.0 * b * b));
}

vec4 zlay_box_layer(
  vec2 local,
  vec2 half_size,
  float radius,
  vec4 fill,
  vec4 border,
  float border_width,
  float exponent
) {
  float r = min(radius, min(half_size.x, half_size.y));
  float dist = zlay_rounded_rect_sdf(local, half_size, r, exponent);
  float fill_alpha = zlay_aa_coverage(dist);
  float inner_dist = zlay_rounded_rect_sdf(local, half_size - vec2(border_width), max(r - border_width, 0.0), exponent);
  float border_alpha = zlay_saturate(fill_alpha - zlay_aa_coverage(inner_dist));
  vec4 color = vec4(fill.rgb, fill.a * fill_alpha);
  return mix(color, vec4(border.rgb, border.a), border_alpha * step(0.0001, border_width));
}

void main() {
  float r = min(v_radius, min(v_half_size.x, v_half_size.y));
  frag_color = zlay_box_layer(v_local, v_half_size, r, v_color, v_border_color, v_border_width, v_rounded_exponent);
}
