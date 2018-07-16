$input v_normal, v_pos, v_texcoord0

#include "../common/common.sh"
#include "forward_rendering.sh"

uniform vec4 PM_albedoMetal;
uniform vec4 PM_normalGloss;
SAMPLER2D(S_albedoTex, 0);


void main()
{
	vec3 v = v_pos;
	vec3 n = normalize(v_normal);

	//Fog.
	vec3 fogColor = vec3_splat(0.0);
	float fogDensity = 0.0035;
	float LOG2 = 1.442695;
	float z = length(v);
	float fogFactor = clamp(1.0/exp2(fogDensity*fogDensity*z*z*LOG2), 0.0, 1.0);
	/*
	vec3 final = toGamma(abs(ambient + brdf));
	*/
	//vec3 final = v_normal * 0.5 + 0.5;

	vec3 albedo = texture2D(S_albedoTex, v_texcoord0).xyz;
	vec4 mtlAlbedoMetal = vec4(albedo, PM_albedoMetal.w);
	vec3 final = ShadingFS(n, v, mtlAlbedoMetal, PM_normalGloss);

	gl_FragColor.xyz = mix(fogColor, final, fogFactor);
	gl_FragColor.w = 1.0;
}
