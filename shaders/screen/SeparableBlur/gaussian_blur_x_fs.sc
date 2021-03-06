$input v_tc0

#include "../common/common.sh"

uniform vec4 texSize;
uniform vec4 tcOffsets[2];
uniform vec4 colorWeights[2];

SAMPLER2D(src_tex, 0);


void main()
{
	vec2 uv = v_tc0.xy;
	float offset = (floor(uv.x * texSize.x) + 0.5f) * texSize.z - uv.x;
	offset += uv.x;

	vec4 color = vec4(0, 0, 0, 1);

	vec4 tc1 = tcOffsets[0] * texSize.z + vec4_splat(offset);
	vec4 tc2 = tcOffsets[1] * texSize.z + vec4_splat(offset);
	vec2 tc;

	tc = vec2(tc1.x, uv.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[0].x;
	tc = vec2(tc1.y, uv.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[0].y;
	tc = vec2(tc1.z, uv.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[0].z;
	tc = vec2(tc1.w, uv.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[0].w;
	tc = vec2(tc2.x, uv.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[1].x;
	tc = vec2(tc2.y, uv.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[1].y;
	tc = vec2(tc2.z, uv.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[1].z;
	tc = vec2(tc2.w, uv.y);
	color.rgb += texture2D(src_tex, tc).rgb * colorWeights[1].w;

	gl_FragColor = color;
}
