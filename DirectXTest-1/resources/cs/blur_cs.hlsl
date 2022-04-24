
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

[numthreads(1, 1, 1)]
void main(
    uint3 GTid : SV_GroupThreadID,
    uint3 DTid : SV_DispatchThreadID
)
{
    
}