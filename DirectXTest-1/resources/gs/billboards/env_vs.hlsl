
#include "env.hlsli"

GSInput main(VSInput vs_input)
{
    GSInput gs_input;
    
    gs_input.PosW = mul(float4(vs_input.PosW, 1.f), gWorld).xyz;
    gs_input.Size = vs_input.Size;
    
    return gs_input;
}