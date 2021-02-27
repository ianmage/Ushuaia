$input v_tc0

#include "../common/common.sh"

uniform vec4 uParam;

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);


void main()
{
	vec2 uv = v_tc0.xy;
	uv = (uv - uParam.xy) / uParam.zw;

	vec3 final = texture2D(s_tex0, uv).xyz;
	final = toGamma(final);

	float depth = texture2D(s_tex1, uv).x;
	// final = vec3_splat(depth / 300);

	gl_FragColor = vec4(final, 1.0);
}
