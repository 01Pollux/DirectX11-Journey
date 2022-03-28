
cbuffer SquareMVP : register(b0)
{
    matrix Square_Transform;
};

float4 main(float3 positon : POSITION) : SV_POSITION
{
    return mul(float4(positon, 1.0f), Square_Transform);
}