
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

[numthreads(1, N, 1)]
void main(
    uint3 GTid : SV_GroupThreadID,
    uint3 DTid : SV_DispatchThreadID
)
{
    int2 size;
    g_TexInput.GetDimensions(size.x, size.y);
    
    if (GTid.y < RadiusSize)
    {
        int y = max(DTid.y - RadiusSize, 0);
        g_BlurredTex[GTid.y] = g_TexInput[int2(DTid.x, y)];
    }
    else if (GTid.y >= N - RadiusSize)
    {
        int y = min(DTid.y + RadiusSize, size.y - 1);
        g_BlurredTex[GTid.y + 2 * RadiusSize] = g_TexInput[int2(DTid.x, y)];
    }
    
    g_BlurredTex[GTid.y + RadiusSize] = g_TexInput[min(DTid.xy, size - 1)];
    
    GroupMemoryBarrierWithGroupSync();
    
    float4 blur_color = (float4) 0;
    
    [unroll]
    for (int i = -RadiusSize; i <= RadiusSize; i++)
    {
        int k = GTid.y + RadiusSize + i;
        blur_color += g_Weights[i + RadiusSize] * g_BlurredTex[k];
    }
 
    g_TexOutput[DTid.xy] = blur_color;
}