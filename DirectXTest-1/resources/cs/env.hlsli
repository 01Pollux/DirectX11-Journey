
struct PSInput
{
    float4 PosH : SV_Position;
    float3 Normal : Normal;
    float2 TexCoord : TexCoord;
};

struct VSInput
{
    float3 Position : Position;
    float3 Normal : Normal;
    float2 TexCoord : TexCoord;
};
