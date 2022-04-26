
static const float g_Weights[] =
{
    .05f, .05f, 
        .1f, .1f,
            .1f,
                .2f,
            .1f,
        .1f, .1f, 
    .05f, .05f  
};

#define RadiusSize 5
#define N 256
#define CacheSize (N + 2 * RadiusSize)

groupshared float4 g_BlurredTex[CacheSize];

Texture2D g_TexInput;
RWTexture2D<float4> g_TexOutput;

[numthreads(N, 1, 1)]
void main(
    uint3 GTid : SV_GroupThreadID,
    uint3 DTid : SV_DispatchThreadID
)
{
    int2 size;
    g_TexInput.GetDimensions(size.x, size.y);
    
    if (GTid.x < RadiusSize)
    {
        int x = max(DTid.x - RadiusSize, 0);
        g_BlurredTex[GTid.x] = g_TexInput[int2(x, DTid.y)];
    }
    else if (GTid.x > N - RadiusSize)
    {
        int x = min(DTid.x + RadiusSize, size.x - 1);
        g_BlurredTex[GTid.x + 2 * RadiusSize] = g_TexInput[int2(x, DTid.y)];
    }
    
    g_BlurredTex[GTid.x + RadiusSize] = g_TexInput[min(DTid.xy, size - 1)];
    
    GroupMemoryBarrierWithGroupSync();
    
    float4 blur_color = (float4) 0;
    
    [unroll]
    for (int i = -RadiusSize; i <= RadiusSize; i++)
    {
        int k = GTid.x + RadiusSize + i;
        blur_color += g_Weights[i + RadiusSize] * g_BlurredTex[k];
    }
 
    g_TexOutput[DTid.xy] = blur_color;
}