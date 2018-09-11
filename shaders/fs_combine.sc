$input v_texcoord0

#include "../common/common.sh"

SAMPLER2D(s_tex0, 0);


void main()
{
	vec3 final = texture2D(s_tex0, v_texcoord0.xy).xyz;

	gl_FragColor = vec4(final, 1.0);
}
