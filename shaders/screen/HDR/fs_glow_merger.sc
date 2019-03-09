$input v_tc0

#include "../common/common.sh"

SAMPLER2D(glow_tex_0, 0);
SAMPLER2D(glow_tex_1, 1);


void main()
{
	vec2 uv = v_tc0.xy;

	vec4 clr0 = texture2D(glow_tex_0, uv);
	vec4 clr1 = texture2D(glow_tex_1, uv);

	gl_FragColor = clr0 * 2.575f + clr1 * 1.025f;
}
