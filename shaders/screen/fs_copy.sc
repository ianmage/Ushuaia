$input v_tc0

#include "../common/common.sh"


SAMPLER2D(s_tex0, 0);


void main()
{
	vec2 uv = v_tc0.xy;

	gl_FragColor = texture2D(s_tex0, uv);
}
