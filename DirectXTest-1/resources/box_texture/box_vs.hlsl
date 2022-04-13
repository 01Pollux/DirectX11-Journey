
#include "box.hlsli"

PSInput main(VSInput vs_input)
{
    PSInput ps_input = (PSInput) 0;
    
    ps_input.PosH = mul(float4(vs_input.Position, 1.f), gWorldViewProj);
    ps_input.PosW = mul(float4(vs_input.Position, 1.f), gWorld).xyz;
    ps_input.Normal = mul(vs_input.Normal, (float3x3) gWorldInvTranspose);
    ps_input.TexCoord = vs_input.TexCoord;
    
    return ps_input;
}