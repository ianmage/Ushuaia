$input v_tc0

#include "../common/common.sh"

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);
SAMPLER2D(s_tex2, 2);


void main()
{
	vec3 normG = texture2D(s_tex0, v_tc0.xy).xyz;
	vec3 normal = decodeNormalSphereMap(normG.xy);

	float depth = texture2D(s_tex1, v_tc0.xy).x;
	vec3 vPos = vec3(depth * v_tc0.zw, depth);

	vec4 lighting = texture2D(s_tex2, v_tc0.xy);

	//vec3 final = normal * 0.5 + 0.5;
	vec3 final = toGamma(lighting.xyz);
	//vec3 final = vec3_splat(depth / 300);

	gl_FragColor = vec4(final, 1.0);
}
