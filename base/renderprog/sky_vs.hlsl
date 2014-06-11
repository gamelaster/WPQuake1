// sky_vs.hlsl
//

#include "global.inc"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
	float3 dir = input.Pos.xyz - float3(origin_x, origin_y, origin_z);

	dir[2] *= 3;	// flatten the sphere

	float length = dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2];
	length = sqrt (length);
	length = 6*63/length;

	dir[0] *= length;
	dir[1] *= length;

	float s = (realtime + dir[0]) * (1.0/128);
	float t = (realtime + dir[1]) * (1.0/128);

    output.Pos = mul( float4(input.Pos.x, input.Pos.y, input.Pos.z, 1), World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = float2(s, t);
	output.color = input.Color;
    
    return output;
}
