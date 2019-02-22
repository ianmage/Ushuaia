$input v_tc0, v_tc1

#include "../common/common.sh"

uniform vec4 uParam;

SAMPLER2D(s_src_tex, 0);

#define RGB_TO_LUM	vec3(0.2126f, 0.7152f, 0.0722f)


void main()
{
	vec4 inTC[2] = { v_tc0, v_tc1 };

	float s = 0;
	vec3 c;
	float l;
	for (int i = 0; i < 2; ++i) {
		c = texture2D(s_src_tex, inTC[i].xy).rgb;
		l = dot(c, RGB_TO_LUM);
		s += log(l + 0.001f);
		c = texture2D(s_src_tex, inTC[i].zw).rgb;
		l = dot(c, RGB_TO_LUM);
		s += log(l + 0.001f);
	}

	gl_FragColor = vec4_splat(s / 4);
}
