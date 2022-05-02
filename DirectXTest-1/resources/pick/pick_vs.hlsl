
#include "helper.hlsli"

PSInput main(VSInput vs_input)
{
	PSInput ps_input;
	
	ps_input.PositionW = mul(float4(vs_input.Position, 1.f), g_World).xyz;
	ps_input.PositionH = mul(float4(ps_input.PositionW, 1.f), g_WorldViewProj);
	
	ps_input.Normal = normalize(mul(vs_input.Normal, (float3x3) g_WorldInv));

	return ps_input;
}