
#include "skull_demo.hlsli"

PSInput main(VSInput vs_input)
{
    PSInput ps_input = (PSInput) 0;
    
    ps_input.PosH = mul(float4(vs_input.Position, 1.f), gWorld);
    ps_input.PosW = mul(float4(vs_input.Position, 1.f), gWorldViewProj).xyz;
    ps_input.Normal = mul(vs_input.Normal, (float3x3) gWorldInvTranspose);
    
    return ps_input;
}