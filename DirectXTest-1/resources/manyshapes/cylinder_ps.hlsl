
struct PSInput
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(PSInput ps_input) : SV_Target
{
    return ps_input.Color;
}