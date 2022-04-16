
#include "box.hlsli"

Texture2D gBoxTexture[2] : register(t0);

SamplerState gBoxSampler;

float4 main(PSInput ps_input) : SV_Target
{
    ps_input.Normal = normalize(ps_input.Normal);
    float3 position_normal = normalize(gWorldEyePosition - ps_input.PosW);
    
    LightRes res;
    Compute_DirectionalLight(gDirLight, gMaterial, ps_input.Normal, position_normal, res);
    
    float4 sampled = gBoxTexture[0].Sample(gBoxSampler, ps_input.TexCoord) * gBoxTexture[1].Sample(gBoxSampler, ps_input.TexCoord);
    float4 lit_color = (res.Ambient + res.Diffuse) * sampled + res.Specular;
    lit_color.a = gMaterial.Diffuse.a * sampled.a;
    return lit_color;
}