
StructuredBuffer<float3> g_InVec;
AppendStructuredBuffer<float> g_OutVec;

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	g_OutVec.Append(length(g_InVec[DTid.x]));
}