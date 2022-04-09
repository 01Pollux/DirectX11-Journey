
#include "light_test.hlsli"

float4 main(PSInput ps_input) : SV_Target
{
    ps_input.Normal = normalize(ps_input.Normal);
    float3 position_normal = normalize(gWorldEyePosition - ps_input.PosW);
    
    LightRes light;
    
    switch (gLightType)
    {
    case 0:
        Compute_DirectionalLight(gDirLight, gMaterial, ps_input.Normal, position_normal, light);
        break;
    
    case 1:
        Compute_PointLight(gPtLight, gMaterial, ps_input.PosW, ps_input.Normal, position_normal, light);
        break;
    
    case 2:
        Compute_SpotLight(gSpLight, gMaterial, ps_input.PosW, ps_input.Normal, position_normal, light);
        break;
    
    default: return float4(1.f, 0.f, 1.f, 1.f);
    }
    
    float4 lit_color = light.Ambient + light.Diffuse + light.Specular;
    lit_color.a = gMaterial.Diffuse.a;
    return lit_color;
}