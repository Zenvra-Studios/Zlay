struct ZLayFrame {
  viewport: vec2<f32>,
};

@group(0) @binding(0) var<uniform> u_frame: ZLayFrame;

struct VSIn {
  @location(0) pos: vec2<f32>,
  @location(1) color: vec4<f32>,
  @location(2) rect_pos: vec2<f32>,
  @location(3) rect_size: vec2<f32>,
  @location(4) radius: f32,
};

struct VSOut {
  @builtin(position) position: vec4<f32>,
  @location(0) color: vec4<f32>,
  @location(1) local: vec2<f32>,
  @location(2) half_size: vec2<f32>,
  @location(3) radius: f32,
};

@vertex fn main(input: VSIn) -> VSOut {
  var out: VSOut;
  let ndc = vec2<f32>((input.pos.x / u_frame.viewport.x) * 2.0 - 1.0, 1.0 - (input.pos.y / u_frame.viewport.y) * 2.0);
  out.position = vec4<f32>(ndc, 0.0, 1.0);
  out.color = input.color;
  out.half_size = input.rect_size * 0.5;
  out.local = input.pos - (input.rect_pos + out.half_size);
  out.radius = input.radius;
  return out;
}

