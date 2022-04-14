
#include "box.hlsli"

Texture2D gBoxTexture0 : register(t0);
Texture2D gBoxTexture1 : register(t1);

SamplerState gBoxSampler;

float4 main(PSInput ps_input) : SV_Target
{
    ps_input.Normal = normalize(ps_input.Normal);
    float3 position_normal = normalize(gWorldEyePosition - ps_input.PosW);
    
    LightRes res;
    Compute_DirectionalLight(gDirLight, gMaterial, ps_input.Normal, position_normal, res);
    
    //float4 sampled = gBoxTexture1.Sample(gBoxSampler, ps_input.TexCoord);
    //float4 sampled = gBoxTexture0.Sample(gBoxSampler, ps_input.TexCoord); 
    float4 sampled = gBoxTexture0.Sample(gBoxSampler, ps_input.TexCoord) * gBoxTexture1.Sample(gBoxSampler, ps_input.TexCoord);
    float4 lit_color = (res.Ambient + res.Diffuse) * sampled + res.Specular;
    lit_color.a = gMaterial.Diffuse.a;
    return lit_color;
}