$input v_tc0, v_tc1, v_tc2

#include "../common/common.sh"

uniform vec4 uTexSize;
uniform vec4 colorWeights[2];

SAMPLER2D(s_tex0, 0);


void main()
{
	vec2 uv = v_tc0;
	float offset = (floor(uv.y * uTexSize.x) + 0.5f) * uTexSize.y - uv.y;

	vec4 color = vec4(0, 0, 0, 1);

	vec4 tc1 = v_tc1 + vec4_splat(offset);
	vec4 tc2 = v_tc2 + vec4_splat(offset);
	vec2 tc;

	tc = vec2(uv.x, tc1.x);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[0].x;
	tc = vec2(uv.x, tc1.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[0].y;
	tc = vec2(uv.x, tc1.z);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[0].z;
	tc = vec2(uv.x, tc1.w);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[0].w;
	tc = vec2(uv.x, tc2.x);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[1].x;
	tc = vec2(uv.x, tc2.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[1].y;
	tc = vec2(uv.x, tc2.z);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[1].z;
	tc = vec2(uv.x, tc2.w);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[1].w;

	gl_FragColor = color;
}
