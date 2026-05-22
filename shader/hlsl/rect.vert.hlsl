cbuffer ZLayFrame : register(b0) {
  float2 u_viewport;
};

struct VSIn {
  float2 pos : POSITION;
  float4 color : COLOR0;
  float2 rect_pos : TEXCOORD0;
  float2 rect_size : TEXCOORD1;
  float radius : TEXCOORD2;
};

struct VSOut {
  float4 position : SV_Position;
  float4 color : COLOR0;
  float2 local : TEXCOORD0;
  float2 half_size : TEXCOORD1;
  float radius : TEXCOORD2;
};

VSOut main(VSIn input) {
  VSOut output;
  float2 ndc = float2((input.pos.x / u_viewport.x) * 2.0 - 1.0, 1.0 - (input.pos.y / u_viewport.y) * 2.0);
  output.position = float4(ndc, 0.0, 1.0);
  output.color = input.color;
  output.half_size = input.rect_size * 0.5;
  output.local = input.pos - (input.rect_pos + output.half_size);
  output.radius = input.radius;
  return output;
}

