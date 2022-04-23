
struct VecAdd_t
{
    float3 v0;
    float2 v1;
};

StructuredBuffer<VecAdd_t> gVecAddA : register(t0);
StructuredBuffer<VecAdd_t> gVecAddB : register(t1);
RWStructuredBuffer<VecAdd_t> gOutputBuf : register(u0);

[numthreads(32, 1, 1)]
void main(uint3 dispatch_id : SV_DispatchThreadID)
{
    gOutputBuf[dispatch_id.x].v0 = gVecAddA[dispatch_id.x].v0 + gVecAddB[dispatch_id.x].v0;
    gOutputBuf[dispatch_id.x].v1 = gVecAddA[dispatch_id.x].v1 + gVecAddB[dispatch_id.x].v1;
}