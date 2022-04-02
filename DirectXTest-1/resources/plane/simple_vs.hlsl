
struct PSInput
{
    float4 PosH : SV_Position;
    float4 Color : Color;
};

struct VSInput
{
    float3 Position : SV_Position;
    float4 Color : Color;
};


cbuffer PlaneWRP : register(b0)
{
    matrix gWorldViewProj;
};

PSInput main(VSInput vs_input)
{
    PSInput ps_input;
    
    ps_input.Color = vs_input.Color;
    ps_input.PosH = mul(float4(vs_input.Position, 1.f), gWorldViewProj);
    
    return ps_input;
}