$input v_texcoord0

#include "../common/common.sh"

SAMPLER2D(S_shadeTex, 0);


void main()
{
	vec4 shade = texture2D(S_shadeTex, v_texcoord0);

	gl_FragColor = toGamma(shade);
}
