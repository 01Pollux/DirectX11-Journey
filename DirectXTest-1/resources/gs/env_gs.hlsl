
#include "env.hlsli"

static const int gSudivideCount = 0;

void SubdivideTriangle(
    in GSInput init_verts[3],
    out GSInput new_verts[6]
)
{
    GSInput mx[3];
    
    for (uint i = 0; i < 3; i++)
    {
        mx[i].PosW = (init_verts[i].PosW + init_verts[(i + 1) % 3].PosW) * 0.5f;
        mx[i].Normal = normalize(mx[i].PosW);
        mx[i].TexCoord = (init_verts[i].TexCoord + init_verts[(i + 1) % 3].TexCoord) * 0.5f;
    }

    new_verts[0] = init_verts[0];
    new_verts[1] = mx[0];
    new_verts[2] = mx[2];
    new_verts[3] = mx[1];
    new_verts[4] = init_verts[2];
    new_verts[5] = init_verts[1];
}


void OutputNewTriangle(
    in GSInput gs_input[6],
    inout TriangleStream<PSInput> triangle_stream
)
{
    PSInput ps_input[6];
    uint i;
    
    for (i = 0; i < 6; i++)
    {
        ps_input[i].PosH = mul(float4(gs_input[i].PosW, 1.f), gWorldViewProj);
        ps_input[i].PosW = mul(float4(gs_input[i].PosW, 1.f), gWorld).xyz;
        ps_input[i].Normal = mul(gs_input[i].PosW, (float3x3) gWOrldInvTranspose);
        ps_input[i].TexCoord = gs_input[i].TexCoord;
    }

    for (i = 0; i < 5; i++)
        triangle_stream.Append(ps_input[i]);
    
    triangle_stream.RestartStrip();
    
    triangle_stream.Append(ps_input[1]);
    triangle_stream.Append(ps_input[5]);
    triangle_stream.Append(ps_input[3]);
}


[maxvertexcount(6)]
void main(
    triangle GSInput gs_input[3],
    inout TriangleStream<PSInput> triangle_stream
)
{
    GSInput new_input[6];
    switch (gSudivideCount)
    {
    default:
    {
        PSInput ps_input;
            
        for (int i = 0; i < 3; i++)
        {
            ps_input.PosH = mul(float4(gs_input[i].PosW, 1.f), gWorldViewProj);
            ps_input.PosW = mul(float4(gs_input[i].PosW, 1.f), gWorld).xyz;
            ps_input.Normal = mul(gs_input[i].PosW, (float3x3) gWOrldInvTranspose);
            ps_input.TexCoord = gs_input[i].TexCoord;
                
            triangle_stream.Append(ps_input);
        }
        return;
    }
    case 1:
    {
        SubdivideTriangle(gs_input, new_input);
        OutputNewTriangle(new_input, triangle_stream);
        return;
    }
    }
}