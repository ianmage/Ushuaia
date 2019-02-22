$input v_tc0

#include "../common/common.sh"

uniform vec4 uParam;

SAMPLER2D(s_src_tex, 0);
SAMPLER2D(s_bloom_tex, 1);

#define RGB_TO_LUM	vec3(0.2126f, 0.7152f, 0.0722f)
#define bloomStrength	uParam.x
#define hdrRescale	uParam.y
#define ENABLE_BLUE_SHIFT	true


float EyeAdaption(float lum)
{
	return mix(0.2f, lum, 0.5f);
}


vec3 ACESFilm(vec3 x)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	return (x * (A*x + B)) / (x * (C*x + D) + E);
}


vec3 ToneMapping(vec3 color, vec3 blur, float adaptedLum)
{
	color += blur * bloomStrength;

#if ENABLE_BLUE_SHIFT
	// --- Blue Shift Begin --- //
	const vec3 BLUE_SHIFT = vec3(0.4f, 0.4f, 0.7f);
	float lum = dot(color, RGB_TO_LUM);
	// martin's modified blue Shift
	color = lerp(lum * BLUE_SHIFT, color, saturate(16.f * lum));
	// --- Blue Shift End --- //
#endif

	float adaptedLumDst = 2 / (max(0.1f, 1 + 10 * EyeAdaption(adaptedLum)));
	return ACESFilm(adaptedLumDst * hdrRescale * color);
}


void main()
{
	vec2 uv = v_tc0.xy;

	vec3 srcColor = texture2D(s_src_tex, uv).rgb;
	vec3 bloomColor = texture2D(s_bloom_tex, uv).rgb;
	vec3 ldr_rgb = saturate(ToneMapping(srcColor, bloomColor, v_tc0.z));

	gl_FragColor = vec4(ldr_rgb, dot(toGamma(ldr_rgb), RGB_TO_LUM));
}
