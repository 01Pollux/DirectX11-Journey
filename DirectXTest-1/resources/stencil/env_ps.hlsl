
#include "env.hlsli"

Texture2D gBoxTexture : register(t0);
SamplerState gBoxSampler;

float4 main(PSInput ps_input) : SV_Target
{
    ps_input.Normal = normalize(ps_input.Normal);
    
    float3 to_eye = gWorldEyePosition - ps_input.PosW;
    float dist_to_eye = length(to_eye);
    
    float3 position_normal = to_eye / dist_to_eye;
    
    LightRes res;
    Compute_DirectionalLight(gDirLight, gMaterial, ps_input.Normal, position_normal, res);
    
    float4 sampled = float4(1.f, 1.f, 1.f, 1.f);
    if (gUsingTexture)
    {
        sampled = gBoxTexture.Sample(gBoxSampler, ps_input.TexCoord);
        clip(sampled.a - .1f);
    }
    
    float4 lit_color = (res.Ambient + res.Diffuse) * sampled + res.Specular;
    lit_color.a = gMaterial.Diffuse.a * sampled.a;
    
    return lit_color;
}