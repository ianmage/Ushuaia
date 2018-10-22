$input v_tc0

#include "../common/common.sh"

SAMPLER2D(s_tex0, 0);
uniform vec4 uParam;	// near, far, f-n, 1/f

void main()
{
	float deviceZ = texture2D(s_tex0, v_tc0.xy).x;

#if BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL
#else
	deviceZ = deviceZ * 2.0 - 1.0;
#endif // BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL

#if 0	// ReverseZ
	//float lz = uParam.x / (deviceZ * uParam.z + uParam.x)
#else
	//float lz = uParam.x / (-deviceZ * uParam.z + uParam.y);
	float lz = uParam.x / (uParam.y - deviceZ);
#endif
	gl_FragData[0] = vec4(lz, lz, lz, 1.0);
}
