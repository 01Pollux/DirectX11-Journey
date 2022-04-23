
struct VecAdd_t
{
    float3 v0;
    float2 v1;
};

StructuredBuffer<VecAdd_t> gVecAddA : register(t0);
StructuredBuffer<VecAdd_t> gVecAddB : register(t1);
RWStructuredBuffer<VecAdd_t> gOutputBuf : register(u0);

[numthreads(32, 1, 1)]
void main(int gid : SV_GroupIndex)
{
    gOutputBuf[gid].v0 = gVecAddA[gid].v0 + gVecAddB[gid].v0;
    gOutputBuf[gid].v1 = gVecAddA[gid].v1 + gVecAddB[gid].v1;
}