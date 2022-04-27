
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

#define NUM_CONTROL_POINTS 4

struct DSPatchInput
{
	float TessFactor[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

struct DSConstInput
{
	float3 PosL : Position;
};


struct PSInput
{
	float4 PosL : SV_Position;
};