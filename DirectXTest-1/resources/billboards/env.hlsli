
#include "helper.hlsli"

struct PSInput
{
    float4 PosH : SV_Position;
    float3 PosW : Position;
    float3 Normal : Normal;
    float2 TexCoord : TexCoord;
    uint PrimId : SV_PrimitiveID;
};

struct GSInput
{
    float3 PosW : Position;
    float2 Size : Size;
};

struct VSInput
{
    float3 PosW : Position;
    float2 Size : Size;
};


cbuffer WorldConstantBuffer : register(b0)
{
    matrix gViewProj;
    
    Material gMaterial;
    DirectionalLight gDirLight;
    
    float3 gWorldEyePosition;
    int gUsingTexture;
};