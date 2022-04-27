
#include "helper.hlsli"

DSPatchInput CalcHSPatchConstants(
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
	float tess = max(tess_count * saturate((d1 - dx) / (d1 - d0)), 1.f);
	
	DSPatchInput patch;
	
	[unroll]
	for (i = 0; i < 4; i++)
		patch.TessFactor[i] = tess;
	[unroll]
	for (i = 0; i < 2; i++)
		patch.InsideTess[i] = tess;

	return patch;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("CalcHSPatchConstants")]
[maxtessfactor(64.f)]
DSConstInput main(
	InputPatch<HSInput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint patch_id : SV_PrimitiveID
)
{
	DSConstInput ds_input;

	ds_input.PosL = ip[i].PosL;

	return ds_input;
}
