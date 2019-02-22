$input v_tc0, v_tc1

#include "../common/common.sh"

uniform vec4 uParam;

SAMPLER2D(s_src_tex, 0);


void main()
{
	vec4 inTC[2] = { v_tc0, v_tc1 };

	float s = 0;
	float l;
	for (int i = 0; i < 2; ++i) {
		l = texture2D(s_src_tex, inTC[i].xy).r;
		s += l;
		l = texture2D(s_src_tex, inTC[i].zw).r;
		s += l;
	}

	gl_FragColor = vec4_splat(s / 4);
}
