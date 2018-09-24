#ifndef DEFERRED
$input v_normal, v_pos, v_texcoord0
#else
$input v_normal, v_texcoord0
#endif

#include "../common/common.sh"
#ifndef DEFERRED
#include "forward_rendering.sh"
#include "fog.sh"
#endif

uniform vec4 PM_albedoMetal;
uniform vec4 PM_normalGloss;
SAMPLER2D(S_albedoTex, 0);


void main()
{
	vec3 n = normalize(v_normal);

	vec3 albedo = texture2D(S_albedoTex, v_texcoord0).xyz;
	vec4 mtlAlbedoMetal = vec4(albedo, PM_albedoMetal.w);

#ifndef DEFERRED

	vec3 v = v_pos;

	vec3 final = ShadingFS(n, v, mtlAlbedoMetal, PM_normalGloss);

	//Fog.
	vec3 fogColor = vec3_splat(0.0);
	float fogFactor = CalcFogFactor(length(v));
	
	gl_FragColor.xyz = mix(fogColor, final, fogFactor);

	gl_FragColor.w = 1.0;

#else	// DEFERRED

	gl_FragData[0].xy = encodeNormalSphereMap(n);
	gl_FragData[0].w = PM_normalGloss.w;

	gl_FragData[1] = mtlAlbedoMetal;

#endif	// DEFERRED
}
