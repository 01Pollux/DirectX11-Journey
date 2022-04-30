
#include "helper.hlsli"

PSInput main(VSInput vs_input)
{
	PSInput ps_input;

	ps_input.PositionW = mul(float4(vs_input.Position, 1.f), vs_input.World).xyz;
	ps_input.PositionH = mul(float4(ps_input.PositionW, 1.f), g_ViewProj);
	
	ps_input.Normal = normalize(mul(vs_input.Normal, (float3x3) g_ViewProj));
	ps_input.Color = vs_input.Color;

	return ps_input;
}