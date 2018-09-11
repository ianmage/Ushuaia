$input v_texcoord0

#include "../common/common.sh"

SAMPLER2D(s_tex0, 0);
uniform vec4 uParam;	// near, far, q, near*q, while q = far/(far-near)

void main()
{
	float deviceZ = texture2D(s_tex0, v_texcoord0.xy).x;

#if BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL
#else
	deviceZ = deviceZ * 2.0 - 1.0;
#endif // BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL

	float lz = uParam.w / (uParam.z - deviceZ);
	gl_FragData[0] = vec4(lz, lz, lz, 1.0);
}
