$input v_tc0, v_tc1

#include "../common/common.sh"

SAMPLER2D(src_tex, 0);


void main()
{
	vec4 s = vec4_splat(0);
	s += texture2D(src_tex, v_tc0.xy);
	s += texture2D(src_tex, v_tc0.zw);
	s += texture2D(src_tex, v_tc1.xy);
	s += texture2D(src_tex, v_tc1.zw);

	gl_FragColor = s / 4;
}
