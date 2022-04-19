
#include "env.hlsli"

static const float2 tex_coords[4] =
{
    float2(0.f, 1.f),
    float2(0.f, 0.f),
    float2(1.f, 0.f),
    float2(1.f, 1.f)
};


[maxvertexcount(4)]
void main(
    uint prim_id : SV_PrimitiveID,
    point GSInput gs_input[1],
    inout TriangleStream<PSInput> triangle_stream
)
{
    float3 up = float3(0.f, 1.f, 0.f);
    float3 look = gWorldEyePosition - gs_input[0].PosW;
    look.y = 0.f;
    look = normalize(look);
    float3 right = cross(look, up);
    
    float half_width = gs_input[0].Size.x * 0.5f;
    float half_height = gs_input[0].Size.y * 0.5f;
    
    float4 pts[4] =
    {
        float4(gs_input[0].PosW + half_width * right - half_height * up, 1.f),
        float4(gs_input[0].PosW + half_width * right + half_height * up, 1.f),
        float4(gs_input[0].PosW - half_width * right + half_height * up, 1.f),
        float4(gs_input[0].PosW - half_width * right - half_height * up, 1.f),
    };
    
    PSInput ps_input;
    
    for (uint i = 0; i < 4; i++)
    {
        ps_input.PosH = mul(pts[i], gViewProj);
        ps_input.PosW = pts[i].xyz;
        ps_input.Normal = look;
        ps_input.TexCoord = tex_coords[i];
        ps_input.PrimId = prim_id;

        triangle_stream.Append(ps_input);
    }
}