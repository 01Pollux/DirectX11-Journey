
#include "helper.hlsli"

DSPatchOutput CalcHSPatchConstants(
	InputPatch<HSInput, NUM_CONTROL_POINTS> ip,
	uint patch_id : SV_PrimitiveID
)
{
	float3 center = 0.f;
	[unroll]
	for (uint i = 0; i < NUM_CONTROL_POINTS; i++)
		center += ip[i].PosL;
	
	center = mul(float4(center * .25f, 1.f), g_World).xyz;
	float dx = distance(center, g_EyePosition);

	const float d0 = 20.f, d1 = 100.f, tess_count = 65.f;
	float tess = (max(tess_count * saturate((d1 - dx) / (d1 - d0)), 1.f)) / 3.f;
	
	DSPatchOutput patch;
	
	[unroll]
	for (i = 0; i < 3; i++)
		patch.TessFactor[i] = tess;
	patch.InsideTess = tess;

	return patch;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("CalcHSPatchConstants")]
[maxtessfactor(64.f)]
DSControlPoints main(
	InputPatch<HSInput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint patch_id : SV_PrimitiveID
)
{
	DSControlPoints ds_input;

	ds_input.PosL = ip[i].PosL;

	return ds_input;
}
