float zlay_saturate(float v) { return saturate(v); }

float zlay_aa_coverage(float signed_distance) {
  float width = max(fwidth(signed_distance), 0.0001);
  return zlay_saturate(0.5 - signed_distance / width);
}

float zlay_rounded_rect_sdf(float2 p, float2 half_size, float radius) {
  float2 q = abs(p) - half_size + float2(radius, radius);
  return length(max(q, float2(0.0, 0.0))) + min(max(q.x, q.y), 0.0) - radius;
}

struct FSIn {
  float4 position : SV_Position;
  float4 color : COLOR0;
  float2 local : TEXCOORD0;
  float2 half_size : TEXCOORD1;
  float radius : TEXCOORD2;
};

[shader("fragment")]
float4 main(FSIn input) : SV_Target0 {
  float r = min(input.radius, min(input.half_size.x, input.half_size.y));
  float dist = zlay_rounded_rect_sdf(input.local, input.half_size, r);
  float alpha = zlay_aa_coverage(dist);
  return float4(input.color.rgb, input.color.a * alpha);
}

