
#include "helper.hlsli"

struct PSInput
{
    float4 PosH : SV_Position;
    float3 PosW : POSITION;
    float3 Normal : NORMAL;
};

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};



cbuffer WorldConstantBuffer : register(b0)
{
    matrix gWorld;
    matrix gWorldInvTranspose;
    matrix gWorldViewProj;
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