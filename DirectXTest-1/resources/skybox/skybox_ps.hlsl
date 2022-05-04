
#include "skybox.hlsli"

TextureCube g_WorldCube : register(t0);
SamplerState g_WorldSampler : register(s0);

float4 main(PSInput ps_input) : SV_Target
{
	return g_WorldCube.Sample(g_WorldSampler, ps_input.PositionL);
}