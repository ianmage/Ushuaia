$input v_texcoord0

#include "../common/common.sh"

SAMPLER2D(S_deviceDepth, 0);

void main()
{
	float deviceZ = texture2D(S_deviceDepth, v_texcoord0).x;

#if BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL
#else
	deviceZ = deviceZ * 2.0 - 1.0;
#endif // BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_PSSL || BGFX_SHADER_LANGUAGE_METAL

	float q = far / (far - near);
	gl_FragColor.x = near * q / (q - deviceZ);
}
