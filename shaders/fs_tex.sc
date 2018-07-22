$input v_normal, v_pos, v_texcoord0

#include "../common/common.sh"
#include "forward_rendering.sh"
#include "fog.sh"

uniform vec4 PM_albedoMetal;
uniform vec4 PM_normalGloss;
SAMPLER2D(S_albedoTex, 0);


void main()
{
	vec3 v = v_pos;
	vec3 n = normalize(v_normal);

#if 1
	gl_FragColor.xyz = PV_lightDirDir.xyz * 0.5 + 0.5;
#else
	vec3 albedo = texture2D(S_albedoTex, v_texcoord0).xyz;
	vec4 mtlAlbedoMetal = vec4(albedo, PM_albedoMetal.w);
	vec3 final = ShadingFS(n, v, mtlAlbedoMetal, PM_normalGloss);

	//Fog.
	vec3 fogColor = vec3_splat(0.0);
	float fogFactor = CalcFogFactor(length(v));
	
	gl_FragColor.xyz = mix(fogColor, final, fogFactor);
#endif
	gl_FragColor.w = 1.0;
}
