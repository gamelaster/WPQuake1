// 2d_albedo.vp
//

#include "global.inc"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( float4(input.Pos.x, input.Pos.y, input.Pos.z, 1), World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = input.Tex;
	output.lightmapST = input.LightmapST;
	output.color = input.Color;
    
    return output;
}

