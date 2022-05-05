
#include "skybox.hlsli"

PSInput main(VSInput vs_input)
{
	PSInput ps_input;
	ps_input.PositionL = vs_input.Position;
	ps_input.PositionW = mul(float4(ps_input.PositionL, 1.f), g_WorldViewProj).xyww;

	return ps_input;
}