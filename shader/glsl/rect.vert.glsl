#version 330 core

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_rect_pos;
layout(location = 3) in vec2 a_rect_size;
layout(location = 4) in float a_radius;
layout(location = 5) in float a_rounded_exponent;
layout(location = 6) in vec4 a_border_color;
layout(location = 7) in float a_border_width;

uniform vec2 u_viewport;

out vec4 v_color;
out vec2 v_local;
out vec2 v_half_size;
out float v_radius;
out float v_rounded_exponent;
out vec4 v_border_color;
out float v_border_width;

void main() {
  vec2 ndc = vec2((a_pos.x / u_viewport.x) * 2.0 - 1.0, 1.0 - (a_pos.y / u_viewport.y) * 2.0);
  gl_Position = vec4(ndc, 0.0, 1.0);
  v_color = a_color;
  v_half_size = a_rect_size * 0.5;
  v_local = a_pos - (a_rect_pos + v_half_size);
  v_radius = a_radius;
  v_rounded_exponent = a_rounded_exponent;
  v_border_color = a_border_color;
  v_border_width = a_border_width;
}
