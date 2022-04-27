
#include "helper.hlsli"

DSPatchOutput CalcHSPatchConstants(
	InputPatch<HSInput, NUM_CONTROL_POINTS> ip,
	uint patch_id : SV_PrimitiveID
)
{
	uint i;
	DSPatchOutput patch;
	
	[unroll]
	for (i = 0; i < 4; i++)
		patch.TessFactor[i] = g_TessFactor[i];
	[unroll]
	for (i = 0; i < 2; i++)
		patch.InsideTess[i] = g_InsideTessFactor[i];

	return patch;
}

[domain("quad")]
[partitioning("integer")]
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
