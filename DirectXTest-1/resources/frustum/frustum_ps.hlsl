
#include "helper.hlsli"

float4 main(PSInput ps_input) : SV_TARGET
{
	float3 to_eye = g_WorldEyePosition - ps_input.PositionW;
	float dist = length(to_eye);
	
	to_eye /= dist;
	
	LightRes lit_color;
	Compute_DirectionalLight(
		g_Light,
		g_Material,
		ps_input.Normal,
		to_eye,
		lit_color
	);
	
	float4 out_color = ps_input.Color * (lit_color.Ambient + lit_color.Diffuse + lit_color.Specular);
	out_color.a = g_Material.Diffuse.a;
	
	return out_color;
}