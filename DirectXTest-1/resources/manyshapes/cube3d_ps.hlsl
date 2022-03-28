
cbuffer TestRegisters : register(b0)
{
    float2 Dummy[2];
    matrix DummyMatrix[2];
};

cbuffer SingleColorInSquare : register(b1)
{
    float4 FaceColor[6];
};

void main(
    out float4 face_color : SV_TARGET,
    uint id : SV_PRIMITIVEID
)
{
    // Since we're drawing two triangles to make a square,
    // There will be two primitives seperating each color
    face_color = FaceColor[id / 2];
}