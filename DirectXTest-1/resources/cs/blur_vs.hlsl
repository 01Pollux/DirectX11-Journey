
#include "env.hlsli"

PSInput main(VSInput vs_input)
{
    PSInput ps_input = (PSInput) 0;
    
    ps_input.PosH = float4(vs_input.Position, 1.f);
    ps_input.Normal = vs_input.Normal;
    ps_input.TexCoord = vs_input.TexCoord;
    
    return ps_input;
}