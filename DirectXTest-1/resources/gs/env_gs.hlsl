
#include "env.hlsli"

void SubdivideTriangle(
    in GSInput init_verts[3],
    out GSInput new_verts[6]
)
{
    GSInput mx[3];

    mx[0].PosW = 0.5f * (init_verts[0].PosW + init_verts[1].PosW);
    mx[1].PosW = 0.5f * (init_verts[1].PosW + init_verts[2].PosW);
    mx[2].PosW = 0.5f * (init_verts[2].PosW + init_verts[0].PosW);
 
    mx[0].Normal = normalize(mx[0].PosW);
    mx[1].Normal = normalize(mx[1].PosW);
    mx[2].Normal = normalize(mx[2].PosW);

    mx[0].TexCoord = 0.5f * (init_verts[0].TexCoord + init_verts[1].TexCoord);
    mx[1].TexCoord = 0.5f * (init_verts[1].TexCoord + init_verts[2].TexCoord);
    mx[2].TexCoord = 0.5f * (init_verts[2].TexCoord + init_verts[0].TexCoord);

    new_verts[0] = init_verts[0];
    new_verts[1] = mx[0];
    new_verts[2] = mx[2];
    new_verts[3] = mx[1];
    new_verts[4] = init_verts[2];
    new_verts[5] = init_verts[1];
};

void OutputNewTriangle(
    in GSInput gs_input[6],
    inout TriangleStream<PSInput> triangle_stream
)
{
    PSInput ps_input[6];

	[unroll]
    for (int i = 0; i < 6; ++i)
    {
        ps_input[i].PosH = mul(float4(gs_input[i].PosW, 1.f), gWorldViewProj);
        ps_input[i].PosW = mul(float4(gs_input[i].PosW, 1.f), gWorld).xyz;
        ps_input[i].Normal = mul(gs_input[i].Normal, (float3x3) gWOrldInvTranspose);
        ps_input[i].TexCoord = gs_input[i].TexCoord;
    }
		
	[unroll]
    for (int j = 0; j < 5; ++j)
    {
        triangle_stream.Append(ps_input[j]);
    }
    triangle_stream.RestartStrip();
	
    triangle_stream.Append(ps_input[1]);
    triangle_stream.Append(ps_input[5]);
    triangle_stream.Append(ps_input[3]);
}

[maxvertexcount(8)]
void main(triangle GSInput gs_input[3], inout TriangleStream<PSInput> triStream)
{
    if (true) 
    {
        GSInput v[6];
        SubdivideTriangle(gs_input, v);
        OutputNewTriangle(v, triStream);
    }
    else 
    {
        PSInput ps_input;
		[unroll]
        for (int i = 0; i < 3; ++i)
        {
            ps_input.PosH = mul(float4(gs_input[i].PosW, 1.0f), gWorldViewProj);
            ps_input.PosW = mul(float4(gs_input[i].PosW, 1.0f), gWorld).xyz;
            ps_input.Normal = mul(gs_input[i].Normal, (float3x3) gWOrldInvTranspose);
            ps_input.TexCoord = gs_input[i].TexCoord;

            triStream.Append(ps_input);
        }
    }
}
