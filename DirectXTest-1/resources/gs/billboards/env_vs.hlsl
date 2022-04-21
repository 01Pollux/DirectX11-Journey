
#include "env.hlsli"

GSInput main(VSInput vs_input)
{
    GSInput gs_input;
    
    gs_input.PosW = vs_input.PosW;
    gs_input.Normal = vs_input.Normal;
    gs_input.TexCoord = vs_input.TexCoord;
    
    return gs_input;
}