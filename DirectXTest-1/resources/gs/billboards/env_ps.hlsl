
#include "env.hlsli"

Texture2DArray gTex_Billboard : register(t0);
SamplerState gLinear_Billboard : register(s0);

float4 main(PSInput ps_input) : SV_Target
{
    float3 to_eye = gWorldEyePosition - ps_input.PosW;
    float dist_to_eye = length(to_eye);
    
    float3 position_normal = to_eye / dist_to_eye;
    
    float4 sampled = gTex_Billboard.Sample(gLinear_Billboard, float3(ps_input.TexCoord, ps_input.PrimId % 4));
    clip(sampled.a - .1f);
    
    LightRes res;
    Compute_DirectionalLight(gDirLight, gMaterial, ps_input.Normal, position_normal, res);
    
    float4 lit_color = (res.Ambient + res.Diffuse) * sampled + res.Specular;
    lit_color.a = gMaterial.Diffuse.a * sampled.a;
    
    return lit_color;
}