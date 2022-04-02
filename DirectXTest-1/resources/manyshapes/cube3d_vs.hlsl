
cbuffer SquareWVP : register(b0)
{
    matrix gWorldViewProj;
};

float4 main(float3 positon : POSITION) : SV_POSITION
{
    return mul(float4(positon, 1.0f), gWorldViewProj);
}