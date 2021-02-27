$input v_tc0

#include "../common/common.sh"


SAMPLER2D(src_tex, 0);


void main()
{
	vec2 uv = v_tc0.xy;

	gl_FragColor = texture2D(src_tex, uv);
}
