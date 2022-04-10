
#include "helper.hlsli"

struct PSInput
{
    float4 PosH : SV_Position;
    float3 PosW : Position;
    float3 Normal : Normal;
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
    float3 gWorldEyePosition;
    int    gLightCount;
    
    Material gMaterial;
    DirectionalLight gDirLight[3];
};