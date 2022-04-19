
#include "env.hlsli"

GSInput main(VSInput vs_input)
{
    GSInput gs_input;
    
    gs_input.PosW = vs_input.PosW;
    gs_input.Size = vs_input.Size;
    
    return gs_input;
}