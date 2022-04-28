
#include "helper.hlsli"

[domain("tri")]
PSInput main(
	DSPatchOutput dsp_input,
	float3 uvw : SV_DomainLocation,
	const OutputPatch<DSControlPoints, NUM_CONTROL_POINTS> dsc_input
)
{
	PSInput ps_input;

	float3 v;
	for (uint i = 0; i < NUM_CONTROL_POINTS; i++)
		v += dsc_input[i].PosL * uvw[i % 3];
	v /= 3.f;
	
	//v.z = 3.f * v.z * sin(v.x);
	
	ps_input.PosL = mul(mul(float4(v, 1.f), g_World), g_ViewProj);
	
	return ps_input;
}
