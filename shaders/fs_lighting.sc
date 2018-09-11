$input v_texcoord0

#include "../common/common.sh"

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);
SAMPLER2D(s_tex2, 2);


void main()
{
	vec3 normG = texture2D(s_tex0, v_texcoord0.xy).xyz;
	vec3 normal = decodeNormalSphereMap(normG.xy);

	vec3 albedo = texture2D(s_tex1, v_texcoord0.xy).xyz;

	float depth = texture2D(s_tex2, v_texcoord0.xy).x;
	vec3 vPos = vec3(depth * v_texcoord0.zw, depth);

	vec3 final = normal * 0.5 + 0.5;
	//vec3 final = toGamma(albedo);
	//vec3 final = vec3_splat(depth / 300);

	gl_FragColor = vec4(final, 1.0);
}
