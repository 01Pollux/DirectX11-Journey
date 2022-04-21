
#include "env.hlsli"

float4 main(PSInput ps_input) : SV_Target
{
    float3 to_eye = gWorldEyePosition - ps_input.PosW;
    float dist_to_eye = length(to_eye);
    
    float3 position_normal = to_eye / dist_to_eye;
    
    LightRes res;
    Compute_DirectionalLight(gDirLight, gMaterial, ps_input.Normal, position_normal, res);
    
    float4 lit_color = (res.Ambient + res.Diffuse) + res.Specular;
    lit_color.a = gMaterial.Diffuse.a;
    
    return lit_color;
}