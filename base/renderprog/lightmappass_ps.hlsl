Texture2D txDiffuse : register( t0 );
Texture2D txLightmap : register( t1 );
SamplerState samLinear : register( s0 );
SamplerState samLinear2 : register( s1 );

#include "global.inc"


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSMain( PS_INPUT input) : SV_Target
{
    float4 color = txDiffuse.Sample( samLinear, input.Tex );
	
	color.rgb *= clamp(1.0 - txLightmap.Sample( samLinear2, input.lightmapST ).r, 0, 1);

	return color;
}
