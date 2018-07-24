$input v_texcoord0

#include "../common/common.sh"

SAMPLER2D(S_albedoTex, 0);
SAMPLER2D(S_normalTex, 1);


void main()
{
	vec3 albedo = texture2D(S_albedoTex, v_texcoord0).xyz;
	vec3 normG = texture2D(S_normalTex, v_texcoord0).xyz;
	vec3 normal = decodeNormalSphereMap(normG.xy);

	vec3 final = toGamma(albedo);
final = vec3(v_texcoord0, 1);
	gl_FragColor = vec4(final, 1.0);
}
