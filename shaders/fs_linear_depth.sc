$input v_texcoord0

#include "../common/common.sh"

SAMPLER2D(s_tex0, 0);

void main()
{
	float deviceZ = texture2D(s_tex0, v_texcoord0).x;

#if BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL
#else
	deviceZ = deviceZ * 2.0 - 1.0;
#endif // BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL

	float far = 1000, near = 0;
	float q = far / (far - near);
	float lz = near * q / (q - deviceZ);
	lz = deviceZ * 100 - 99;
	gl_FragColor = vec4(lz, lz, lz, 1.0);
}
