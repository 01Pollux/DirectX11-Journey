
#include "helper.hlsli"

[domain("tri")]
PSInput main(
	DSPatchOutput dsp_input,
	float3 uv : SV_DomainLocation,
	const OutputPatch<DSControlPoints, NUM_CONTROL_POINTS> dsc_input
)
{
	PSInput ps_input;

	float3 v1 = lerp(dsc_input[0].PosL, dsc_input[1].PosL, uv.x);
	float3 v2 = lerp(dsc_input[2].PosL, dsc_input[0].PosL, uv.x);
	float3 v = lerp(v1, v2, uv.y);
	
	//v.y = .3f * (v.x * cos(v.z) + v.z * sin(v.x));
	
	ps_input.PosL = mul(mul(float4(v, 1.f), g_World), g_ViewProj);
	
	return ps_input;
}
