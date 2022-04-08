
struct PSInput
{
    float4 PosH : SV_POSITION;
};

float4 main(PSInput ps_input) : SV_Target
{
    return float4(1.f, 0.f, 0.5f, 1.f);
}