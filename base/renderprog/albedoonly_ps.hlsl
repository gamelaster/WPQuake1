Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

#include "global.inc"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSMain( PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex );
}