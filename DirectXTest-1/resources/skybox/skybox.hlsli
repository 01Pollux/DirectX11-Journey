
struct VSInput
{
	float3 Position : WorldPosition;
};

struct PSInput
{
	float4 PositionW : SV_Position;
	float3 PositionL : WorldPosition;
};


cbuffer WorldConstantBuffers : register(b0)
{
	matrix g_WorldViewProj;
}