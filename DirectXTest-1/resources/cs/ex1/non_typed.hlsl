
StructuredBuffer<float3> g_InVec;
RWStructuredBuffer<float> g_OutVec;

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	g_OutVec[DTid.x] = length(g_InVec[DTid.x]);
}