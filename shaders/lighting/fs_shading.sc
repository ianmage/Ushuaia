$input v_tc0

#include "../common/common.sh"
#include "pbr_lighting.sh"
#include "util.sh"

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);
SAMPLER2D(s_tex2, 2);


void main()
{
	vec2 uv = v_tc0.xy;

	vec4 normG = texture2D(s_tex0, uv);
	vec3 normal = decodeNormalSphereMap(normG.xy);
	float shiny = Glossiness2Shininess(normG.w);

	vec4 baseM = texture2D(s_tex1, uv);
	vec3 albedo = baseM.rgb;
	float metal = baseM.a;
	vec3 diff = albedo * (1 - metal);
	vec3 spec = mix(vec3_splat(0.04), albedo, vec3_splat(metal));

	vec3 viewDir = normalize(vec3(v_tc0.zw, 1));

	vec4 lighting = texture2D(s_tex2, uv);

	vec3 shading = CalcShading(lighting, shiny, diff, spec, viewDir, normal);

	// shading = normal * 0.5 + 0.5;

	gl_FragColor = vec4(shading, 1.0);
}
