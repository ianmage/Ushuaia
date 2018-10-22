$input v_tc0

#include "../common/common.sh"

uniform vec4 uParam;

SAMPLER2D(s_tex0, 0);


void main()
{
	vec2 uv = v_tc0.xy;
	uv = (uv - uParam.xy) / uParam.zw;
	vec3 final = texture2D(s_tex0, uv).xyz;

	gl_FragColor = vec4(final, 1.0);
}
