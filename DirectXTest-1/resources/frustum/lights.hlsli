
struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Reflect;
};

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
    
	float3 Direction;
	float _Pad;
};

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Attenuation;
	float _Pad;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;
    
	float3 Direction;
	float ExpFactor;

	float3 Attenuation;
	float _Pad;
};


struct LightRes
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};


void Compute_DirectionalLight(
    DirectionalLight light,
    Material mat,
    float3 surface_normal,
    float3 position_normal,
    out LightRes res
)
{
	res = (LightRes) 0;
	res.Ambient = light.Ambient * mat.Ambient;
    
	float diffuse_factor = dot(-light.Direction, surface_normal);
	if (diffuse_factor > 0.f)
	{
		float3 reflected = reflect(light.Direction, surface_normal);
		float spec_factor = pow(max(dot(position_normal, reflected), 0.f), mat.Specular.w);
        
		res.Specular = spec_factor * light.Specular * mat.Specular;
		res.Diffuse = diffuse_factor * light.Diffuse * mat.Diffuse;
	}
}


void Compute_PointLight(
    PointLight light,
    Material mat,
    float3 position,
    float3 surface_normal,
    float3 position_normal,
    out LightRes res
)
{
	res = (LightRes) 0;
	float3 to_light = light.Position - position;
    
	float distance = length(to_light);
	if (distance > light.Range)
		return;
   
	res.Ambient = light.Ambient * mat.Ambient;
   
	to_light /= distance;
	float diffuse_factor = dot(to_light, surface_normal);

	if (diffuse_factor > 0.f)
	{
		float3 reflected = reflect(-to_light, surface_normal);
		float spec_factor = pow(max(dot(position_normal, reflected), 0.f), mat.Specular.w);
        
		float attenuation = 1.f / dot(light.Attenuation, float3(1.f, distance, distance * distance));
        
		res.Specular = spec_factor * light.Specular * mat.Specular * attenuation;
		res.Diffuse = diffuse_factor * light.Diffuse * mat.Diffuse * attenuation;
	}
}


void Compute_SpotLight(
    SpotLight light,
    Material mat,
    float3 position,
    float3 surface_normal,
    float3 position_normal,
    out LightRes res
)
{
	res = (LightRes) 0;
	float3 to_light = light.Position - position;
    
	float dist = length(to_light);
	if (dist > light.Range)
		return;
    
	to_light /= dist;
    
	res.Ambient = light.Ambient * mat.Ambient;
	float diffuse_factor = dot(to_light, surface_normal);
    
	if (diffuse_factor > 0.f)
	{
		float3 reflected = reflect(-to_light, surface_normal);
		float spec_factor = pow(max(dot(reflected, position_normal), 0.f), mat.Specular.w);
        
		float spot_factor = pow(max(dot(-to_light, light.Direction), 0.f), light.ExpFactor);
		float attenuation = spot_factor / dot(light.Attenuation, float3(1.f, dist, dist * dist));

		res.Ambient *= spot_factor;
		res.Diffuse = light.Diffuse * mat.Diffuse * diffuse_factor * attenuation;
		res.Specular = light.Specular * float4(mat.Specular.rgb, 1.f) * spec_factor * attenuation;
	}
}