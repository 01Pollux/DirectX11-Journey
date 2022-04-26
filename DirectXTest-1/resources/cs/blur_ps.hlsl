
#include "env.hlsli"

Texture2D gBoxTexture : register(t0);
SamplerState gBoxSampler;

float4 main(PSInput ps_input) : SV_Target
{
    float4 v = gBoxTexture.Sample(gBoxSampler, ps_input.TexCoord);
    return v;
}