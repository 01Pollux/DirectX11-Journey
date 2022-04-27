
cbuffer WorldConstantBuffers
{
	matrix g_World;
	matrix g_ViewProj;
	
	float3 g_EyePosition;
};


struct VSInput
{
	float3 PosL : Position;
};

struct HSInput
{
	float3 PosL : Position;
};

#define NUM_CONTROL_POINTS 16

struct DSPatchOutput
{
	float TessFactor[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

struct DSControlPoints
{
	float3 PosL : Position;
};


struct PSInput
{
	float4 PosL : SV_Position;
};


float4 CalcCubicBezierBasis(float t)
{
	float inv_t = 1.f - t;
	return float4(
		inv_t * inv_t * inv_t, // (1 - t)^3
		3 * t * inv_t * inv_t, // 3t(1 - t)^2
		3 * t * t * inv_t, // 3(1 - t)t^2
		t * t * t // t ^3
	);
}


float4 CalcCubicBezierBasis_Derivative(float t)
{
	float inv_t = 1.f - t;
	return float4(
		-3 * inv_t * inv_t,			 // -3(1 - t)^2
		inv_t * (3 * inv_t - 6 * t), // 3(1 - t)^2 - 6t(1 - t)
		6 * inv_t - 3 * t * t,		 // 6(1 - t) - 3t^2
		3 * t * t					 // 3t^2
	);
}

float3 CalcCubicBezier(
	const OutputPatch<DSControlPoints, NUM_CONTROL_POINTS> control_points,
	float4 basisu, 
	float4 basisv
)
{
	float3 sum = (float3) 0.f;
	for (uint i = 0; i < NUM_CONTROL_POINTS / 4; i++)
	{
		sum += basisv[i] * (
			basisu.x * control_points[i * 4 + 0].PosL +
			basisu.y * control_points[i * 4 + 1].PosL +
			basisu.z * control_points[i * 4 + 2].PosL +
			basisu.w * control_points[i * 4 + 3].PosL
		);
	}
	return sum;
}
