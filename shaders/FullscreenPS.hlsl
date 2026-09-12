Texture2D virtualScreen : register(t0);
SamplerState pointSampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 uv : TEXCOORD0)
    : SV_TARGET
{
    return virtualScreen.Sample(
        pointSampler,
        uv
    );
}