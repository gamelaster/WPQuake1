// liquid_vs.hlsl
//

#include "global.inc"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSMain( VS_INPUT input )
{
	float2 st;
	int lookupX = (int)((input.Tex.y*0.125+realtime) * TURBSCALE);
	int lookupY = (int)((input.Tex.x*0.125+realtime) * TURBSCALE);

	if(lookupX > 255) {
		lookupX = 255 - lookupX;
	}

	if(lookupY > 255) {
		lookupY = 255 - lookupY;
	}

	st.x = input.Tex.x + turbsin[lookupX];
	st.x *= (1.0/64);

	st.y = input.Tex.y + turbsin[lookupY];
	st.y *= (1.0/64);

    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( float4(input.Pos.x, input.Pos.y, input.Pos.z, 1), World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Tex = st;
	output.color = input.Color;
    
    return output;
}
