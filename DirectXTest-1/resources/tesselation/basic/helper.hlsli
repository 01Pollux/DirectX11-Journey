
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

#define NUM_CONTROL_POINTS 3

struct DSPatchOutput
{
	float TessFactor[3] : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

struct DSControlPoints
{
	float3 PosL : Position;
};


struct PSInput
{
	float4 PosL : SV_Position;
};