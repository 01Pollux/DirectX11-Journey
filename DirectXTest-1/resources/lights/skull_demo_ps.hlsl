
#include "skull_demo.hlsli"

float4 main(PSInput ps_input) : SV_Target
{
    ps_input.Normal = normalize(ps_input.Normal);
    float3 position_normal = normalize(gWorldEyePosition - ps_input.PosW);
    
    LightRes light;
    LightRes res = (LightRes) 0;
    for (int i = 0; i < gLightCount; i++)
    {
        Compute_DirectionalLight(gDirLight[i], gMaterial, ps_input.Normal, position_normal, light);
        res.Ambient += light.Ambient;
        res.Diffuse += light.Diffuse;
        res.Specular += light.Specular;
    }
    
    float4 lit_color = res.Ambient + light.Diffuse + light.Specular;
    lit_color.a = gMaterial.Diffuse.a;
    return lit_color;
}