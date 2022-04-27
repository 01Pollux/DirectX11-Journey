
#include "helper.hlsli"

[domain("quad")]
PSInput main(
	DSPatchOutput patch,
	float2 uv : SV_DomainLocation,
	const OutputPatch<DSControlPoints, NUM_CONTROL_POINTS> dsc_points
)
{
	PSInput ps_input;
	
	float4 basisu = CalcCubicBezierBasis(uv.x);
	float4 basisv = CalcCubicBezierBasis(uv.y);
	
	float4 v = float4(CalcCubicBezier(dsc_points, basisu, basisv), 1.f);
	ps_input.PosL = mul(mul(v, g_World), g_ViewProj);

	return ps_input;
}