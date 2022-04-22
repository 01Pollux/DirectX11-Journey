
#include "env.hlsli"

/*
3---------1
|    |    |
|----x----|
|    |    |
0---------2
     
clockwise:
012
031

*/
static const float2 s_TexCoords[] =
{
    { 0.f, 1.f },
    { 1.f, 0.f },
    { 1.f, 1.f },
    { 0.f, 0.f }
};

[maxvertexcount(6)]
void main(
    point GSInput gs_input[1],
    inout TriangleStream<PSInput> triangle_stream
)
{
    float3 up = float3(0.f, 1.f, 0.f);
    float3 look = gWorldEyePosition - gs_input[0].PosW;
    look.y = 0.f; look = normalize(look);
    float3 right = cross(look, up);
    
    float2 half_size = gs_input[0].Size * .5f;
    
    float3 pos[] =
    {
        gs_input[0].PosW + right * half_size.x - up * half_size.y,
        gs_input[0].PosW - right * half_size.x + up * half_size.y,
        gs_input[0].PosW - right * half_size.x - up * half_size.y,
        gs_input[0].PosW + right * half_size.x + up * half_size.y
    };
    
    PSInput ps_input[4];

    [unroll]
    for (uint i = 0; i < 4; i++)
    {
        ps_input[i].PosH = mul(float4(pos[i], 1.f), gViewProj);
        ps_input[i].PosW = pos[i];
        ps_input[i].Normal = look;
        ps_input[i].TexCoord = s_TexCoords[i];
        ps_input[i].TexId = gCurTexId;
    }
    
    triangle_stream.Append(ps_input[0]);
    triangle_stream.Append(ps_input[1]);
    triangle_stream.Append(ps_input[2]);
    
    triangle_stream.RestartStrip();
    
    triangle_stream.Append(ps_input[0]);
    triangle_stream.Append(ps_input[3]);
    triangle_stream.Append(ps_input[1]);
}
