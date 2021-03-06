$input v_tc0

#include "../common/common.sh"

uniform vec4 texSize;
uniform vec4 tcOffsets[2];
uniform vec4 colorWeights[2];

SAMPLER2D(src_tex, 0);


void main()
{
	vec2 uv = v_tc0.xy;
	float offset = (floor(uv.y * texSize.y) + 0.5f) * texSize.w - uv.y;
	offset += uv.y;

	vec4 color = vec4(0, 0, 0, 1);

	vec4 tc1 = tcOffsets[0] * texSize.w + vec4_splat(offset);
	vec4 tc2 = tcOffsets[1] * texSize.w + vec4_splat(offset);
	vec2 tc;

	tc = vec2(uv.x, tc1.x);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[0].x;
	tc = vec2(uv.x, tc1.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[0].y;
	tc = vec2(uv.x, tc1.z);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[0].z;
	tc = vec2(uv.x, tc1.w);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[0].w;
	tc = vec2(uv.x, tc2.x);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[1].x;
	tc = vec2(uv.x, tc2.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[1].y;
	tc = vec2(uv.x, tc2.z);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[1].z;
	tc = vec2(uv.x, tc2.w);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[1].w;

	gl_FragColor = color;
}
