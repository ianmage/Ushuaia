$input v_tc0

#include "../common/common.sh"

uniform vec4 uParam;	// near, far, ?, ?

SAMPLER2D(s_tex0, 0);


void main()
{
	vec2 uv = v_tc0.xy;
	float deviceZ = texture2D(s_tex0, uv).x;

#if BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL
#else
	deviceZ = deviceZ * 2.0 - 1.0;
#endif // BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL

#ifdef NormalizedZ	// map to 0~1, z is f-n
#ifdef ReverseZ
	//float lz = uParam.x / (uParam.x + deviceZ * uParam.z)
#else
	//float lz = uParam.x / (uParam.y - deviceZ * uParam.z);
#endif
#else	// origin depth, * far
	float lz = uParam.x / (uParam.y - deviceZ);
#endif
	gl_FragData[0] = vec4(lz, lz, lz, 1.0);
}
