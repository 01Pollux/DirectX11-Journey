
#include "lights.hlsli"

cbuffer CBWorldInfo
{
	matrix g_World;
	matrix g_WorldInv;
	matrix g_WorldViewProj;
	
	Material g_Material;
	DirectionalLight g_Light;
	float3 g_WorldEyePosition;
};

struct VSInput
{
	float3 Position : LocalPosition;
	float3 Normal : Normal;
};

struct PSInput
{
	float4 PositionH : SV_Position;
	float3 PositionW : WorldPosition;
	float3 Normal : Normal;
};