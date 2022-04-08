
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
    Material gMaterial;
};

cbuffer LightConstantBuffer : register(b1)
{
    DirectionalLight gDirLight;
    PointLight gPtLight;
    SpotLight gSpLight;
    
    float3 gWorldEyePosition;
    uint gLightType;
};