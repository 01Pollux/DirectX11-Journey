
#include "helper.hlsli"

struct PSInput
{
    float4 PosH : SV_Position;
    float3 PosW : Position;
    float3 Normal : Normal;
    float2 TexCoord : TexCoord;
};

struct VSInput
{
    float3 Position : SV_Position;
    float3 Normal : Normal;
    float2 TexCoord : TexCoord;
};



cbuffer WorldConstantBuffer : register(b0)
{
    matrix gWorldViewProj;
    matrix gWorld;
    matrix gWorldInvTranspose;
    
    Material gMaterial;
    DirectionalLight gDirLight;
    
    float4 gFogColor;
    float gFogStartDist;
    
    float3 gWorldEyePosition;
    float gFogEndDist;
};