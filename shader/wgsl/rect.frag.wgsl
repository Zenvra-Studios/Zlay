fn zlay_saturate(v: f32) -> f32 {
  return clamp(v, 0.0, 1.0);
}

fn zlay_aa_coverage(signed_distance: f32) -> f32 {
  let width = max(fwidth(signed_distance), 0.0001);
  return zlay_saturate(0.5 - signed_distance / width);
}

fn zlay_rounded_rect_sdf(p: vec2<f32>, half_size: vec2<f32>, radius: f32) -> f32 {
  let q = abs(p) - half_size + vec2<f32>(radius, radius);
  return length(max(q, vec2<f32>(0.0, 0.0))) + min(max(q.x, q.y), 0.0) - radius;
}

struct FSIn {
  @location(0) color: vec4<f32>,
  @location(1) local: vec2<f32>,
  @location(2) half_size: vec2<f32>,
  @location(3) radius: f32,
};

@fragment fn main(input: FSIn) -> @location(0) vec4<f32> {
  let r = min(input.radius, min(input.half_size.x, input.half_size.y));
  let dist = zlay_rounded_rect_sdf(input.local, input.half_size, r);
  let alpha = zlay_aa_coverage(dist);
  return vec4<f32>(input.color.rgb, input.color.a * alpha);
}

