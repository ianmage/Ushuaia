#ifndef DEFERRED
$input v_normal, v_pos
#else
$input v_normal
#endif

#include "../common/common.sh"
#ifndef DEFERRED
#include "../lighting/forward_rendering.sh"
#include "fog.sh"
#endif

uniform vec4 PM_albedoMetal;
uniform vec4 PM_normalGloss;


void main()
{
	vec3 n = normalize(v_normal);

#ifndef DEFERRED

	vec3 v = v_pos;

	vec3 final = ShadingFS(n, v, PM_albedoMetal, PM_normalGloss);
	
	//Fog.
	vec3 fogColor = vec3_splat(0.0);
	float fogFactor = CalcFogFactor(length(v));
	
	final = mix(fogColor, final, fogFactor);
	
	gl_FragColor.xyz = toGamma(final);
	//gl_FragColor.xyz = PV_lightDirColor.xyz;

	gl_FragColor.w = 1.0;

#else	// DEFERRED

	gl_FragData[0].xy = encodeNormalSphereMap(n);
	gl_FragData[0].w = PM_normalGloss.w;

	gl_FragData[1] = PM_albedoMetal;

#endif	// DEFERRED
}
