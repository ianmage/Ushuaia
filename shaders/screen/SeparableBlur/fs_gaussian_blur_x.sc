$input v_tc0

#include "../common/common.sh"

uniform vec4 uTexSize;
uniform vec4 colorWeights[2];
uniform vec4 tcOffsets[2];

SAMPLER2D(s_tex0, 0);


void main()
{
	vec2 uv = v_tc0;
	float offset = (floor(uv.x * uTexSize.x) + 0.5f) * uTexSize.y - uv.x;
	offset += uv.x;

	vec4 color = vec4(0, 0, 0, 1);

	vec4 tc1 = tcOffsets[0] + vec4_splat(offset);
	vec4 tc2 = tcOffsets[1] + vec4_splat(offset);
	vec2 tc;

	tc = vec2(tc1.x, uv.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[0].x;
	tc = vec2(tc1.y, uv.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[0].y;
	tc = vec2(tc1.z, uv.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[0].z;
	tc = vec2(tc1.w, uv.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[0].w;
	tc = vec2(tc2.x, uv.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[1].x;
	tc = vec2(tc2.y, uv.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[1].y;
	tc = vec2(tc2.z, uv.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[1].z;
	tc = vec2(tc2.w, uv.y);
	color.rgb += texture2D(s_tex0, tc).rgb * colorWeights[1].w;

	gl_FragColor = color;
}
