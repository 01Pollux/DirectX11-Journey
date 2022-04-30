
#include "lights.hlsli"

cbuffer CBWorldInfo
{
	matrix g_ViewProj;
	Material g_Material;
	DirectionalLight g_Light;
	float3 g_WorldEyePosition;
};

struct VSInput
{
	float3 Position : LocalPosition;
	float3 Normal : Normal;
	
	matrix World : World;
	float4 Color : Color;
	
	uint InstId : SV_InstanceID;
};

struct PSInput
{
	float4 PositionH : SV_Position;
	float4 Color : Color;
	
	float3 PositionW : WorldPosition;
	float3 Normal : Normal;
};